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

#include "WidgetFrame.h"

#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <dashboard/IWidget.h>

namespace dashboard {

WidgetFrame::WidgetFrame(QWidget* content, QWidget* parent)
    : QFrame(parent), content_(content) {
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setLineWidth(0);
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("widgetFrame");
    setStyleSheet(
        "#widgetFrame {"
        "  background-color: #26263a;"
        "  border: 1px solid #3e3e60;"
        "  border-radius: 10px;"
        "}");

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 140));
    setGraphicsEffect(shadow);

    setMouseTracking(true);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->addWidget(content_);

    // Delete button — overlaid on top, not part of the layout
    deleteButton_ = new QPushButton("\u2715", this);
    deleteButton_->setFixedSize(22, 22);
    deleteButton_->setStyleSheet(
        "QPushButton { background-color: #36364e; color: #a0a8c8; border: none; "
        "border-radius: 11px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #d04040; color: white; }");
    deleteButton_->hide();
    deleteButton_->raise();

    connect(deleteButton_, &QPushButton::clicked, this, [this]() {
        auto answer = QMessageBox::question(
            this, "Delete Widget",
            QString("Remove \"%1\" from the dashboard?")
                .arg(pluginName_.isEmpty() ? widgetId_ : pluginName_),
            QMessageBox::Yes | QMessageBox::No);
        if (answer == QMessageBox::Yes) {
            emit deleteRequested(this);
        }
    });
}

QWidget* WidgetFrame::contentWidget() const {
    return content_;
}

void WidgetFrame::setWidgetId(const QString& id) {
    widgetId_ = id;
}

QString WidgetFrame::widgetId() const {
    return widgetId_;
}

void WidgetFrame::setPluginName(const QString& name) {
    pluginName_ = name;
}

QString WidgetFrame::pluginName() const {
    return pluginName_;
}

void WidgetFrame::setIWidget(IWidget* widget) {
    iwidget_ = widget;
}

IWidget* WidgetFrame::iwidget() const {
    return iwidget_;
}

static constexpr int kResizeZone = 8;

WidgetFrame::ResizeEdge WidgetFrame::hitTest(const QPoint& pos) const {
    bool left   = pos.x() < kResizeZone;
    bool right  = pos.x() >= width() - kResizeZone;
    bool top    = pos.y() < kResizeZone;
    bool bottom = pos.y() >= height() - kResizeZone;

    if (top    && left)  return ResizeEdge::TopLeft;
    if (top    && right) return ResizeEdge::TopRight;
    if (bottom && left)  return ResizeEdge::BottomLeft;
    if (bottom && right) return ResizeEdge::BottomRight;
    if (left)            return ResizeEdge::Left;
    if (right)           return ResizeEdge::Right;
    if (top)             return ResizeEdge::Top;
    if (bottom)          return ResizeEdge::Bottom;
    return ResizeEdge::None;
}

void WidgetFrame::applyCursorForEdge(ResizeEdge edge) {
    switch (edge) {
        case ResizeEdge::TopLeft:
        case ResizeEdge::BottomRight:
            setCursor(Qt::SizeFDiagCursor); break;
        case ResizeEdge::TopRight:
        case ResizeEdge::BottomLeft:
            setCursor(Qt::SizeBDiagCursor); break;
        case ResizeEdge::Left:
        case ResizeEdge::Right:
            setCursor(Qt::SizeHorCursor); break;
        case ResizeEdge::Top:
        case ResizeEdge::Bottom:
            setCursor(Qt::SizeVerCursor); break;
        default:
            setCursor(Qt::ArrowCursor); break;
    }
}

void WidgetFrame::applyResize(const QPoint& parentPos) {
    QPoint delta = parentPos - resizeGlobalStart_;
    QRect geo = resizeOriginalGeometry_;

    bool adjLeft   = (resizeEdge_ == ResizeEdge::Left ||
                      resizeEdge_ == ResizeEdge::TopLeft ||
                      resizeEdge_ == ResizeEdge::BottomLeft);
    bool adjRight  = (resizeEdge_ == ResizeEdge::Right ||
                      resizeEdge_ == ResizeEdge::TopRight ||
                      resizeEdge_ == ResizeEdge::BottomRight);
    bool adjTop    = (resizeEdge_ == ResizeEdge::Top ||
                      resizeEdge_ == ResizeEdge::TopLeft ||
                      resizeEdge_ == ResizeEdge::TopRight);
    bool adjBottom = (resizeEdge_ == ResizeEdge::Bottom ||
                      resizeEdge_ == ResizeEdge::BottomLeft ||
                      resizeEdge_ == ResizeEdge::BottomRight);

    if (adjLeft) {
        int w = qBound(minimumWidth(), geo.right() - (geo.left() + delta.x()) + 1, maximumWidth());
        geo.setLeft(geo.right() - w + 1);
    }
    if (adjRight) {
        int w = qBound(minimumWidth(), (geo.right() + delta.x()) - geo.left() + 1, maximumWidth());
        geo.setRight(geo.left() + w - 1);
    }
    if (adjTop) {
        int h = qBound(minimumHeight(), geo.bottom() - (geo.top() + delta.y()) + 1, maximumHeight());
        geo.setTop(geo.bottom() - h + 1);
    }
    if (adjBottom) {
        int h = qBound(minimumHeight(), (geo.bottom() + delta.y()) - geo.top() + 1, maximumHeight());
        geo.setBottom(geo.top() + h - 1);
    }

    setGeometry(geo);
}

void WidgetFrame::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        ResizeEdge edge = hitTest(event->pos());
        if (edge != ResizeEdge::None) {
            resizing_ = true;
            resizeEdge_ = edge;
            resizeOriginalGeometry_ = geometry();
            resizeGlobalStart_ = mapToParent(event->pos());
        } else {
            dragging_ = true;
            dragStart_ = event->pos();
        }
        raise();
        deleteButton_->raise();
    }
    QFrame::mousePressEvent(event);
}

void WidgetFrame::mouseMoveEvent(QMouseEvent* event) {
    if (resizing_) {
        applyResize(mapToParent(event->pos()));
    } else if (dragging_) {
        QPoint delta = event->pos() - dragStart_;
        move(pos() + delta);
    } else {
        applyCursorForEdge(hitTest(event->pos()));
    }
    QFrame::mouseMoveEvent(event);
}

void WidgetFrame::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (resizing_) {
            resizing_ = false;
            emit resized(size());
        } else if (dragging_) {
            dragging_ = false;
            emit moved(pos());
        }
    }
    QFrame::mouseReleaseEvent(event);
}

void WidgetFrame::enterEvent(QEnterEvent* /*event*/) {
    deleteButton_->show();
    deleteButton_->raise();
}

void WidgetFrame::leaveEvent(QEvent* /*event*/) {
    deleteButton_->hide();
}

void WidgetFrame::resizeEvent(QResizeEvent* event) {
    QFrame::resizeEvent(event);
    positionDeleteButton();
}

void WidgetFrame::positionDeleteButton() {
    deleteButton_->move(width() - deleteButton_->width() - 6, 6);
}

}  // namespace dashboard
