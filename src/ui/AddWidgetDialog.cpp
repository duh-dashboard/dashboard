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

#include "AddWidgetDialog.h"

#include "core/WidgetManager.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

namespace dashboard {

AddWidgetDialog::AddWidgetDialog(WidgetManager& widgetManager, QWidget* parent)
    : QDialog(parent), widgetManager_(widgetManager) {
    setWindowTitle("Add Widget");
    setMinimumSize(350, 300);
    setupUi();
}

void AddWidgetDialog::setupUi() {
    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Select widgets to add:", this));

    listWidget_ = new QListWidget(this);
    listWidget_->setSelectionMode(QAbstractItemView::ExtendedSelection);

    const auto& widgets = widgetManager_.widgets();
    for (auto* widget : widgets) {
        auto meta = widget->metadata();
        auto* item = new QListWidgetItem(listWidget_);
        item->setText(QString("%1  —  %2").arg(meta.name, meta.description));
        item->setData(Qt::UserRole, QVariant::fromValue(reinterpret_cast<quintptr>(widget)));
    }

    // Select first item by default
    if (listWidget_->count() > 0) {
        listWidget_->setCurrentRow(0);
    }

    layout->addWidget(listWidget_, 1);

    auto* buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    // Double-click to add immediately
    connect(listWidget_, &QListWidget::itemDoubleClicked, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

std::vector<IWidget*> AddWidgetDialog::selectedWidgets() const {
    std::vector<IWidget*> result;
    for (auto* item : listWidget_->selectedItems()) {
        auto ptr = item->data(Qt::UserRole).value<quintptr>();
        result.push_back(reinterpret_cast<IWidget*>(ptr));
    }
    return result;
}

}  // namespace dashboard
