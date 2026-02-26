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

#include <QJsonObject>
#include <QString>

namespace dashboard {

// Reads and writes per-widget-instance data files.
// Each file lives at: <AppConfigLocation>/widget-data/<instanceId>.json
class WidgetDataStore {
public:
    static QJsonObject load(const QString& instanceId);
    static void save(const QString& instanceId, const QJsonObject& data);
    static void remove(const QString& instanceId);

private:
    static QString dirPath();
    static QString filePath(const QString& instanceId);
};

}  // namespace dashboard
