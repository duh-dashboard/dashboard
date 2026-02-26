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

#include "WidgetDataStore.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

namespace dashboard {

QString WidgetDataStore::dirPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
           + "/widget-data";
}

QString WidgetDataStore::filePath(const QString& instanceId) {
    return dirPath() + "/" + instanceId + ".json";
}

QJsonObject WidgetDataStore::load(const QString& instanceId) {
    QFile file(filePath(instanceId));
    if (!file.open(QFile::ReadOnly)) return {};
    return QJsonDocument::fromJson(file.readAll()).object();
}

void WidgetDataStore::save(const QString& instanceId, const QJsonObject& data) {
    QDir().mkpath(dirPath());
    QFile file(filePath(instanceId));
    if (file.open(QFile::WriteOnly)) {
        file.write(QJsonDocument(data).toJson());
    }
}

void WidgetDataStore::remove(const QString& instanceId) {
    QFile::remove(filePath(instanceId));
}

}  // namespace dashboard
