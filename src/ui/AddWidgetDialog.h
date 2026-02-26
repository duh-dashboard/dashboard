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

#include <QDialog>
#include <QListWidget>
#include <vector>

namespace dashboard {

class WidgetManager;

class AddWidgetDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddWidgetDialog(WidgetManager& widgetManager, QWidget* parent = nullptr);

    std::vector<IWidget*> selectedWidgets() const;

private:
    void setupUi();

    WidgetManager& widgetManager_;
    QListWidget* listWidget_;
};

}  // namespace dashboard
