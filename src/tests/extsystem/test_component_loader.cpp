#include <doctest/doctest.h>
#include <extsystem/Component.h>
#include <extsystem/ComponentLoader.h>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QPluginLoader>
#include <QTemporaryDir>

TEST_SUITE("ComponentLoader") {
  TEST_CASE("AddComponents with empty folder") {
    QTemporaryDir temp_dir;
    REQUIRE(temp_dir.isValid());  // 确保临时目录已创建

    sss::extsystem::ComponentLoader loader;
    loader.AddComponents(temp_dir.path());

    CHECK(loader.Components().isEmpty());
  }

  TEST_CASE("AddComponents with non-plugin files") {
    QTemporaryDir temp_dir;
    REQUIRE(temp_dir.isValid());

    QFile file1(temp_dir.filePath("test.txt"));
    REQUIRE(file1.open(QIODevice::WriteOnly));
    file1.write("hello");
    file1.close();

    QFile file2(temp_dir.filePath("another.log"));
    REQUIRE(file2.open(QIODevice::WriteOnly));
    file2.write("world");
    file2.close();

    sss::extsystem::ComponentLoader loader;
    loader.AddComponents(temp_dir.path());

    CHECK(loader.Components().isEmpty());
  }

  TEST_CASE("AddComponents with dummy plugin-named files") {
    QTemporaryDir temp_dir;
    REQUIRE(temp_dir.isValid());

    // 创建一个带有插件扩展名的虚拟文件
    // 在Linux上，典型扩展名为.so
    // 在Windows上，典型扩展名为.dll
    QString plugin_extension;
#if defined(Q_OS_LINUX)
    plugin_extension = ".so";
#elif defined(Q_OS_WINDOWS)
    plugin_extension = ".dll";
#else
    plugin_extension = ".dummy_plugin_ext";  // 其他操作系统的后备选项
#endif

    QFile file1(temp_dir.filePath("dummy_plugin" + plugin_extension));
    REQUIRE(file1.open(QIODevice::WriteOnly));
    file1.write("This is not a real plugin.");
    file1.close();

    // 对于无效插件，QPluginLoader::metaData() 将返回空的 QVariantMap。
    // 如果 ComponentLoader 依赖有效的元数据，很可能会过滤掉这些插件。
    sss::extsystem::ComponentLoader loader;
    loader.AddComponents(temp_dir.path());

    // 根据 ComponentLoader 的描述，它"查找兼容的组件"
    // 不是有效 Qt 插件的文件（即没有 Q_PLUGIN_METADATA）
    // 不会被视为兼容的组件。
    // 因此，Components() 应该为空。
    CHECK(loader.Components().isEmpty());
  }
}
