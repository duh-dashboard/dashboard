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

#include "DashboardApp.h"

#include "core/ConfigStore.h"
#include "core/LayoutEngine.h"
#include "core/PluginLoader.h"
#include "core/WidgetManager.h"
#include "ui/DashboardWindow.h"

#include <QFile>

namespace dashboard {

DashboardApp::DashboardApp(int& argc, char** argv) : QApplication(argc, argv) {
    setApplicationName("Dashboard");
    setOrganizationName("Dashboard");
    setApplicationVersion("0.1.0");

    QFile styleFile(":/theme.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        setStyleSheet(styleFile.readAll());
    }

    config_ = std::make_unique<ConfigStore>();
    layoutEngine_ = std::make_unique<LayoutEngine>();
    pluginLoader_ = std::make_unique<PluginLoader>();
    widgetManager_ = std::make_unique<WidgetManager>(*pluginLoader_);
    window_ = std::make_unique<DashboardWindow>(*widgetManager_, *config_, *layoutEngine_);
}

DashboardApp::~DashboardApp() = default;

int DashboardApp::run() {
    window_->show();
    widgetManager_->loadPlugins();
    window_->restoreLayout();
    return exec();
}

}  // namespace dashboard
