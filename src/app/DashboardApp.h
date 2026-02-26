// Copyright (C) 2026 Sean Moon
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <QApplication>
#include <memory>

namespace dashboard {

class ConfigStore;
class LayoutEngine;
class PluginLoader;
class WidgetManager;
class DashboardWindow;

class DashboardApp : public QApplication {
    Q_OBJECT

public:
    DashboardApp(int& argc, char** argv);
    ~DashboardApp() override;

    int run();

private:
    std::unique_ptr<ConfigStore> config_;
    std::unique_ptr<LayoutEngine> layoutEngine_;
    std::unique_ptr<PluginLoader> pluginLoader_;
    std::unique_ptr<WidgetManager> widgetManager_;
    std::unique_ptr<DashboardWindow> window_;
};

}  // namespace dashboard
