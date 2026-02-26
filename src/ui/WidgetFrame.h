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

#include <QFrame>
#include <QPoint>
#include <QPushButton>
#include <QRect>

namespace dashboard {

class IWidget;

class WidgetFrame : public QFrame {
    Q_OBJECT

public:
    enum class ResizeEdge {
        None,
        Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };

    explicit WidgetFrame(QWidget* content, QWidget* parent = nullptr);

    QWidget* contentWidget() const;
    void setWidgetId(const QString& id);
    QString widgetId() const;
    void setPluginName(const QString& name);
    QString pluginName() const;
    void setIWidget(IWidget* widget);
    IWidget* iwidget() const;

signals:
    void moved(const QPoint& newPos);
    void resized(const QSize& newSize);
    void deleteRequested(WidgetFrame* frame);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void positionDeleteButton();
    ResizeEdge hitTest(const QPoint& localPos) const;
    void applyCursorForEdge(ResizeEdge edge);
    void applyResize(const QPoint& parentPos);

    QWidget* content_;
    QPushButton* deleteButton_;
    IWidget* iwidget_ = nullptr;
    QString widgetId_;
    QString pluginName_;

    // Drag state
    QPoint dragStart_;
    bool dragging_ = false;

    // Resize state
    bool resizing_ = false;
    ResizeEdge resizeEdge_ = ResizeEdge::None;
    QRect resizeOriginalGeometry_;
    QPoint resizeGlobalStart_;
};

}  // namespace dashboard
