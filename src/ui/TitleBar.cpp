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

#include "TitleBar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QWindow>

namespace dashboard {

TitleBar::TitleBar(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "TitleBar {"
        "  background-color: #181825;"
        "  border-bottom: 1px solid #2d2d4a;"
        "}");

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    const QString baseBtn =
        "QPushButton {"
        "  background: transparent;"
        "  color: #707090;"
        "  border: none;"
        "  font-size: 16px;"
        "}";

    // Hamburger menu button — left side
    menuButton_ = new QPushButton("\u2630", this);  // ☰
    menuButton_->setStyleSheet(baseBtn + "QPushButton:hover { background: #2d2d4a; color: #c8cee8; }");
    layout->addWidget(menuButton_);

    titleLabel_ = new QLabel(this);
    titleLabel_->setStyleSheet(
        "QLabel { color: #a0a8d0; font-size: 13px; font-weight: 600;"
        " background: transparent; padding-left: 8px; }");
    layout->addWidget(titleLabel_);
    layout->addStretch();

    // Window control buttons — square, width matches height (set in resizeEvent)
    minButton_ = new QPushButton("\u2212", this);  // −
    maxButton_ = new QPushButton("\u25a1", this);  // □
    closeButton_ = new QPushButton("\u00d7", this);  // ×

    minButton_->setStyleSheet(baseBtn + "QPushButton:hover { background: #2d2d4a; color: #c8cee8; }");
    maxButton_->setStyleSheet(baseBtn + "QPushButton:hover { background: #2d2d4a; color: #c8cee8; }");
    closeButton_->setStyleSheet(
        baseBtn + "QPushButton:hover { background: #c0392b; color: white; }");

    layout->addWidget(minButton_);
    layout->addWidget(maxButton_);
    layout->addWidget(closeButton_);

    connect(menuButton_, &QPushButton::clicked, this, [this]() {
        if (menu_) {
            menu_->popup(menuButton_->mapToGlobal(QPoint(0, menuButton_->height())));
        }
    });

    connect(minButton_, &QPushButton::clicked, this,
            [this]() { window()->showMinimized(); });
    connect(maxButton_, &QPushButton::clicked, this, [this]() {
        if (maximized_) {
            window()->showNormal();
            maxButton_->setText("\u25a1");  // □
            maximized_ = false;
        } else {
            window()->showMaximized();
            maxButton_->setText("\u276f\u276f");  // ❐ approximation
            maximized_ = true;
        }
    });
    connect(closeButton_, &QPushButton::clicked, this, [this]() { window()->close(); });
}

void TitleBar::setTitle(const QString& title) {
    titleLabel_->setText(title);
}

void TitleBar::setMenu(QMenu* menu) {
    menu_ = menu;
}

void TitleBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // Use the platform's native move so it works on both X11 and Wayland
        if (auto* handle = window()->windowHandle()) {
            handle->startSystemMove();
        }
    }
    QWidget::mousePressEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        maxButton_->click();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateButtonWidths();
}

void TitleBar::updateButtonWidths() {
    int w = height();  // square buttons
    menuButton_->setFixedWidth(w);
    minButton_->setFixedWidth(w);
    maxButton_->setFixedWidth(w);
    closeButton_->setFixedWidth(w);
}

}  // namespace dashboard
