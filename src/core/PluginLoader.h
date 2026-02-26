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

#include <QDir>
#include <QMap>
#include <QPluginLoader>
#include <QString>
#include <QStringList>
#include <memory>
#include <vector>

namespace dashboard {

class PluginLoader {
public:
    PluginLoader();

    void addSearchPath(const QString& path);
    QStringList searchPaths() const;

    std::vector<IWidget*> loadAll();

private:
    QStringList searchPaths_;
    QMap<QString, QPluginLoader*> loaded_;
};

}  // namespace dashboard
