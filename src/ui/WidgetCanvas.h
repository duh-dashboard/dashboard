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

#include <QColor>
#include <QList>
#include <QPixmap>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace dashboard {

class ConfigStore;
class WidgetFrame;

class WidgetCanvas : public QWidget {
    Q_OBJECT

public:
    explicit WidgetCanvas(QWidget* parent = nullptr);

    WidgetFrame* addWidget(IWidget* widget, const QPoint& position = {});
    void removeWidget(WidgetFrame* frame);

    void applyBackground(ConfigStore& config);

    QPoint centerPosition(const QSize& widgetSize) const;
    const QList<WidgetFrame*>& frames() const;
    void clampFramePositions();

signals:
    void addWidgetRequested();
    void widgetAdded(WidgetFrame* frame);
    void widgetRemoved(const QString& instanceId);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void positionAddButton();

    QPushButton* addButton_;
    QList<WidgetFrame*> frames_;

    // Background state
    QString bgMode_;
    QColor bgColor_{0x2d, 0x2d, 0x2d};
    QPixmap bgPixmap_;
    double bgAlpha_ = 1.0;
};

}  // namespace dashboard
