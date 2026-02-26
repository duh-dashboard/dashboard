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

#include <QMainWindow>

namespace dashboard {

class ConfigStore;
class LayoutEngine;
class TitleBar;
class WidgetCanvas;
class WidgetFrame;
class WidgetManager;

class DashboardWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit DashboardWindow(WidgetManager& widgetManager, ConfigStore& config,
                             LayoutEngine& layoutEngine, QWidget* parent = nullptr);

    WidgetCanvas* canvas() const;
    void restoreLayout();

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupUi();
    void openSettings();
    void openAddWidget();
    void saveLayout();

    void applyWindowSize();
    void applyTitleBarHeight();
    void saveWindowGeometry();
    void restoreWindowGeometry();
    void onScreensChanged();
    void onWidgetAdded(WidgetFrame* frame);
    void onWidgetRemoved(const QString& instanceId);
    void onWidgetMoved(WidgetFrame* frame);
    void onWidgetResized(WidgetFrame* frame);

    TitleBar* titleBar_;
    WidgetCanvas* canvas_;
    WidgetManager& widgetManager_;
    ConfigStore& config_;
    LayoutEngine& layoutEngine_;
    bool layoutReady_ = false;
};

}  // namespace dashboard
