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

#include <dashboard/IWidget.h>
#include <dashboard/WidgetContext.h>

#include <QObject>
#include <QString>
#include <memory>
#include <vector>

namespace dashboard {

class PluginLoader;
class WidgetCanvas;

class WidgetManager : public QObject {
    Q_OBJECT

public:
    explicit WidgetManager(PluginLoader& pluginLoader, QObject* parent = nullptr);

    void loadPlugins();
    const std::vector<IWidget*>& widgets() const;
    IWidget* findByName(const QString& name) const;

signals:
    void widgetLoaded(IWidget* widget);

private:
    PluginLoader& pluginLoader_;
    std::vector<IWidget*> widgets_;
};

}  // namespace dashboard
