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

#include "PluginLoader.h"

#include <QCoreApplication>
#include <QDirIterator>
#include <QPluginLoader>
#include <qlogging.h>

namespace dashboard {

PluginLoader::PluginLoader() {
    // Default: look for plugins next to the executable
    addSearchPath(QCoreApplication::applicationDirPath() + "/plugins");

#ifdef DASHBOARD_PLUGIN_DIR
    // Installed layout: search the compile-time-defined system plugin directory.
    addSearchPath(QStringLiteral(DASHBOARD_PLUGIN_DIR));
#endif
}

void PluginLoader::addSearchPath(const QString& path) {
    if (!searchPaths_.contains(path)) {
        searchPaths_.append(path);
    }
}

QStringList PluginLoader::searchPaths() const {
    return searchPaths_;
}

std::vector<IWidget*> PluginLoader::loadAll() {
    std::vector<IWidget*> widgets;

    for (const auto& searchPath : searchPaths_) {
        QDir dir(searchPath);
        if (!dir.exists()) {
            continue;
        }

        const auto entries = dir.entryList(QDir::Files);
        for (const auto& fileName : entries) {
            QString filePath = dir.absoluteFilePath(fileName);

            if (loaded_.contains(filePath)) {
                if (auto* w = qobject_cast<IWidget*>(loaded_[filePath]->instance())) {
                    widgets.push_back(w);
                }
                continue;
            }

            auto* loader = new QPluginLoader(filePath);
            if (QObject* instance = loader->instance()) {
                if (auto* widget = qobject_cast<IWidget*>(instance)) {
                    qInfo() << "Loaded widget plugin:" << filePath;
                    loaded_[filePath] = loader;
                    widgets.push_back(widget);
                } else {
                    qWarning() << "Plugin does not implement IWidget:" << filePath;
                    loader->unload();
                    delete loader;
                }
            } else {
                qWarning() << "Failed to load plugin:" << filePath << loader->errorString();
                delete loader;
            }
        }
    }

    return widgets;
}

}  // namespace dashboard
