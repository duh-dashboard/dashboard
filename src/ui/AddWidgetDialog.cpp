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
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <algorithm>

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

    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText("Search widgets...");
    layout->addWidget(searchEdit_);

    listWidget_ = new QListWidget(this);
    listWidget_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(listWidget_, 1);

    emptyStateLabel_ = new QLabel("No widgets match your search.", this);
    emptyStateLabel_->setAlignment(Qt::AlignCenter);
    layout->addWidget(emptyStateLabel_);

    buttonBox_ =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttonBox_);

    const auto& widgets = widgetManager_.widgets();
    widgetEntries_.reserve(static_cast<qsizetype>(widgets.size()));
    for (auto* widget : widgets) {
        auto meta = widget->metadata();
        widgetEntries_.push_back({widget, meta.name, meta.description});
    }

    std::sort(widgetEntries_.begin(), widgetEntries_.end(),
              [](const WidgetEntry& a, const WidgetEntry& b) {
                  return QString::compare(a.name, b.name, Qt::CaseInsensitive) < 0;
              });

    applyFilter(QString());
    updateOkButtonState();

    connect(searchEdit_, &QLineEdit::textChanged, this, &AddWidgetDialog::applyFilter);
    connect(listWidget_, &QListWidget::itemSelectionChanged, this,
            &AddWidgetDialog::updateOkButtonState);

    // Double-click to add immediately only if there is an active selection.
    connect(listWidget_, &QListWidget::itemDoubleClicked, this,
            [this](QListWidgetItem*) { if (!selectedWidgets().empty()) accept(); });
    connect(buttonBox_, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox_, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void AddWidgetDialog::applyFilter(const QString& filterText) {
    const QString filter = filterText.trimmed();

    listWidget_->clear();

    int matches = 0;
    for (const auto& entry : widgetEntries_) {
        const bool include = filter.isEmpty()
                             || entry.name.contains(filter, Qt::CaseInsensitive)
                             || entry.description.contains(filter, Qt::CaseInsensitive);

        if (!include) {
            continue;
        }

        auto* item = new QListWidgetItem(listWidget_);
        item->setText(QString("%1  —  %2").arg(entry.name, entry.description));
        item->setData(Qt::UserRole,
                      QVariant::fromValue(reinterpret_cast<quintptr>(entry.widget)));
        ++matches;
    }

    emptyStateLabel_->setVisible(matches == 0);
    listWidget_->setVisible(matches > 0);

    updateOkButtonState();
}

void AddWidgetDialog::updateOkButtonState() {
    if (auto* okButton = buttonBox_->button(QDialogButtonBox::Ok)) {
        okButton->setEnabled(!selectedWidgets().empty());
    }
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
