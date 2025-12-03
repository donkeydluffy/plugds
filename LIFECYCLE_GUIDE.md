# PlugDS Lifecycle Management Guide

This document outlines the strict lifecycle phases required to avoid race conditions and initialization errors in the `plugds` microkernel architecture.

## The Three Phases of Startup

### Phase 1: Infrastructure Initialization
*   **Trigger**: `ComponentLoader::LoadComponents` -> `CoreComponent::InitialiseEvent`
*   **Responsible**: `dscore` (Core Plugin)
*   **Actions**:
    *   Create and Register Global Managers (`ContextManager`, `CommandManager`, `StatusbarManager`, etc.).
    *   Create `MainWindow` (Skeleton only).
    *   **PROHIBITED**: Do NOT populate dynamic UI (Menus/Toolbars) yet. Do NOT activate specific contexts.

### Phase 2: Plugin Registration
*   **Trigger**: `ComponentLoader::LoadComponents` -> `Plugin::InitialiseEvent` (for all other plugins)
*   **Responsible**: Downstream Plugins (e.g., `ws1`, `analysis`)
*   **Actions**:
    *   **Register Contexts**: `contextManager->RegisterContext("my.plugin")`.
    *   **Register Commands**: `commandManager->RegisterCommand(...)`.
    *   **Expose Providers**: Register `IMenuProvider`, `IToolbarProvider` (or similar).
    *   **PROHIBITED**:
        *   Do NOT call `ActivateMode()` or `SetContext()`.
        *   Do NOT assume other plugins are loaded.
        *   Do NOT access UI widgets directly.

### Phase 3: UI Composition & Presentation
*   **Trigger**: `ComponentLoader::LoadComponents` -> `CoreComponent::InitialisationFinishedEvent`
*   **Responsible**: `dscore` (Core Plugin runs LAST in reverse order)
*   **Actions**:
    *   **Discovery**: Query all registered plugins for `IMenuProvider`, `IToolbarProvider`.
    *   **Build**: Construct the Menu Bar, Toolbars, and Status Bar based on registered data.
    *   **Activate**: Restore last used Mode or activate default Mode via `ContextManager->ActivateMode()`.
    *   **Show**: `mainWindow->show()`.

## Common Pitfalls & Violations

### ❌ Violation: Premature Activation
```cpp
// BAD: In MyPlugin::InitialiseEvent
modeManager->ActivateMode(myModeId); 
// Reason: UI is not built yet. ContextManager signals might trigger updates on null widgets.
```

### ❌ Violation: Hardcoded Load Order
```cpp
// BAD: Assuming Plugin B is loaded before Plugin A without explicit dependency
auto* b = GetTObject<IPluginB>(); 
b->DoSomething(); // Crash if B is not loaded.
```

### ✅ Correct Pattern: Registration Only
```cpp
// GOOD: In MyPlugin::InitialiseEvent
contextId_ = contextManager->RegisterContext("my.ctx");
// Wait for user interaction or Phase 3 to activate.
```

## Context Manager Usage

*   **Global Context**: Always active.
*   **Mode Context**: One active at a time (e.g., "Edit Mode", "Analysis Mode").
*   **Sub-Context**: Active only when specific Mode is active.
*   **Switching**: Use `ContextManager::ActivateMode(modeId)`. This handles saving/restoring sub-contexts automatically.
