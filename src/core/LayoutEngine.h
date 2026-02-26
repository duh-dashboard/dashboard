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

#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QPoint>
#include <QSize>
#include <QString>

namespace dashboard {

struct WidgetLayout {
    QString instanceId;
    QString pluginName;
    QPoint position;
    QSize size;
};

class LayoutEngine {
public:
    LayoutEngine();

    QString addWidget(const QString& pluginName, const QPoint& pos, const QSize& size);
    void updatePosition(const QString& instanceId, const QPoint& pos);
    void updateSize(const QString& instanceId, const QSize& size);
    void removeWidget(const QString& instanceId);
    WidgetLayout widgetLayout(const QString& instanceId) const;
    QList<WidgetLayout> allLayouts() const;

    void saveToFile(const QString& path) const;
    void loadFromFile(const QString& path);

    static QString layoutFilePath();

private:
    QString generateInstanceId(const QString& pluginName) const;

    QJsonArray serialize() const;
    void deserialize(const QJsonArray& data);

    QMap<QString, WidgetLayout> layouts_;
};

}  // namespace dashboard
