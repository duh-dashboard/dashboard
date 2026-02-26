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

#include "SettingsDialog.h"

#include "core/ConfigStore.h"

#include <QColorDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace dashboard {

SettingsDialog::SettingsDialog(ConfigStore& config, QWidget* parent)
    : QDialog(parent), config_(config) {
    setWindowTitle("Settings");
    setMinimumWidth(450);
    setupUi();
    loadFromConfig();
}

void SettingsDialog::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);

    // Background group
    auto* bgGroup = new QGroupBox("Background", this);
    auto* bgLayout = new QVBoxLayout(bgGroup);

    // Color option
    colorRadio_ = new QRadioButton("Solid color", bgGroup);
    auto* colorRow = new QHBoxLayout();
    colorButton_ = new QPushButton(bgGroup);
    colorButton_->setFixedSize(60, 24);
    colorRow->addWidget(colorRadio_);
    colorRow->addWidget(colorButton_);
    colorRow->addStretch();
    bgLayout->addLayout(colorRow);

    // Image option
    imageRadio_ = new QRadioButton("Image", bgGroup);
    auto* imageRow = new QHBoxLayout();
    imagePath_ = new QLineEdit(bgGroup);
    imagePath_->setPlaceholderText("Path to image...");
    imagePath_->setReadOnly(true);
    browseButton_ = new QPushButton("Browse...", bgGroup);
    imageRow->addWidget(imageRadio_);
    imageRow->addWidget(imagePath_, 1);
    imageRow->addWidget(browseButton_);
    bgLayout->addLayout(imageRow);

    // Opacity
    auto* opacityRow = new QHBoxLayout();
    opacityRow->addWidget(new QLabel("Opacity", bgGroup));
    opacitySlider_ = new QSlider(Qt::Horizontal, bgGroup);
    opacitySlider_->setRange(0, 100);
    opacityLabel_ = new QLabel(bgGroup);
    opacityLabel_->setFixedWidth(36);
    opacityLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    opacityRow->addWidget(opacitySlider_, 1);
    opacityRow->addWidget(opacityLabel_);
    bgLayout->addLayout(opacityRow);

    mainLayout->addWidget(bgGroup);

    // Window group
    auto* winGroup = new QGroupBox("Window", this);
    auto* winLayout = new QVBoxLayout(winGroup);

    auto* sizeRow = new QHBoxLayout();
    sizeRow->addWidget(new QLabel("Size", winGroup));
    sizeSlider_ = new QSlider(Qt::Horizontal, winGroup);
    sizeSlider_->setRange(30, 100);
    sizeLabel_ = new QLabel(winGroup);
    sizeLabel_->setFixedWidth(36);
    sizeLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sizeRow->addWidget(sizeSlider_, 1);
    sizeRow->addWidget(sizeLabel_);
    winLayout->addLayout(sizeRow);

    auto* titleBarRow = new QHBoxLayout();
    titleBarRow->addWidget(new QLabel("Title bar height", winGroup));
    titleBarHeightSlider_ = new QSlider(Qt::Horizontal, winGroup);
    titleBarHeightSlider_->setRange(24, 56);
    titleBarHeightLabel_ = new QLabel(winGroup);
    titleBarHeightLabel_->setFixedWidth(36);
    titleBarHeightLabel_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    titleBarRow->addWidget(titleBarHeightSlider_, 1);
    titleBarRow->addWidget(titleBarHeightLabel_);
    winLayout->addLayout(titleBarRow);

    mainLayout->addWidget(winGroup);

    // Dialog buttons
    auto* buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(buttons);

    // Connections
    connect(colorButton_, &QPushButton::clicked, this, &SettingsDialog::chooseColor);
    connect(browseButton_, &QPushButton::clicked, this, &SettingsDialog::chooseImage);
    connect(colorRadio_, &QRadioButton::toggled, this, [this](bool checked) {
        colorButton_->setEnabled(checked);
    });
    connect(imageRadio_, &QRadioButton::toggled, this, [this](bool checked) {
        imagePath_->setEnabled(checked);
        browseButton_->setEnabled(checked);
    });
    connect(opacitySlider_, &QSlider::valueChanged, this, [this](int value) {
        opacityLabel_->setText(QString("%1%").arg(value));
    });
    connect(sizeSlider_, &QSlider::valueChanged, this, [this](int value) {
        sizeLabel_->setText(QString("%1%").arg(value));
    });
    connect(titleBarHeightSlider_, &QSlider::valueChanged, this, [this](int value) {
        titleBarHeightLabel_->setText(QString("%1px").arg(value));
    });
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::apply);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void SettingsDialog::loadFromConfig() {
    QString mode = config_.value("background/mode", "color").toString();
    selectedColor_ = QColor(config_.value("background/color", "#1a1a2a").toString());
    QString path = config_.value("background/imagePath", "").toString();

    if (mode == "image") {
        imageRadio_->setChecked(true);
    } else {
        colorRadio_->setChecked(true);
    }

    // Update color button preview
    colorButton_->setStyleSheet(
        QString("background-color: %1; border: 1px solid #4a4a6a; border-radius: 4px;")
            .arg(selectedColor_.name()));

    imagePath_->setText(path);

    int opacity = config_.value("background/opacity", 100).toInt();
    opacitySlider_->setValue(opacity);
    opacityLabel_->setText(QString("%1%").arg(opacity));

    int sizePercent = config_.value("window/sizePercent", 60).toInt();
    sizeSlider_->setValue(sizePercent);
    sizeLabel_->setText(QString("%1%").arg(sizePercent));

    int titleBarHeight = config_.value("window/titleBarHeight", 36).toInt();
    titleBarHeightSlider_->setValue(titleBarHeight);
    titleBarHeightLabel_->setText(QString("%1px").arg(titleBarHeight));
}

void SettingsDialog::apply() {
    if (colorRadio_->isChecked()) {
        config_.setValue("background/mode", "color");
    } else {
        config_.setValue("background/mode", "image");
    }
    config_.setValue("background/color", selectedColor_.name());
    config_.setValue("background/imagePath", imagePath_->text());
    config_.setValue("background/opacity", opacitySlider_->value());
    config_.setValue("window/sizePercent", sizeSlider_->value());
    config_.setValue("window/titleBarHeight", titleBarHeightSlider_->value());

    emit backgroundChanged();
    emit windowSizeChanged();
    emit titleBarHeightChanged();
    accept();
}

void SettingsDialog::chooseColor() {
    QColor color = QColorDialog::getColor(selectedColor_, this, "Choose Background Color");
    if (color.isValid()) {
        selectedColor_ = color;
        colorButton_->setStyleSheet(
            QString("background-color: %1; border: 1px solid gray;").arg(color.name()));
    }
}

void SettingsDialog::chooseImage() {
    QString path = QFileDialog::getOpenFileName(
        this, "Choose Background Image", QString(),
        "Images (*.png *.jpg *.jpeg *.bmp *.webp)");
    if (!path.isEmpty()) {
        imagePath_->setText(path);
        imageRadio_->setChecked(true);
    }
}

}  // namespace dashboard
