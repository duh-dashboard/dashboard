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

#include "WidgetCanvas.h"

#include "WidgetFrame.h"
#include "core/ConfigStore.h"

#include <QMouseEvent>
#include <QPainter>

namespace dashboard {

static constexpr int kAddButtonSize = 36;
static constexpr int kAddButtonMargin = 16;
static constexpr int kHoverZone = 120;
static constexpr int kEdgePadding = 10;

WidgetCanvas::WidgetCanvas(QWidget* parent) : QWidget(parent) {
    setAutoFillBackground(false);
    setMouseTracking(true);

    addButton_ = new QPushButton("+", this);
    addButton_->setFixedSize(kAddButtonSize, kAddButtonSize);
    addButton_->setStyleSheet(
        "QPushButton { background-color: #3a6acc; color: white; border: none; "
        "border-radius: 18px; font-size: 22px; font-weight: bold; }"
        "QPushButton:hover { background-color: #4a7ae0; }"
        "QPushButton:pressed { background-color: #2c5aa8; }");
    addButton_->hide();
    addButton_->raise();

    connect(addButton_, &QPushButton::clicked, this, &WidgetCanvas::addWidgetRequested);
}

WidgetFrame* WidgetCanvas::addWidget(IWidget* widget, const QPoint& position) {
    QWidget* content = widget->createWidget(this);
    auto* frame = new WidgetFrame(content, this);
    frame->setIWidget(widget);

    auto meta = widget->metadata();
    frame->setPluginName(meta.name);
    frame->setMinimumSize(meta.minSize);
    frame->setMaximumSize(meta.maxSize);
    frame->resize(meta.defaultSize);
    frame->move(position);
    frame->show();
    frame->raise();

    frames_.append(frame);

    connect(frame, &WidgetFrame::deleteRequested, this, &WidgetCanvas::removeWidget);

    emit widgetAdded(frame);

    return frame;
}

void WidgetCanvas::removeWidget(WidgetFrame* frame) {
    QString instanceId = frame->widgetId();
    frames_.removeOne(frame);
    frame->hide();
    frame->deleteLater();
    emit widgetRemoved(instanceId);
}

void WidgetCanvas::applyBackground(ConfigStore& config) {
    bgMode_ = config.value("background/mode", "color").toString();
    int opacity = config.value("background/opacity", 100).toInt();
    bgAlpha_ = qBound(0, opacity, 100) / 100.0;

    if (bgMode_ == "image") {
        QString path = config.value("background/imagePath", "").toString();
        bgPixmap_ = QPixmap(path);
    } else {
        bgPixmap_ = QPixmap();
    }

    bgColor_ = QColor(config.value("background/color", "#1a1a2a").toString());

    update();
}

void WidgetCanvas::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);

    if (bgMode_ == "image" && !bgPixmap_.isNull()) {
        QPixmap scaled = bgPixmap_.scaled(size(), Qt::KeepAspectRatioByExpanding,
                                          Qt::SmoothTransformation);
        // Center the scaled image
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;

        if (bgAlpha_ < 1.0) {
            painter.fillRect(rect(), Qt::white);
            painter.setOpacity(bgAlpha_);
        }
        painter.drawPixmap(x, y, scaled);
    } else {
        QColor color = bgColor_;
        color.setAlphaF(bgAlpha_);
        painter.fillRect(rect(), color);
    }
}

QPoint WidgetCanvas::centerPosition(const QSize& widgetSize) const {
    return QPoint((width() - widgetSize.width()) / 2,
                  (height() - widgetSize.height()) / 2);
}

const QList<WidgetFrame*>& WidgetCanvas::frames() const {
    return frames_;
}

void WidgetCanvas::mouseMoveEvent(QMouseEvent* event) {
    QPoint pos = event->pos();
    bool inZone = pos.x() >= width() - kHoverZone && pos.y() <= kHoverZone;
    if (inZone) {
        addButton_->show();
        addButton_->raise();
    } else if (!addButton_->underMouse()) {
        addButton_->hide();
    }
    QWidget::mouseMoveEvent(event);
}

void WidgetCanvas::leaveEvent(QEvent* event) {
    if (!addButton_->underMouse()) {
        addButton_->hide();
    }
    QWidget::leaveEvent(event);
}

void WidgetCanvas::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    positionAddButton();
    clampFramePositions();
}

void WidgetCanvas::positionAddButton() {
    addButton_->move(width() - kAddButtonSize - kAddButtonMargin, kAddButtonMargin);
}

void WidgetCanvas::clampFramePositions() {
    for (auto* frame : frames_) {
        int x = qBound(kEdgePadding, frame->x(), width() - frame->width() - kEdgePadding);
        int y = qBound(kEdgePadding, frame->y(), height() - frame->height() - kEdgePadding);
        frame->move(x, y);
    }
}

}  // namespace dashboard
