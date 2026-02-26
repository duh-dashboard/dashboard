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

#include "LayoutEngine.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

namespace dashboard {

LayoutEngine::LayoutEngine() = default;

QString LayoutEngine::generateInstanceId(const QString& pluginName) const {
    int maxNum = 0;
    QString prefix = pluginName + "_";
    for (auto it = layouts_.cbegin(); it != layouts_.cend(); ++it) {
        if (it.key().startsWith(prefix)) {
            bool ok = false;
            int num = it.key().mid(prefix.length()).toInt(&ok);
            if (ok && num > maxNum) {
                maxNum = num;
            }
        }
    }
    return prefix + QString::number(maxNum + 1);
}

QString LayoutEngine::addWidget(const QString& pluginName, const QPoint& pos, const QSize& size) {
    QString id = generateInstanceId(pluginName);
    layouts_[id] = {id, pluginName, pos, size};
    return id;
}

void LayoutEngine::updatePosition(const QString& instanceId, const QPoint& pos) {
    if (layouts_.contains(instanceId)) {
        layouts_[instanceId].position = pos;
    }
}

void LayoutEngine::updateSize(const QString& instanceId, const QSize& size) {
    if (layouts_.contains(instanceId)) {
        layouts_[instanceId].size = size;
    }
}

void LayoutEngine::removeWidget(const QString& instanceId) {
    layouts_.remove(instanceId);
}

WidgetLayout LayoutEngine::widgetLayout(const QString& instanceId) const {
    return layouts_.value(instanceId);
}

QList<WidgetLayout> LayoutEngine::allLayouts() const {
    return layouts_.values();
}

QJsonArray LayoutEngine::serialize() const {
    QJsonArray arr;
    for (const auto& layout : layouts_) {
        QJsonObject obj;
        obj["instanceId"] = layout.instanceId;
        obj["pluginName"] = layout.pluginName;
        obj["x"] = layout.position.x();
        obj["y"] = layout.position.y();
        obj["width"] = layout.size.width();
        obj["height"] = layout.size.height();
        arr.append(obj);
    }
    return arr;
}

void LayoutEngine::deserialize(const QJsonArray& data) {
    layouts_.clear();
    for (const auto& val : data) {
        auto obj = val.toObject();
        WidgetLayout layout;
        layout.instanceId = obj["instanceId"].toString();
        layout.pluginName = obj["pluginName"].toString();
        layout.position = QPoint(obj["x"].toInt(), obj["y"].toInt());
        layout.size = QSize(obj["width"].toInt(), obj["height"].toInt());
        layouts_[layout.instanceId] = layout;
    }
}

void LayoutEngine::saveToFile(const QString& path) const {
    QFileInfo info(path);
    QDir().mkpath(info.absolutePath());
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(serialize());
        file.write(doc.toJson());
    }
}

void LayoutEngine::loadFromFile(const QString& path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        auto doc = QJsonDocument::fromJson(file.readAll());
        deserialize(doc.array());
    }
}

QString LayoutEngine::layoutFilePath() {
    QString configDir =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/layouts";
    return configDir + "/default.json";
}

}  // namespace dashboard
