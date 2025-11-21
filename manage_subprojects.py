#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import os
import shutil
import re
import sys
import json
import configparser
from pathlib import Path

try:
    from jinja2 import Environment, FileSystemLoader
except ImportError:
    print("错误：未找到 Jinja2 库。请使用 'pip install Jinja2' 安装。")
    sys.exit(1)

try:
    from ruamel.yaml import YAML
    from ruamel.yaml.scalarstring import DoubleQuotedScalarString as DQString
except ImportError:
    print("错误：未找到 ruamel.yaml 库。请使用 'pip install ruamel.yaml' 安装。")
    sys.exit(1)


class ProjectManager:
    """
    管理 C++ 子项目 (CMake/Conan) 的类。
    封装了子项目的添加、删除和 GitLab CI 文件生成等逻辑。
    """

    def __init__(self, config_file_path_str="config.ini"):
        """
        初始化 ProjectManager。

        Args:
            config_file_path_str (str): 配置 INI 文件的路径。
        """
        self.config = configparser.ConfigParser()
        self.script_dir = Path(__file__).parent.resolve()
        self._load_config(config_file_path_str)
        self.yaml = YAML()
        self.yaml.preserve_quotes = True
        self.yaml.indent(mapping=2, sequence=4, offset=2)  # 控制输出格式

    def _resolve_template_path(
        self, template_key_name: str, fallback_name: str, exit_on_fail: bool = True
    ):
        """Helper function to resolve template paths from config."""
        template_name = self.config.get(
            "Paths", template_key_name, fallback=fallback_name
        )
        resolved_template_path = None
        possible_paths = [
            self.script_dir / "templates" / template_name,
            self.script_dir / template_name,
        ]
        for p_path in possible_paths:
            if p_path.exists():
                resolved_template_path = str(p_path)
                break

        if resolved_template_path:
            self.config.set(
                "Paths", f"_resolved_{template_key_name}", resolved_template_path
            )
        elif exit_on_fail:
            print(
                f"错误：模板 '{template_name}' (来自配置 '{template_key_name}') 在指定位置未找到。"
            )
            print(f"  尝试的路径: {', '.join(map(str, possible_paths))}")
            sys.exit(1)
        else:
            # Store None or an empty string if not found and not exiting
            self.config.set("Paths", f"_resolved_{template_key_name}", "")
            print(
                f"警告：可选模板 '{template_name}' (来自配置 '{template_key_name}') 未找到。将跳过相关步骤。"
            )
        return resolved_template_path

    def _get_config_path(
        self, section, key, fallback_name=None, is_relative_to_project_root=True
    ):
        """从配置中获取路径，并根据需要解析它。"""
        if not self.config:
            print("错误：配置尚未加载。")
            sys.exit(1)

        path_str = self.config.get(section, key, fallback=fallback_name)
        if not path_str:
            print(
                f"警告：配置项 {section}/{key} 为空。将使用回退值 '{fallback_name if fallback_name else ''}'。"
            )
            return Path(fallback_name) if fallback_name else Path()

        if Path(path_str).is_absolute():
            return Path(path_str)

        if is_relative_to_project_root:
            return Path.cwd() / path_str
        else:
            return self.script_dir / path_str

    def _load_config(self, config_file_path_str="config.ini"):
        """从 INI 文件加载配置。"""
        config_file_path = Path(config_file_path_str)

        if not config_file_path.is_absolute():
            config_file_path = Path.cwd() / config_file_path_str
            if not config_file_path.exists():
                config_file_path = self.script_dir / config_file_path_str

        if not config_file_path.exists():
            print(f"错误：配置文件 '{config_file_path}' 未找到。")
            print("请创建它或从包含它的目录运行脚本，或提供正确的路径。")
            sys.exit(1)

        self.config.read(config_file_path, encoding="utf-8")

        # 子项目 CMakeLists.txt 模板
        self._resolve_template_path(
            "subproject_cmakelists_template",
            "subproject_cmakelists.txt.j2",
            exit_on_fail=True,
        )

        # 子项目 conanfile.py 模板
        self._resolve_template_path(
            "subproject_conanfile_template",
            "generic_conanfile.py.j2",
            exit_on_fail=True,
        )

        # 子项目 xxConfig.cmake.in 模板
        self._resolve_template_path(
            "subproject_config_cmake_in_template",
            "config.cmake.in.j2",
            exit_on_fail=False,
        )

        # 子项目 user_config.cmake 模板
        self._resolve_template_path(
            "subproject_user_config_cmake_template",
            "user_config.cmake",
            exit_on_fail=False,
        )

        # 子项目 version.rc.in 模板
        self._resolve_template_path(
            "subproject_version_rc_in_template",
            "version.rc.in.j2",
            exit_on_fail=False,
        )

        # 根 CMakeLists.txt
        self.main_cmake_file_path = self._get_config_path(
            section="Paths",
            key="main_cmake_filename",
            fallback_name="CMakeLists.txt",
            is_relative_to_project_root=True,
        )

        # 根 CMakePresets.json
        self.cmake_presets_file_path = self._get_config_path(
            section="Paths",
            key="cmake_presets_filename",
            fallback_name="CMakePresets.json",
            is_relative_to_project_root=True,
        )

        # 根 .gitlab-ci.yml
        self.gitlab_ci_file_path = self._get_config_path(
            section="Paths",
            key="gitlab_ci_filename",
            fallback_name=".gitlab-ci.yml",
            is_relative_to_project_root=True,
        )

    @staticmethod
    def sanitize_project_name(name: str) -> str:
        """清理名称，使其成为有效的 C++ 标识符和目录名。"""
        name = re.sub(r"[^\w_]", "_", name)
        if name and name[0].isdigit():
            name = "_" + name
        return name

    @staticmethod
    def _find_block_indices(lines, macro_name, first_arg_value_quoted):
        """
        同时支持单行和多行宏块的查找。
        - 宏调用头匹配 macro_name
        - 第一个参数严格匹配 first_arg_value_quoted（带不带引号要和实际内容一致）
        - 单行和多行自动判断括号闭合
        返回 (start_idx, end_idx)，没找到返回 None
        """
        i = 0
        while i < len(lines):
            line_stripped = lines[i].strip()
            # 1. 匹配宏头
            match_start = re.match(rf"^\s*{re.escape(macro_name)}\s*\(", line_stripped)
            if match_start:
                block_start_line_idx = i
                # 查找第一个参数
                # 先提取括号及后续内容
                rest_of_line = line_stripped[line_stripped.find("(") + 1 :].strip()
                # 情况一：单行宏（括号直接闭合）
                if rest_of_line.endswith(")"):
                    arg_str = rest_of_line[:-1].strip()
                    # 只取第一个参数
                    if arg_str.startswith(first_arg_value_quoted):
                        return (i, i)
                else:
                    # 多行或参数多，找下一个非空非注释行
                    first_arg_line_idx = i + 1
                    while first_arg_line_idx < len(lines):
                        arg_line = lines[first_arg_line_idx].strip()
                        if arg_line == "" or arg_line.startswith("#"):
                            first_arg_line_idx += 1
                            continue
                        if arg_line == first_arg_value_quoted:
                            # 继续括号配对查找块尾
                            open_paren_count = 0
                            for j in range(block_start_line_idx, len(lines)):
                                line_nocomment = re.sub(r"#.*$", "", lines[j])
                                # 字符串内括号替换
                                line_nocomment = re.sub(
                                    r'"[^"\\]*(?:\\.[^"\\]*)*"', '""', line_nocomment
                                )
                                # 找首行第一个 (
                                first_paren_on_start_line = -1
                                if j == block_start_line_idx:
                                    first_paren_on_start_line = line_nocomment.find("(")
                                for char_idx, char in enumerate(line_nocomment):
                                    if (
                                        j == block_start_line_idx
                                        and first_paren_on_start_line != -1
                                        and char_idx < first_paren_on_start_line
                                    ):
                                        continue
                                    if char == "(":
                                        open_paren_count += 1
                                    elif char == ")":
                                        open_paren_count -= 1
                                if open_paren_count == 0:
                                    return (block_start_line_idx, j)
                            break
                        else:
                            break  # 第一个参数不匹配
            i += 1
        return None

    @staticmethod
    def _add_or_update_cmake_macro_block(
        lines, macro_name, block_content, first_arg_value_quoted
    ):
        """
        添加或更新指定宏块.
        如果存在则替换，如果不存在则按末尾插入.
        返回 (新lines, 是否有更改)
        """
        # block_content 应为多行字符串
        block_lines = [l.rstrip() for l in block_content.strip().splitlines()]
        indices = ProjectManager._find_block_indices(
            lines, macro_name, first_arg_value_quoted
        )
        if indices:
            start, end = indices
            # 检查是否内容一致, 一致则不变
            old_block = lines[start : end + 1]
            if [l.rstrip() for l in old_block] == block_lines:
                return lines, False
            print(f"  更新宏块 '{macro_name}' (第一个参数为 {first_arg_value_quoted})")
            new_lines = lines[:start] + block_lines + lines[end + 1 :]
            return new_lines, True
        else:
            # 没有则添加到合适区域（调用者需插入到 begin/end 标记之间）
            print(f"  添加宏块 '{macro_name}' (第一个参数为 {first_arg_value_quoted})")
            return lines + block_lines, True

    @staticmethod
    def _remove_cmake_macro_block(lines, macro_name, first_arg_value_quoted):
        """
        删除匹配的宏块（多行），返回 (新lines, 是否有更改)
        """
        indices = ProjectManager._find_block_indices(
            lines, macro_name, first_arg_value_quoted
        )
        if indices:
            start, end = indices
            print(
                f"  删除宏块 '{macro_name}' (第一个参数为 {first_arg_value_quoted})，行 {start + 1}-{end + 1}"
            )
            new_lines = lines[:start] + lines[end + 1 :]
            return new_lines, True
        else:
            return lines, False

    def _update_cmake_multiline_block(
        self,
        file_path,
        begin_marker,
        end_marker,
        macro_name,
        block_content,
        first_arg_value_quoted,
    ):
        """
        在 begin/end 标记之间添加/更新（多行）宏块
        """
        if not file_path.exists():
            print(f"警告：文件 '{file_path}' 未找到，无法更新。")
            return False
        try:
            with open(file_path, "r", encoding="utf-8") as f:
                lines = f.read().splitlines()
        except Exception as e:
            print(f"读取文件 '{file_path}' 时出错：{e}")
            return False

        try:
            begin_idx = lines.index(begin_marker)
            end_idx = lines.index(end_marker)
        except ValueError:
            print(f"错误：起止标记 '{begin_marker}' 或 '{end_marker}' 未找到。")
            return False
        if begin_idx >= end_idx:
            print(f"错误：起始标记在结束标记之后。")
            return False

        section_lines = lines[begin_idx + 1 : end_idx]
        new_section_lines, changed = ProjectManager._add_or_update_cmake_macro_block(
            section_lines, macro_name, block_content, first_arg_value_quoted
        )
        if changed:
            new_lines = lines[: begin_idx + 1] + new_section_lines + lines[end_idx:]
            with open(file_path, "w", encoding="utf-8", newline="\n") as f:
                f.write("\n".join(new_lines) + "\n")
            print(f"  {macro_name} 宏块已写入 '{file_path}'。")
        return changed

    def _remove_cmake_multiline_block(
        self, file_path, begin_marker, end_marker, macro_name, first_arg_value_quoted
    ):
        """
        在 begin/end 标记之间删除（多行）宏块
        """
        if not file_path.exists():
            print(f"警告：文件 '{file_path}' 未找到，无法更新。")
            return False
        try:
            with open(file_path, "r", encoding="utf-8") as f:
                lines = f.read().splitlines()
        except Exception as e:
            print(f"读取文件 '{file_path}' 时出错：{e}")
            return False

        try:
            begin_idx = lines.index(begin_marker)
            end_idx = lines.index(end_marker)
        except ValueError:
            print(f"错误：起止标记 '{begin_marker}' 或 '{end_marker}' 未找到。")
            return False
        if begin_idx >= end_idx:
            print(f"错误：起始标记在结束标记之后。")
            return False

        section_lines = lines[begin_idx + 1 : end_idx]
        new_section_lines, changed = ProjectManager._remove_cmake_macro_block(
            section_lines, macro_name, first_arg_value_quoted
        )
        if changed:
            new_lines = lines[: begin_idx + 1] + new_section_lines + lines[end_idx:]
            with open(file_path, "w", encoding="utf-8", newline="\n") as f:
                f.write("\n".join(new_lines) + "\n")
            print(f"  {macro_name} 宏块已从 '{file_path}' 删除。")
        return changed

    def _generate_conan_target_call(
        self,
        subproject_cmake_path: str,
        conan_pkg_name: str,
        version: str,
    ) -> str:
        """生成 define_conan_publish_targets_for_subproject 调用字符串"""
        external_deps_str = ""
        internal_deps_str = ""
        revision = "${PRODUCT_VERSION_REVISION}"

        pkgs = self._parse_publish_targets_from_cmakelists(
            str(self.main_cmake_file_path),
            self.config.get("Markers", "conan_targets_begin"),
            self.config.get("Markers", "conan_targets_end"),
        )
        if pkgs.get(subproject_cmake_path):
            conan_pkg_name = pkgs[subproject_cmake_path].get(
                "conan_pkg_name", conan_pkg_name
            )
            version = pkgs[subproject_cmake_path].get("version", version)
            revision = pkgs[subproject_cmake_path].get("revision", revision)
            external_deps_str = pkgs[subproject_cmake_path].get(
                "external_dependencies", external_deps_str
            )
            internal_deps_str = pkgs[subproject_cmake_path].get(
                "internal_dependencies", internal_deps_str
            )
        # 生成多行格式的函数调用
        call_lines = [
            "define_conan_publish_targets_for_subproject(",
            f'  "{subproject_cmake_path}"',
            f'  "{conan_pkg_name}"',
            f'  "{version}"',
            f'  "{revision}"',
            f'  "{external_deps_str}"',
            f'  "{internal_deps_str}")',
        ]
        return "\n".join(call_lines)

    def _update_cmake_presets(
        self, subproject_name_for_display: str, conan_pkg_name: str, action: str = "add"
    ):
        presets_file = self.cmake_presets_file_path
        if not presets_file.exists():
            print(f"信息：'{presets_file}' 未找到。跳过 CMakePresets.json 更新。")
            return False
        try:
            with open(presets_file, "r", encoding="utf-8") as f:
                presets_data = json.load(f)
        except Exception as e:
            print(f"读取或解析 '{presets_file}' 时出错：{e}")
            return False

        if "buildPresets" not in presets_data:
            presets_data["buildPresets"] = []
        if "configurePresets" not in presets_data:
            print(
                f"警告：在 '{presets_file}' 中未找到 'configurePresets'。无法生成详细的构建预设。"
            )
            return False

        made_change = False
        build_preset_prefix = self.config.get(
            "CMakePresets", "build_preset_name_template_prefix", fallback="publish-"
        )
        debug_suffix = self.config.get(
            "CMakePresets", "build_preset_name_template_debug_suffix", fallback="-debug"
        )
        release_suffix = self.config.get(
            "CMakePresets",
            "build_preset_name_template_release_suffix",
            fallback="-release",
        )
        base_configure_preset_to_ignore = self.config.get(
            "CMakePresets", "default_configure_preset", fallback="default-config"
        )
        relevant_configure_presets = []
        for cp_entry in presets_data.get("configurePresets", []):
            cp_name = cp_entry.get("name")
            is_hidden = cp_entry.get("hidden", False)
            if cp_name and not is_hidden and cp_name != base_configure_preset_to_ignore:
                relevant_configure_presets.append(cp_entry)
            elif (
                cp_name and cp_name == base_configure_preset_to_ignore and not is_hidden
            ):
                relevant_configure_presets.append(cp_entry)

        if not relevant_configure_presets:
            default_cp_entry = next(
                (
                    cp
                    for cp in presets_data.get("configurePresets", [])
                    if cp.get("name") == base_configure_preset_to_ignore
                    and not cp.get("hidden", False)
                ),
                None,
            )
            if default_cp_entry:
                relevant_configure_presets.append(default_cp_entry)
                print(
                    f"  将使用配置预设 '{base_configure_preset_to_ignore}' 作为生成的基础，因为未找到其他非隐藏的配置预设。"
                )
            else:
                print(
                    f"警告：在 '{presets_file}' 中未找到可用的 configurePresets (非隐藏且非基础的，或者基础预设本身不是隐藏的)。无法为 '{conan_pkg_name}' 生成构建预设。"
                )
                return False

        build_types_map_config = {
            "debug": debug_suffix,
            "release": release_suffix,
        }
        all_generated_preset_names_for_this_pkg = []

        for cp_entry in relevant_configure_presets:
            configure_preset_name_to_use = cp_entry.get("name")
            sane_configure_preset_name_for_file = ProjectManager.sanitize_project_name(
                configure_preset_name_to_use
            ).lower()
            implied_build_type = None
            cache_vars = cp_entry.get("cacheVariables", {})
            if "CMAKE_BUILD_TYPE" in cache_vars:
                implied_build_type = cache_vars["CMAKE_BUILD_TYPE"].lower()
                if implied_build_type not in build_types_map_config:
                    print(
                        f"警告：配置预设 '{configure_preset_name_to_use}' 定义了未知的 CMAKE_BUILD_TYPE '{implied_build_type}'。将忽略此预设的隐含构建类型。"
                    )
                    implied_build_type = None
            build_types_to_generate_for_this_cp = []
            if implied_build_type:
                if implied_build_type in build_types_map_config:
                    build_types_to_generate_for_this_cp.append(implied_build_type)
                else:
                    continue
            else:
                build_types_to_generate_for_this_cp = list(
                    build_types_map_config.keys()
                )
            for build_type_key in build_types_to_generate_for_this_cp:
                bt_suffix = build_types_map_config[build_type_key]
                preset_name = f"{build_preset_prefix}{conan_pkg_name}{bt_suffix}-using-{sane_configure_preset_name_for_file}"
                all_generated_preset_names_for_this_pkg.append(preset_name)
                cmake_target_for_publish = (
                    f"publish_{conan_pkg_name}_{build_type_key.lower()}"
                )
                if action == "add":
                    if not any(
                        p.get("name") == preset_name
                        for p in presets_data["buildPresets"]
                    ):
                        new_preset = {
                            "name": preset_name,
                            "displayName": f"Publish {conan_pkg_name} ({build_type_key.capitalize()}) using {configure_preset_name_to_use}",
                            "configurePreset": configure_preset_name_to_use,
                            "targets": [cmake_target_for_publish],
                        }
                        presets_data["buildPresets"].append(new_preset)
                        print(
                            f"  已添加 buildPreset '{preset_name}' (for configurePreset '{configure_preset_name_to_use}') 到 '{presets_file}'。"
                        )
                        made_change = True
                    else:
                        print(
                            f"  信息：buildPreset '{preset_name}' 已存在于 '{presets_file}'。"
                        )
        if action == "remove":
            initial_len = len(presets_data["buildPresets"])
            presets_data["buildPresets"] = [
                p
                for p in presets_data["buildPresets"]
                if p.get("name") not in all_generated_preset_names_for_this_pkg
            ]
            if len(presets_data["buildPresets"]) != initial_len:
                made_change = True
                actually_removed_count = initial_len - len(presets_data["buildPresets"])
                print(
                    f"  已从 '{presets_file}' 成功删除 {actually_removed_count} 个与 '{conan_pkg_name}' 相关的 buildPreset。"
                )
            else:
                print(
                    f"  信息：在 '{presets_file}' 中未找到与 '{conan_pkg_name}' 相关的预设进行删除。"
                )
        if made_change:
            try:
                with open(presets_file, "w", encoding="utf-8") as f:
                    json.dump(presets_data, f, indent=2)
                    f.write("\n")
                print(f"  已更新 '{presets_file}'。")
            except Exception as e:
                print(f"写入 '{presets_file}' 时出错：{e}")
                return False
        return made_change

    def _load_gitlab_ci_data(self):
        if not self.gitlab_ci_file_path.exists():
            print(
                f"警告：GitLab CI 文件 '{self.gitlab_ci_file_path}' 未找到。将创建一个空的结构。"
            )
            return self.yaml.load("stages:\n  - build_and_package\n")
        try:
            with open(self.gitlab_ci_file_path, "r", encoding="utf-8") as f:
                return self.yaml.load(f)
        except Exception as e:
            print(f"读取或解析 GitLab CI 文件 '{self.gitlab_ci_file_path}' 时出错: {e}")
            return None

    def _save_gitlab_ci_data(self, ci_data):
        if ci_data is None:
            print("错误：CI 数据为空，无法保存。")
            return
        try:
            with open(self.gitlab_ci_file_path, "w", encoding="utf-8") as f:
                self.yaml.dump(ci_data, f)
            print(f"已更新 GitLab CI 文件 '{self.gitlab_ci_file_path}'。")
        except Exception as e:
            print(f"写入 GitLab CI 文件 '{self.gitlab_ci_file_path}' 时出错: {e}")

    def _generate_ci_jobs_for_package(
        self, conan_pkg_name: str, subproject_version: str, subproject_cmake_name: str
    ) -> dict:
        generated_ci_jobs = {}
        presets_file_path = self.cmake_presets_file_path
        if not presets_file_path.exists():
            print(f"错误：CMakePresets.json 文件 '{presets_file_path}' 未找到。")
            return generated_ci_jobs

        try:
            with open(presets_file_path, "r", encoding="utf-8") as f:
                presets_data = json.load(f)
        except Exception as e:
            print(
                f"错误：读取或解析 CMakePresets.json 文件 '{presets_file_path}' 失败: {e}"
            )
            return generated_ci_jobs

        if "buildPresets" not in presets_data or not presets_data["buildPresets"]:
            print("警告：CMakePresets.json 文件中没有找到 'buildPresets'。")
            return generated_ci_jobs

        ci_platforms_config = {}
        for section_name in self.config.sections():
            if section_name.startswith("CIPlatforms."):
                platform_id = section_name.split(".", 1)[1]
                ci_platforms_config[platform_id] = dict(self.config.items(section_name))

        if not ci_platforms_config:
            print("警告：config.ini 文件中未定义任何 [CIPlatforms.*] 部分。")
            return generated_ci_jobs

        build_preset_prefix = self.config.get(
            "CMakePresets", "build_preset_name_template_prefix", fallback="publish-"
        )
        build_type_suffixes_map = {
            "debug": self.config.get(
                "CMakePresets",
                "build_preset_name_template_debug_suffix",
                fallback="-debug",
            ),
            "release": self.config.get(
                "CMakePresets",
                "build_preset_name_template_release_suffix",
                fallback="-release",
            ),
        }

        build_types_to_process = list(build_type_suffixes_map.keys())

        for platform_id, platform_details in ci_platforms_config.items():
            platform_slug = platform_id.lower().replace(" ", "_").replace("/", "_")
            # 合并为一个 job
            ci_job_name = f"publish_{conan_pkg_name}_{platform_slug}"
            script_lines = []
            docker_image_val = platform_details.get("docker_image")

            # 只生成一个 job，先加通用命令
            script_lines.append("conan remote disable conancenter")
            script_lines.append("cd ${CI_PROJECT_DIR}")

            # 然后循环 build_types
            for build_type_key in build_types_to_process:
                # 获取 configurePreset
                target_configure_preset_name_for_ci_platform = ""
                configure_preset_map_str = platform_details.get("configure_preset_map")
                if configure_preset_map_str:
                    try:
                        configure_map = json.loads(configure_preset_map_str)
                        target_configure_preset_name_for_ci_platform = (
                            configure_map.get(build_type_key)
                        )
                    except json.JSONDecodeError:
                        print(
                            f"警告：平台 '{platform_id}' 的 'configure_preset_map' JSON 解析失败: {configure_preset_map_str}"
                        )

                if not target_configure_preset_name_for_ci_platform:
                    target_configure_preset_name_for_ci_platform = platform_details.get(
                        "configure_preset"
                    )

                if not target_configure_preset_name_for_ci_platform:
                    print(
                        f"信息：平台 '{platform_id}' 未能为构建类型 '{build_type_key}' 确定目标 CMake configure preset。跳过此组合。"
                    )
                    continue

                sane_configure_preset_name = ProjectManager.sanitize_project_name(
                    target_configure_preset_name_for_ci_platform
                ).lower()
                build_type_suffix = build_type_suffixes_map.get(build_type_key)
                expected_build_preset_name = f"{build_preset_prefix}{conan_pkg_name}{build_type_suffix}-using-{sane_configure_preset_name}"

                found_build_preset_entry = next(
                    (
                        bp
                        for bp in presets_data["buildPresets"]
                        if bp.get("name") == expected_build_preset_name
                    ),
                    None,
                )

                if not found_build_preset_entry:
                    print(
                        f"信息：期望的 build preset '{expected_build_preset_name}' 未在 CMakePresets.json 中找到 (包: {conan_pkg_name}, 平台: {platform_id}, 类型: {build_type_key})。跳过CI作业生成。"
                    )
                    continue

                cmake_build_preset_for_publish = found_build_preset_entry["name"]
                actual_configure_preset_linked_by_bp = found_build_preset_entry.get(
                    "configurePreset"
                )

                # 生成脚本：每个 build type 一组 configure/build
                script_lines.append(
                    f'echo "Publishing Conan Package: {conan_pkg_name} for platform {platform_id} (Build Type: {build_type_key})"'
                )
                script_lines.append(
                    f'cmake --preset "{actual_configure_preset_linked_by_bp}"'
                )
                script_lines.append(
                    f'cmake --build --preset "{cmake_build_preset_for_publish}"'
                )

            # 只生成一个 job
            product_version = "${CI_COMMIT_TAG}"
            product_version_revision = "${CI_COMMIT_SHORT_SHA}"
            path_to_subproject_cmake = f"src/{subproject_cmake_name}/**"
            job_definition = {
                "extends": DQString(".job_template_conan_publish"),
                "variables": {
                    "PRODUCT_VERSION_FROM_CI": DQString(product_version),
                    "PRODUCT_VERSION_REVISION_FROM_CI": DQString(
                        product_version_revision
                    ),
                },
                "rules": [
                    {"changes": [DQString(path_to_subproject_cmake)]},
                    {"when": "manual"},
                ],
                "tags": [
                    DQString(tag.strip())
                    for tag in platform_details.get("gitlab_runner_tags", "").split(",")
                    if tag.strip()
                ],
                "script": script_lines,
            }
            if docker_image_val:
                job_definition["image"] = DQString(docker_image_val)

            generated_ci_jobs[ci_job_name] = job_definition
            print(f"  成功为包 '{conan_pkg_name}' 生成合并 CI 作业: {ci_job_name}")

        return generated_ci_jobs

    def _render_and_copy_templates_for_subproject(
        self,
        templates_config: dict,
        jinja_context: dict,
        project_path_abs: Path,
        is_update: bool = False,
    ):
        """
        渲染和复制 templates 文件夹下的模板文件到子项目目录。

        Args:
            templates_config (dict): 各模板文件的绝对路径。例如:
                {
                    'cmakelists': Path("xxx/templates/subproject_cmakelists.txt.j2"),
                    'conanfile': Path("xxx/templates/generic_conanfile.py.j2"),
                    'config_cmake_in': Path("xxx/templates/config.cmake.in.j2") 或 None,
                    'user_config_cmake': Path("xxx/templates/user_config.cmake") 或 None,
                    'version_rc_in': Path("xxx/templates/version.rc.in.j2") 或 None,
                }
            jinja_context (dict): Jinja2 渲染上下文。
            project_path_abs (Path): 目标子项目目录。
            is_update (bool): 是否为更新模式，影响日志输出。

        Returns:
            None
        """
        # CMakeLists.txt
        cmakelists_tpl = templates_config.get("cmakelists")
        if cmakelists_tpl and cmakelists_tpl.exists():
            env = Environment(
                loader=FileSystemLoader(str(cmakelists_tpl.parent)),
                trim_blocks=True,
                lstrip_blocks=True,
            )
            tpl = env.get_template(cmakelists_tpl.name)
            content = tpl.render(**jinja_context)
            dest = project_path_abs / "CMakeLists.txt"
            with open(dest, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"  已{'覆盖' if is_update else '创建'} '{dest}'。")
        else:
            print(f"错误: CMakeLists.txt 模板 '{cmakelists_tpl}' 未找到。")

        # conanfile.py
        conanfile_tpl = templates_config.get("conanfile")
        if conanfile_tpl and conanfile_tpl.exists():
            env = Environment(
                loader=FileSystemLoader(str(conanfile_tpl.parent)),
                trim_blocks=True,
                lstrip_blocks=True,
            )
            tpl = env.get_template(conanfile_tpl.name)
            content = tpl.render(**jinja_context)
            dest = project_path_abs / "conanfile.py.in"
            if os.path.exists(project_path_abs / "conanfile.py"):
                os.remove(project_path_abs / "conanfile.py")
            with open(dest, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"  已{'覆盖' if is_update else '创建'} '{dest}'。")
        else:
            print(f"错误: conanfile.py 模板 '{conanfile_tpl}' 未找到。")

        # config_cmake.in
        config_cmake_in_tpl = templates_config.get("config_cmake_in")
        if config_cmake_in_tpl and config_cmake_in_tpl.exists():
            env = Environment(
                loader=FileSystemLoader(str(config_cmake_in_tpl.parent)),
                trim_blocks=True,
                lstrip_blocks=True,
            )
            tpl = env.get_template(config_cmake_in_tpl.name)
            content = tpl.render(**jinja_context)
            dest = project_path_abs / f"{jinja_context['project_name']}Config.cmake.in"
            with open(dest, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"  已{'覆盖' if is_update else '创建'} '{dest}'。")

        # user_config.cmake
        user_config_cmake_tpl = templates_config.get("user_config_cmake")
        if user_config_cmake_tpl and user_config_cmake_tpl.exists():
            dest = project_path_abs / user_config_cmake_tpl.name
            if not dest.exists():
                shutil.copy2(user_config_cmake_tpl, dest)
                print(f"  已{'覆盖' if is_update else '创建'} '{dest}'。")

        # version.rc.in
        version_rc_in_tpl = templates_config.get("version_rc_in")
        if version_rc_in_tpl and version_rc_in_tpl.exists():
            env = Environment(
                loader=FileSystemLoader(str(version_rc_in_tpl.parent)),
                trim_blocks=True,
                lstrip_blocks=True,
            )
            tpl = env.get_template(version_rc_in_tpl.name)
            content = tpl.render(**jinja_context)
            dest = project_path_abs / "version.rc.in"
            with open(dest, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"  已{'覆盖' if is_update else '创建'} '{dest}'。")

    def _load_subprojects_param_file(self, path):
        if not os.path.exists(path):
            return {}
        with open(path, "r", encoding="utf-8") as f:
            return json.load(f)

    def _save_subprojects_param_file(self, path, param_dict):
        with open(path, "w", encoding="utf-8") as f:
            json.dump(param_dict, f, indent=2)

    def _parse_publish_targets_from_cmakelists(
        self,
        cmakelists_path: str,
        marker_begin: str = "# CONAN_PUBLISH_TARGETS_BEGIN (do not remove or modify this line)",
        marker_end: str = "# CONAN_PUBLISH_TARGETS_END (do not remove or modify this line)",
    ):
        """
        解析 CMakeLists.txt 两 marker 之间的 define_conan_publish_targets_for_subproject 宏参数
        返回格式:
        {
            "subproject_path": {
                "subproject_path": ...,
                "conan_pkg_name": ...,
                "version": ...,
                "revision": ...,
                "external_dependencies": ...,
                "internal_dependencies": ...,
                "raw_args": [...6个参数...]
            },
            ...
        }
        """
        with open(cmakelists_path, "r", encoding="utf-8") as f:
            lines = f.readlines()
        # 1. 查找 marker
        try:
            begin_idx = lines.index(marker_begin + "\n")
            end_idx = lines.index(marker_end + "\n")
        except ValueError:
            raise RuntimeError("marker not found in CMakeLists.txt")
        if begin_idx >= end_idx:
            raise RuntimeError("begin marker after end marker")

        # 2. 取区块，去除注释和空行
        block_lines = []
        for l in lines[begin_idx + 1 : end_idx]:
            l_strip = l.strip()
            if l_strip.startswith("#") or not l_strip:
                continue
            block_lines.append(l)

        block_content = "".join(block_lines)

        # 3. 匹配所有 define_conan_publish_targets_for_subproject 宏
        pattern = re.compile(
            r'define_conan_publish_targets_for_subproject\s*\(\s*((?:"(?:[^"\\]|\\.)*"|\s+)*)\)',
            re.DOTALL,
        )
        macros = pattern.findall(block_content)

        results = {}
        for macro_args in macros:
            # 按 "..." 拆分参数，支持参数带换行
            arg_pattern = re.compile(r'"((?:[^"\\]|\\.)*)"')
            args = arg_pattern.findall(macro_args)
            # 宏应有6个参数
            if len(args) < 6:
                print(f"警告: 宏参数不足6个，已跳过。实际参数: {args}")
                continue
            result = {
                "subproject_path": args[0],
                "conan_pkg_name": args[1],
                "version": args[2],
                "revision": args[3],
                "external_dependencies": args[4],
                "internal_dependencies": args[5],
                "raw_args": args[:6],
            }
            results[args[0]] = result
        return results

    def add_subproject(
        self,
        name: str,
        version: str,
        project_type: str,
        conan_pkg_name: str,
        shared: bool,
        publish: bool = True,
    ):
        """添加或更新子项目。若已存在则覆盖相关内容，否则新建。"""
        sane_name = ProjectManager.sanitize_project_name(name)
        if not sane_name or sane_name != name:
            print(
                f"警告：项目名称 '{name}' 已清理为 '{sane_name}'。将使用清理后的名称。"
            )
            name = sane_name
        project_dir_name = name

        libs_subdir_name = self.config.get("Paths", "libs_subdirectory", fallback="src")
        project_path_rel = Path(libs_subdir_name) / project_dir_name
        project_path_abs = Path.cwd() / project_path_rel

        # 判断是新建还是更新
        if project_path_abs.exists():
            print(f"信息：子项目目录 '{project_path_abs}' 已存在，将进行内容更新。")
            is_update = True
        else:
            is_update = False
            project_path_abs.mkdir(parents=True, exist_ok=False)
            print(f"  已创建目录：{project_path_abs}")
            if project_type == "lib":
                project_include_path_abs = project_path_abs / "include" / f"{name}"
                project_include_path_abs.mkdir(parents=True, exist_ok=True)

        # 获取模板路径
        templates_config = {
            "cmakelists": Path(
                self.config.get(
                    "Paths", "_resolved_subproject_cmakelists_template", fallback=""
                )
            ),
            "conanfile": Path(
                self.config.get(
                    "Paths", "_resolved_subproject_conanfile_template", fallback=""
                )
            ),
            "config_cmake_in": Path(
                self.config.get(
                    "Paths",
                    "_resolved_subproject_config_cmake_in_template",
                    fallback="",
                )
            )
            if self.config.get(
                "Paths", "_resolved_subproject_config_cmake_in_template", fallback=""
            )
            else None,
            "user_config_cmake": Path(
                self.config.get(
                    "Paths",
                    "_resolved_subproject_user_config_cmake_template",
                    fallback="",
                )
            )
            if self.config.get(
                "Paths", "_resolved_subproject_user_config_cmake_template", fallback=""
            )
            else None,
            "version_rc_in": Path(
                self.config.get(
                    "Paths", "_resolved_subproject_version_rc_in_template", fallback=""
                )
            )
            if self.config.get(
                "Paths", "_resolved_subproject_version_rc_in_template", fallback=""
            )
            else None,
        }

        # Jinja2 渲染上下文
        jinja_context = {
            "project_name": name,
            "project_name_lower": name.lower(),
            "project_name_upper": name.upper(),
            "project_version": version,
            "project_type": project_type,
            "build_shared_default": shared,
            "conan_pkg_name": conan_pkg_name,
            "conan_pkg_name_upper": conan_pkg_name.upper(),
        }

        self._render_and_copy_templates_for_subproject(
            templates_config,
            jinja_context,
            project_path_abs,
            is_update=is_update,
        )

        # 更新主 CMakeLists.txt (add_subdirectory/conan_target调用 保证唯一即可)
        main_cmake_file = self.main_cmake_file_path
        subproject_cmake_path = str(project_path_rel).replace("\\", "/")
        add_subdir_line = f"add_subdirectory({subproject_cmake_path})"
        self._update_cmake_multiline_block(
            main_cmake_file,
            self.config.get("Markers", "subprojects_begin"),
            self.config.get("Markers", "subprojects_end"),
            "add_subdirectory",
            add_subdir_line,
            subproject_cmake_path,
        )
        conan_macro_block = self._generate_conan_target_call(
            subproject_cmake_path,
            conan_pkg_name,
            version,
        )
        self._update_cmake_multiline_block(
            main_cmake_file,
            self.config.get("Markers", "conan_targets_begin"),
            self.config.get("Markers", "conan_targets_end"),
            "define_conan_publish_targets_for_subproject",
            conan_macro_block,
            f'"{subproject_cmake_path}"',
        )

        if publish:
            # CMakePresets.json
            self._update_cmake_presets(project_dir_name, conan_pkg_name, action="add")

            # GitLab CI 更新
            ci_data = self._load_gitlab_ci_data()
            if ci_data is None:
                print(
                    f"错误：无法加载或初始化 GitLab CI 数据。请先手动创建或修复 '{self.gitlab_ci_file_path}'。"
                )
            else:
                new_ci_jobs = self._generate_ci_jobs_for_package(
                    conan_pkg_name, version, name
                )
                jobs_added_count = 0
                for job_name, job_def in new_ci_jobs.items():
                    ci_data[job_name] = job_def
                    jobs_added_count += 1
                    print(f"  已{'覆盖' if is_update else '添加'} CI 作业: {job_name}")
                if jobs_added_count > 0:
                    self._save_gitlab_ci_data(ci_data)

        if is_update:
            print(f"子项目 '{name}' 已存在，内容已覆盖更新。")
        else:
            print(f"子项目 '{name}' 添加成功。")

    def remove_subproject(self, name: str):
        """删除现有子项目。"""
        sane_name = ProjectManager.sanitize_project_name(name)
        if not sane_name or sane_name != name:
            print(
                f"警告：项目名称 '{name}' 已清理为 '{sane_name}'。将使用清理后的名称进行删除。"
            )
            name = sane_name
        project_dir_name = name

        libs_subdir_name = self.config.get("Paths", "libs_subdirectory", fallback="src")
        project_path_rel = Path(libs_subdir_name) / project_dir_name
        project_path_abs = Path.cwd() / project_path_rel

        if not project_path_abs.is_dir():
            print(
                f"错误：子项目目录 '{project_path_abs}' 不存在或不是一个目录。无法删除。"
            )
            return

        print(f"正在从 '{project_path_rel}' 删除子项目 '{name}'...")

        main_cmake_file = self.main_cmake_file_path
        subproject_cmake_path_to_remove = str(project_path_rel).replace("\\", "/")
        conan_pkg_name_for_removal = project_dir_name

        # 尝试从 CMakeLists.txt 宏块中解析 Conan 包名
        if main_cmake_file.exists():
            with open(main_cmake_file, "r", encoding="utf-8") as f:
                content = f.read()
                pattern_str = rf'define_conan_publish_targets_for_subproject\s*\(\s*"{re.escape(subproject_cmake_path_to_remove)}"\s*[,\s]*"([^"]+)"'
                match = re.search(pattern_str, content, re.MULTILINE)
                if match:
                    conan_pkg_name_for_removal = match.group(1)
                    print(
                        f"  从 CMakeLists.txt 中解析到 Conan 包名 '{conan_pkg_name_for_removal}' 用于删除预设。"
                    )
                else:
                    print(
                        f"  警告：无法从 CMakeLists.txt 中自动解析 '{name}' 的 Conan 包名。将使用 '{project_dir_name}' 和 '{project_dir_name}_pkg' 尝试删除预设。"
                    )

        # 删除 add_subdirectory 宏块（多行/单行都能匹配）
        self._remove_cmake_multiline_block(
            main_cmake_file,
            self.config.get("Markers", "subprojects_begin"),
            self.config.get("Markers", "subprojects_end"),
            "add_subdirectory",
            subproject_cmake_path_to_remove,
        )

        # 删除 define_conan_publish_targets_for_subproject 宏块
        self._remove_cmake_multiline_block(
            main_cmake_file,
            self.config.get("Markers", "conan_targets_begin"),
            self.config.get("Markers", "conan_targets_end"),
            "define_conan_publish_targets_for_subproject",
            f'"{subproject_cmake_path_to_remove}"',
        )

        # 删除 CMakePresets.json 中相关 preset
        self._update_cmake_presets(
            project_dir_name,
            conan_pkg_name_for_removal
            if conan_pkg_name_for_removal
            else project_dir_name,
            action="remove",
        )
        # 尝试用 _pkg 后缀名再删一次（兼容老逻辑）
        if (
            not conan_pkg_name_for_removal
            or conan_pkg_name_for_removal == project_dir_name
        ):
            self._update_cmake_presets(
                project_dir_name, f"{project_dir_name}_pkg", action="remove"
            )

        # 删除子项目目录
        try:
            shutil.rmtree(project_path_abs)
            print(f"  已删除目录：{project_path_abs}")
        except Exception as e:
            print(f"删除目录 '{project_path_abs}' 时出错：{e}")

        # 删除 GitLab CI 相关 job
        conan_pkg_name_for_removal = conan_pkg_name_for_removal or project_dir_name
        print(
            f"正在从 GitLab CI 文件中删除 '{conan_pkg_name_for_removal}' 的相关作业..."
        )
        ci_data = self._load_gitlab_ci_data()
        if ci_data is not None:
            jobs_to_delete = [
                job_name
                for job_name in ci_data
                if isinstance(job_name, str)
                and job_name.startswith(f"publish_{conan_pkg_name_for_removal}_")
            ]
            if jobs_to_delete:
                for job_name in jobs_to_delete:
                    del ci_data[job_name]
                    print(f"  已删除 CI 作业: {job_name}")
                self._save_gitlab_ci_data(ci_data)
            else:
                print(
                    f"  未找到与 '{conan_pkg_name_for_removal}' 相关的 CI 作业进行删除。"
                )
        else:
            print(
                f"警告：无法加载 GitLab CI 文件 '{self.gitlab_ci_file_path}'，跳过 CI 作业删除。"
            )
        print(f"子项目 '{name}' 删除成功。")

    @staticmethod
    def _get_user_input(
        prompt_message: str, default_value=None, type_converter=str, choices=None
    ):
        """获取用户输入，支持默认值、类型转换和选项。"""
        while True:
            full_prompt = prompt_message
            if default_value is not None:
                full_prompt += f" (默认: {default_value})"
            if choices:
                full_prompt += f" [{'/'.join(choices)}]"
            full_prompt += ": "

            value_str = input(full_prompt).strip()
            if not value_str and default_value is not None:
                return default_value

            try:
                converted_value = type_converter(value_str)
                if choices and converted_value not in choices:
                    print(f"无效的选择。请输入以下选项之一：{', '.join(choices)}")
                    continue
                return converted_value
            except ValueError:
                print(f"无效的输入。请输入一个有效的 {type_converter.__name__}。")

    def handle_add_command(self, args):
        """处理 'add' 命令。"""
        subprojects_param_file = "subprojects_params.json"
        subprojects_param = self._load_subprojects_param_file(subprojects_param_file)
        name = (
            args.name
            if args.name is not None
            else ProjectManager._get_user_input("输入子项目名称")
        )
        if not name:
            print("错误：子项目名称是必需的。")
            sys.exit(1)

        if subprojects_param.get(name):
            project_type = subprojects_param[name].get("type")
            version = subprojects_param[name].get("version")
            conan_pkg_name = subprojects_param[name].get("conan_pkg_name")
            shared = subprojects_param[name].get("shared")
            publish = subprojects_param[name].get("publish", True)
        else:
            subprojects_param[name] = {}
            project_type = (
                args.type
                if args.type is not None
                else ProjectManager._get_user_input(
                    "输入子项目类型", "lib", choices=["lib", "exe"]
                )
            )
            subprojects_param[name]["type"] = project_type
            version = (
                args.version
                if args.version is not None
                else ProjectManager._get_user_input("输入子项目版本", "0.1.0")
            )
            subprojects_param[name]["version"] = version

            sane_name_for_conan = ProjectManager.sanitize_project_name(name)
            conan_pkg_name = (
                args.conan_pkg_name
                if args.conan_pkg_name is not None
                else ProjectManager._get_user_input(
                    "输入 Conan 包名",
                    sane_name_for_conan,
                )
            )
            subprojects_param[name]["conan_pkg_name"] = conan_pkg_name

            shared = args.shared
            if shared is None:
                default_shared_choice = "yes" if project_type == "lib" else "no"
                shared_input = ProjectManager._get_user_input(
                    f"默认构建为共享库 (project type: {project_type})? (yes/no/true/false)",
                    default_shared_choice,
                ).lower()
                shared = shared_input in ["yes", "y", "true", "t"]

            if project_type == "exe" and shared:
                print("警告：可执行文件不能是共享的。将 'shared' 选项设置为 False。")
                shared = False
            subprojects_param[name]["shared"] = shared

            publish_input = ProjectManager._get_user_input(
                "是否需要发布该子项目到 Conan/CI? (yes/no)",
                "yes",
                choices=["yes", "no"],
            ).lower()
            publish = publish_input in ["yes", "y"]
            subprojects_param[name]["publish"] = publish

        self.add_subproject(
            name,
            version,
            project_type,
            conan_pkg_name,
            shared,
            publish,
        )

        self._save_subprojects_param_file(subprojects_param_file, subprojects_param)

    def handle_remove_command(self, args):
        """处理 'remove' 命令。"""
        name = (
            args.name
            if args.name is not None
            else ProjectManager._get_user_input("输入要删除的子项目的名称")
        )
        if not name:
            print("错误：删除子项目需要其名称。")
            sys.exit(1)
        self.remove_subproject(name)

        subprojects_param_file = "subprojects_params.json"
        subprojects_param = self._load_subprojects_param_file(subprojects_param_file)
        subprojects_param.pop(name, None)
        if not subprojects_param:
            if os.path.exists(subprojects_param_file):
                os.remove(subprojects_param_file)
        else:
            self._save_subprojects_param_file(subprojects_param_file, subprojects_param)


def main():
    default_config_file = "config.ini"
    parser = argparse.ArgumentParser(description="管理 C++ 子项目 (CMake/Conan)。")
    parser.add_argument(
        "--config",
        default=default_config_file,
        help=f"配置 INI 文件路径 (默认: {default_config_file})。",
    )

    subparsers = parser.add_subparsers(dest="command", help="可用命令", required=True)

    add_parser = subparsers.add_parser("add", help="添加新子项目。")
    add_parser.add_argument(
        "name", nargs="?", default=None, help="新子项目的名称 (例如 MyLibrary)。"
    )
    add_parser.add_argument(
        "--type", choices=["lib", "exe"], default=None, help="子项目类型：lib 或 exe。"
    )
    add_parser.add_argument("--version", default=None, help="子项目的初始版本。")
    add_parser.add_argument(
        "--conan_pkg_name", default=None, help="Conan 包名 (默认: 与子项目名一致)。"
    )
    shared_group = add_parser.add_mutually_exclusive_group()
    shared_group.add_argument(
        "--shared",
        dest="shared",
        action="store_true",
        help="将此库默认构建为共享库 (Conan 选项)。",
    )
    shared_group.add_argument(
        "--static",
        dest="shared",
        action="store_false",
        help="将此库默认构建为静态库 (Conan 选项)。",
    )
    add_parser.set_defaults(shared=None)

    remove_parser = subparsers.add_parser("remove", help="删除现有子项目。")
    remove_parser.add_argument(
        "name", nargs="?", default=None, help="要删除的子项目的名称。"
    )

    args = parser.parse_args()
    manager = ProjectManager(args.config)

    if args.command == "add":
        manager.handle_add_command(args)
    elif args.command == "remove":
        manager.handle_remove_command(args)


if __name__ == "__main__":
    main()
