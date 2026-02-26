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

#include <QColor>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QString>

namespace dashboard {

class ConfigStore;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(ConfigStore& config, QWidget* parent = nullptr);

signals:
    void backgroundChanged();
    void windowSizeChanged();
    void titleBarHeightChanged();

private:
    void setupUi();
    void loadFromConfig();
    void apply();

    void chooseColor();
    void chooseImage();

    ConfigStore& config_;

    QRadioButton* colorRadio_;
    QRadioButton* imageRadio_;
    QPushButton* colorButton_;
    QLineEdit* imagePath_;
    QPushButton* browseButton_;

    QColor selectedColor_;
    QSlider* opacitySlider_;
    QLabel* opacityLabel_;

    QSlider* sizeSlider_;
    QLabel* sizeLabel_;

    QSlider* titleBarHeightSlider_;
    QLabel* titleBarHeightLabel_;
};

}  // namespace dashboard
