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

#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QWidget>

namespace dashboard {

class TitleBar : public QWidget {
    Q_OBJECT

public:
    explicit TitleBar(QWidget* parent = nullptr);
    void setTitle(const QString& title);
    void setMenu(QMenu* menu);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateButtonWidths();

    QLabel* titleLabel_;
    QPushButton* menuButton_;
    QPushButton* minButton_;
    QPushButton* maxButton_;
    QPushButton* closeButton_;
    QMenu* menu_ = nullptr;
    bool maximized_ = false;
};

}  // namespace dashboard
