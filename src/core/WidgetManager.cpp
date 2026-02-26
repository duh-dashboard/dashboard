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

#include "WidgetManager.h"

#include "PluginLoader.h"

namespace dashboard {

WidgetManager::WidgetManager(PluginLoader& pluginLoader, QObject* parent)
    : QObject(parent), pluginLoader_(pluginLoader) {}

void WidgetManager::loadPlugins() {
    auto loaded = pluginLoader_.loadAll();
    for (auto* widget : loaded) {
        if (std::find(widgets_.begin(), widgets_.end(), widget) != widgets_.end()) {
            continue;
        }
        widgets_.push_back(widget);
        emit widgetLoaded(widget);
    }
}

const std::vector<IWidget*>& WidgetManager::widgets() const {
    return widgets_;
}

IWidget* WidgetManager::findByName(const QString& name) const {
    for (auto* widget : widgets_) {
        if (widget->metadata().name == name) {
            return widget;
        }
    }
    return nullptr;
}

}  // namespace dashboard
