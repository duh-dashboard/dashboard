# Dashboard

A customizable widget-based desktop application for Linux. Widgets are Qt plugins loaded at runtime — add, remove, move, and resize them freely. Layout and state persist across restarts.

[![CI](https://github.com/duh-dashboard/dashboard/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/duh-dashboard/dashboard/actions/workflows/ci.yml)

## Features

- Frameless window with a custom title bar (minimize / maximize / close / menu)
- Draggable, resizable widget cards on a free-form canvas
- Plugin system: drop a `.so` into the `plugins/` directory and it appears in the Add Widget dialog
- Background customization: solid color with opacity or image
- Window and title bar size configuration
- Single-instance guard

## Requirements

- Linux
- Qt 6.2+ (Widgets, Network)
- CMake 3.21+
- C++20 compiler

## Build

`dashboard` depends on [`widget-sdk`](https://github.com/duh-dashboard/dashboard). Build and install it first if you haven't already:

```sh
cmake -S ../widget-sdk -B build-sdk -DCMAKE_INSTALL_PREFIX=~/.local
cmake --build build-sdk
cmake --install build-sdk
```

Then build the dashboard:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=~/.local
cmake --build build --parallel
```

For a system install with baked-in plugin path:

```sh
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build --parallel
sudo cmake --install build
```

## Running

From the build directory (widgets placed in `build/dashboard/plugins/` are auto-discovered):

```sh
./build/dashboard/dashboard
```

After a system install:

```sh
dashboard
```

The application enforces a single instance via `/tmp/dashboard.lock`.

## Plugin discovery

At runtime the application searches for widget plugins in two locations, in order:

1. `plugins/` — relative to the executable (used during development)
2. `<install-prefix>/lib/dashboard/plugins` — baked in at compile time via `-DCMAKE_INSTALL_PREFIX`

Any `.so` exporting the `IWidget` interface is loaded automatically.

## Data and configuration paths

All files live under `$XDG_CONFIG_HOME/Dashboard` (defaults to `~/.config/Dashboard`):

| Path | Contents |
|---|---|
| `settings` | QSettings file — window geometry, background, title bar height |
| `layouts/default.json` | Widget positions and sizes |
| `widget-data/<instanceId>.json` | Per-widget serialized state |

## Architecture

```
DashboardApp          — QApplication subclass; coordinates startup
PluginLoader          — scans plugin directories, loads IWidget plugins
WidgetManager         — holds loaded plugin instances
LayoutEngine          — manages widget positions/sizes; writes layouts/default.json
ConfigStore           — QSettings wrapper for app-level preferences
WidgetDataStore       — reads/writes per-widget JSON state files
DashboardWindow       — top-level frameless QMainWindow
TitleBar              — custom title bar with menu/min/max/close buttons
WidgetCanvas          — drawing surface; owns and renders WidgetFrames
WidgetFrame           — draggable, resizable card wrapping each plugin widget
SettingsDialog        — background and window configuration modal
AddWidgetDialog       — widget picker modal
```

## Adding widgets

Build the `widgets` project and place the resulting `.so` files in the `plugins/` directory next to the dashboard executable, or install them to `<prefix>/lib/dashboard/plugins`.

See [`widget-sdk/README.md`](https://github.com/duh-dashboard/widget-sdk#dashboard-widget-sdk) for the full widget developer guide.
