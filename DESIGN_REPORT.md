# Ws1 Workbench Architecture & Design Report

## 1. Overview
This document outlines the architectural changes and design principles applied to the `Ws1` workspace plugin and the underlying `dscore` core infrastructure. The goal was to establish a reusable, flexible, and theme-aware workspace layout that supports 3D viewports, docked sidebars, and floating HUD (Heads-Up Display) overlays.

## 2. Architectural Changes

### 2.1 Core Infrastructure (`dscore`)

To support the complex layout requirements of a CAD/GIS-like application, the layout logic was moved from individual pages to the core library.

*   **`WorkbenchLayout`**:
    *   **Role**: The master container for any workspace page.
    *   **Structure**: Manages a primary `QSplitter` dividing the left sidebar (`QTabWidget`) from the central content area (`OverlayCanvas`).
    *   **Responsibility**: Handles sidebar collapsing/expanding and delegates overlay/background management to the canvas.
    *   **Exports**: Properly exported (`DS_CORE_DLLSPEC`) for use by plugins.

*   **`OverlayCanvas`**:
    *   **Role**: A custom `QWidget` container that manages the central viewport and all floating/docked elements.
    *   **Layout Strategy**: Implements a custom `resizeEvent` to handle:
        *   **Background Layer**: The 3D view (or placeholder) filling the remaining space.
        *   **Squeeze Layer**: Widgets (like toolbars) that dock to edges and reduce the available background area.
        *   **Overlay Layer**: Floating widgets anchored to corners/centers (HUDs).
    *   **Notifications**: Implements a `ShowNotification` method with fade-in/fade-out animations using `QPropertyAnimation` and `QGraphicsOpacityEffect`.

*   **`IWorkbench` Interface**:
    *   **Role**: Defines the contract for workspace interaction.
    *   **Methods**:
        *   `AddSidePanel`: Add tabs to the left sidebar.
        *   `SetBackgroundWidget`: Set the main 3D view.
        *   `AddOverlayWidget`: Place floating widgets (TopRight, BottomLeft, etc.).
        *   `AddSqueezeWidget`: Place structural bars (Top, Bottom).
        *   `ShowNotification`: Trigger transient messages.

*   **`CollapsibleWidget`**:
    *   **Role**: A generic reusable widget with a header toggle button and a content area.
    *   **Usage**: Used for the "Device Info" panel to allow users to minimize space usage over the 3D view.

### 2.3 Core Services

New foundational services were added to `dscore` to centralize application-wide concerns.

*   **Translation Management (`ILanguageService`)**:
    *   **Role**: Manages application localization and dynamic language switching.
    *   **Mechanism**:
        *   **Registration**: Plugins register their translation path via `RegisterTranslator` during initialization.
        *   **Loading**: When `SwitchLanguage` is called, the service iterates through all registered components. It attempts to load the specific locale (e.g., `ws1_zh_CN.qm`) and falls back to the language code (e.g., `ws1_zh.qm`) if the specific region is missing.
        *   **Optimization**: Temporarily disables `updatesEnabled` on top-level widgets during the switch to prevent flickering and redundant repaints while `QTranslator`s are installed.
    *   **Key Methods**: `RegisterTranslator`, `SwitchLanguage`, `GetCurrentLocale`.

*   **Theme Management (`IThemeService`)**:
    *   **Role**: Controls the visual appearance via QPalette, QSS, and semantic colors.
    *   **Architecture**:
        *   **INI Configuration**: Themes are defined in INI files (e.g., `dark.ini`). The `[Palette]` section maps directly to `QPalette::ColorRole`, while `[Colors]` defines semantic roles (e.g., `THEME_COLOR_BrandColor`).
        *   **QSS Generation**: Uses a template (`base.qss`) containing placeholders like `@@THEME_COLOR_PanelBackground@@`. At runtime, these are replaced with values from the active INI configuration to generate the final stylesheet.
        *   **Icon Abstraction**: `GetIcon` automatically resolves icon paths based on the current theme type (e.g., returning `.../dark/icon.svg` vs `.../light/icon.svg`), simplifying plugin logic.
    *   **Key Methods**: `LoadTheme`, `GetColor`, `GetIcon`.
    *   **Performance**: Similar to the Language Service, it disables UI updates during the heavy `qApp->setStyleSheet` operation to ensure a smooth transition.

### 2.4 Business Plugin (`ws1`)

The `Ws1` plugin was refactored to become a consumer of the new `dscore` facilities.

*   **`Ws1Page` Refactoring**:
    *   Inherits from `QWidget` and implements `IWorkbench`.
    *   **Initialization**: Uses `SetupDefaultUi` to populate the `WorkbenchLayout`.
    *   **Theme Integration**:
        *   Connects to `IThemeService::ThemeChanged` signal.
        *   Dynamically updates icons for buttons and actions via `UpdateIcons` to ensure contrast against the active theme (e.g., loading dark icons for light themes).
    *   **Language Integration**:
        *   Connects to `ILanguageService::LanguageChanged` signal.
        *   Implements `retranslateUi` to dynamically refresh UI text (labels, buttons, tree models) when the system language changes.
    *   **Content**:
        *   **Left Sidebar**: Contains a `QTreeView` ("Model Tree").
        *   **Center**: A placeholder `QLabel` representing the 3D rendering area.
        *   **Top Right**: A `CollapsibleWidget` showing device status (Overlay).
        *   **Bottom Left**: A coordinate display label (Overlay).
        *   **Bottom Center**: A functional button bar (Overlay).
        *   **Notifications**: Demonstrates the animation system via a delayed trigger.

## 3. Design Principles

1.  **Separation of Concerns (SoC)**:
    *   **Core (`dscore`)**: Handles *how* widgets are placed (layout, layering, animation, docking).
    *   **Plugin (`ws1`)**: Handles *what* widgets are created (specific buttons, trees, labels).
    *   This allows future workspaces (`Ws2`, `Ws3`) to reuse the exact same skeleton without code duplication.

2.  **Layered Viewport (HUD)**:
    *   Instead of standard layouts which can't easily overlay widgets on top of a 3D window, a manual layout strategy (`OverlayCanvas`) is used.
    *   **Z-Ordering**: Explicit management ensures HUD elements always render above the 3D background.

3.  **Service-Oriented Architecture**:
    *   Plugins do not directly instantiate concrete core classes where interfaces suffice (though `WorkbenchLayout` is used directly as a base widget).
    *   Theme and Context services are accessed via the global `ServiceLocator` pattern (`GetTObject`).

4.  **Visual Consistency**:
    *   All sidebars use `QTabWidget::West` forced via stylesheets to ensure vertical orientation.
    *   Notifications are centralized and consistent across the application.

## 4. Implementation Details & Fixes

*   **Linker Issues (Windows)**: Solved by correctly applying `DS_CORE_DLLSPEC` (`__declspec(dllexport/import)`) to `WorkbenchLayout` and `CollapsibleWidget`.
*   **Layout Glitches**:
    *   **Horizontal Sidebar**: Fixed by applying a targeted Stylesheet to `left_tab_widget_` in C++ to force proper West tab rendering.
    *   **Broken Tree**: Fixed by ensuring the model is set on the `QTreeView` *after* proper initialization.
*   **Theming**:
    *   Switched from manual color updates in C++ to a QSS-driven approach. Custom widgets (like the 3D placeholder) are now assigned specific object names (e.g., `#ws1_bg_label`), allowing the `ThemeService` to automatically apply colors via the generated stylesheet.

## 5. Pending Tasks & Future Work

The following items were identified as necessary improvements but were outside the immediate scope of the initial prototype:

### 5.1 Architecture & Refactoring
*   **Workbench Registration**: Currently, `Ws1Page` registers itself as the `IWorkbench` service in its constructor (`AddObject(this)`). This causes conflicts if multiple pages are created.
    *   *Done*: Implemented `IPageManager` and `PageManager` in `dscore`. `MainWindow` instantiates `PageManager` and connects it to `main_tab_widget_`. `PageManager` now listens to tab changes and dynamically registers/unregisters the `IWorkbench` implementation of the active tab (`Ws1Page` no longer registers itself). This ensures only one `IWorkbench` is active at a time.
*   **Tree View Data**: The `QTreeView` in `Ws1Page` uses hardcoded dummy data.
    *   *Done*: Updated `setupModel()` in `Ws1Page.cpp` to generate a hierarchical dummy tree with nested items.
*   **Sidebar Collapse Button**: Add a collapse/expand button to the right of the left sidebar.
    *   *Done*: Implemented in `WorkbenchLayout.cpp` (logic) and `OverlayCanvas.cpp` (positioning). The button toggles the visibility/width of the sidebar using the `QSplitter` and shows a left/right arrow.

### 5.2 UI & Theming
*   **QSS Migration**: Several styles (Sidebar Tabs, 3D Background color, Notification styling) are currently hardcoded in C++ (`setStyleSheet`).
    *   *Done*: Moved these into `base.qss`, `dark.ini`, and `light.ini`. Used dynamic object names (e.g., `#ws1_bg_label`) to target them cleanly.
*   **Icons**: Ensure all icon paths are valid and resources are properly loaded in the final build configuration.

### 5.3 OverlayCanvas Improvements
*   **State Persistence**: The `CollapsibleWidget` state (open/closed) and `MainSplitter` positions are lost on restart.
    *   *Todo*: Implement `ISettings` integration to save/restore these values.
*   **Advanced Docking (Squeeze)**: Currently, `OverlayCanvas` places squeeze widgets sequentially.
    *   *Todo*: Add support for priority ordering and multiple widgets on the same side (e.g., MenuBar + Toolbar + StatusBar).
*   **Notification Queue**: `ShowNotification` currently replaces the active notification immediately.
    *   *Todo*: Implement a queue to show multiple messages sequentially if they arrive in quick succession.