#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import os
import argparse
import shutil
import subprocess
from pathlib import Path

REPO_URL = "http://192.168.21.118/project-templates/cpp-templates"
SUBMODULE_DIR = Path("cpp-templates")
SCRIPT_DIR = Path(__file__).parent.resolve()
SCRIPT_NAME = Path(__file__).name

CPP_EXTS = {".cpp", ".h", ".cxx", ".cc", ".hpp"}


def log(msg, mode="init"):
    prefix = "[init_cpp_templates]" if mode == "init" else "[update_cpp_templates]"
    print(f"{prefix} {msg}")


def load_subprojects_param_file(path):
    if not os.path.exists(path):
        return {}
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def save_subprojects_param_file(path, param_dict):
    with open(path, "w", encoding="utf-8") as f:
        json.dump(param_dict, f, indent=2)


def should_keep_file(file_path: Path):
    """
    保留.git目录、cpp-templates目录、README、此脚本自身，以及src目录及其所有子文件夹下的C++源文件，其余均删除。
    """
    if file_path == SCRIPT_DIR / ".git":
        return True
    if file_path == SCRIPT_DIR / "cpp-templates":
        return True
    if file_path.name == "README.md":
        return True
    if file_path.name == SCRIPT_NAME:
        return True
    # 保留src目录及其子文件夹下的C++源文件
    try:
        rel = file_path.relative_to(SCRIPT_DIR)
    except ValueError:
        return False
    if rel.parts and rel.parts[0] == "src":
        if file_path.is_file() and file_path.suffix.lower() in CPP_EXTS:
            return True
    return False


def remove_unwanted_files():
    log("开始清理无关文件...", mode="init")
    for item in SCRIPT_DIR.iterdir():
        if item.name in {".git", "cpp-templates", SCRIPT_NAME}:
            log(f"保留 {item.name} 文件/目录", mode="init")
            continue
        if item.is_dir():
            if item.name == "src":
                log("处理 src 目录...", mode="init")
                for sub in item.rglob("*"):
                    if sub.is_file() and not should_keep_file(sub):
                        log(f"删除 src 下非C++源文件: {sub}", mode="init")
                        sub.unlink()
                for sub in sorted(item.rglob("*"), key=lambda x: -len(str(x))):
                    if sub.is_dir() and not any(sub.iterdir()):
                        log(f"删除空目录: {sub}", mode="init")
                        sub.rmdir()
            else:
                log(f"删除目录及其内容: {item}", mode="init")
                shutil.rmtree(item)
        else:
            if not should_keep_file(item):
                log(f"删除文件: {item}", mode="init")
                item.unlink()
            else:
                log(f"保留文件: {item}", mode="init")
    log("清理完成。", mode="init")


def run_git_clone_or_pull(mode="init"):
    if not SUBMODULE_DIR.exists():
        SUBMODULE_DIR.parent.mkdir(parents=True, exist_ok=True)
        log(f"克隆仓库到 {SUBMODULE_DIR} ...", mode=mode)
        subprocess.check_call(["git", "clone", REPO_URL, str(SUBMODULE_DIR)])
    else:
        log(f"拉取 {SUBMODULE_DIR} 的最新变更 ...", mode=mode)
        subprocess.check_call(["git", "-C", str(SUBMODULE_DIR), "pull"])


def copy_from_templates(mode="init"):
    """
    从模板仓库复制文件到当前目录。
    mode="init" 复制全部（除.git）
    mode="update" 复制除 CMakeLists.txt/CMakePresets.json/.git 以外全部
    """
    if mode == "init":
        log("开始从模板仓库复制所有文件...", mode)
        exclude = {".git", "README.md"}
    elif mode == "update":
        log("开始从模板仓库选择性复制文件...", mode)
        exclude = {".git", "CMakeLists.txt", ".gitignore", "README.md"}
    else:
        log(f"未知复制模式: {mode}", mode)
        return

    for item in SUBMODULE_DIR.iterdir():
        if item.name in exclude:
            log(f"跳过 {item.name}", mode)
            continue
        dest = SCRIPT_DIR / item.name
        if dest.exists():
            if dest.is_dir():
                log(f"覆盖目录: {dest}", mode)
                shutil.rmtree(dest)
            else:
                log(f"覆盖文件: {dest}", mode)
                dest.unlink()
        if item.is_dir():
            shutil.copytree(item, dest)
        else:
            shutil.copy2(item, dest)
        log(f"复制 {item} -> {dest}", mode)
    log("文件复制完成。", mode)


def main():
    parser = argparse.ArgumentParser(
        description="Update cpp-templates from remote repo."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser(
        "init",
        help="Delete all except .git, cpp-templates, this script, and all *.cpp|*.h|*.cxx|*.cc|*.hpp in src/ and its subfolders, then copy all files (except .git) from cpp-templates to script dir, overwrite if exists.",
    )
    subparsers.add_parser(
        "update",
        help="Copy cmake, conan_profiles, templates folders and manage_subprojects.py from cpp-templates to script dir.",
    )

    args = parser.parse_args()
    if args.command == "init":
        run_git_clone_or_pull(mode="init")
        remove_unwanted_files()
        copy_from_templates(mode="init")
    elif args.command == "update":
        run_git_clone_or_pull(mode="update")
        copy_from_templates(mode="update")
        # 更新所有包
        subprojects_param_file = "subprojects_params.json"
        subprojects_param = load_subprojects_param_file(subprojects_param_file)
        current_dir = os.path.dirname(os.path.abspath(__file__))
        other_script = os.path.join(current_dir, "manage_subprojects.py")
        for subproject in subprojects_param.keys():
            subprocess.run(["python", other_script, "add", subproject])


if __name__ == "__main__":
    main()
