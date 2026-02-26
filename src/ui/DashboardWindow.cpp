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

#include "DashboardWindow.h"

#include "AddWidgetDialog.h"
#include "SettingsDialog.h"
#include "TitleBar.h"
#include "WidgetCanvas.h"
#include "WidgetFrame.h"
#include "core/ConfigStore.h"
#include "core/LayoutEngine.h"
#include "core/WidgetDataStore.h"
#include "core/WidgetManager.h"

#include <QCloseEvent>
#include <QFile>
#include <dashboard/IWidget.h>
#include <QGuiApplication>
#include <QMenu>
#include <QMenuBar>
#include <QScreen>
#include <QVBoxLayout>

namespace dashboard {

DashboardWindow::DashboardWindow(WidgetManager& widgetManager, ConfigStore& config,
                                 LayoutEngine& layoutEngine, QWidget* parent)
    : QMainWindow(parent),
      widgetManager_(widgetManager),
      config_(config),
      layoutEngine_(layoutEngine) {
    setupUi();
    restoreWindowGeometry();

    canvas_->applyBackground(config_);

    connect(canvas_, &WidgetCanvas::addWidgetRequested, this, &DashboardWindow::openAddWidget);
    connect(canvas_, &WidgetCanvas::widgetAdded, this, &DashboardWindow::onWidgetAdded);
    connect(canvas_, &WidgetCanvas::widgetRemoved, this, &DashboardWindow::onWidgetRemoved);
}

WidgetCanvas* DashboardWindow::canvas() const {
    return canvas_;
}

void DashboardWindow::restoreLayout() {
    QString layoutPath = LayoutEngine::layoutFilePath();
    bool fileExists = QFile::exists(layoutPath);

    if (fileExists) {
        layoutEngine_.loadFromFile(layoutPath);
    }

    if (!fileExists) {
        // First run only (no layout file at all): place all loaded plugins
        layoutReady_ = true;
        int offset = 20;
        for (auto* widget : widgetManager_.widgets()) {
            canvas_->addWidget(widget, QPoint(offset, offset));
            offset += 30;
        }
        return;
    }

    // Restore saved layout (may be empty if user deleted all widgets)
    for (const auto& layout : layoutEngine_.allLayouts()) {
        IWidget* plugin = widgetManager_.findByName(layout.pluginName);
        if (!plugin) {
            continue;
        }
        // Deserialize before createWidget so widgets can use state during construction
        QJsonObject data = WidgetDataStore::load(layout.instanceId);
        if (!data.isEmpty()) {
            plugin->deserialize(data);
        }
        auto* frame = canvas_->addWidget(plugin, layout.position);
        frame->setWidgetId(layout.instanceId);
        frame->resize(layout.size);
    }
    layoutReady_ = true;

    // Clamp all restored widget positions to the current canvas size.
    // Necessary when the saved layout was made on a different screen configuration.
    canvas_->clampFramePositions();
}

void DashboardWindow::closeEvent(QCloseEvent* event) {
    saveWindowGeometry();
    saveLayout();
    QMainWindow::closeEvent(event);
}

void DashboardWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    // After canvas clamps frame positions, sync them to the layout engine
    for (auto* frame : canvas_->frames()) {
        layoutEngine_.updatePosition(frame->widgetId(), frame->pos());
    }
    saveLayout();
}

void DashboardWindow::setupUi() {
    setWindowTitle("Dashboard");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    auto* app = qobject_cast<QGuiApplication*>(QGuiApplication::instance());
    connect(app, &QGuiApplication::screenAdded,   this, [this](QScreen*) { onScreensChanged(); });
    connect(app, &QGuiApplication::screenRemoved,  this, [this](QScreen*) { onScreensChanged(); });

    titleBar_ = new TitleBar(this);
    titleBar_->setTitle("Dashboard");
    applyTitleBarHeight();

    auto* menu = new QMenu(this);
    auto* addWidgetAction = menu->addAction("Add Widget...", this, &DashboardWindow::openAddWidget);
    auto* settingsAction  = menu->addAction("Settings...",   this, &DashboardWindow::openSettings);
    menu->addSeparator();
    auto* quitAction = menu->addAction("Quit", this, &QWidget::close);
    quitAction->setShortcut(QKeySequence::Quit);

    // Register actions on the window so keyboard shortcuts work without a menu bar
    addAction(addWidgetAction);
    addAction(settingsAction);
    addAction(quitAction);

    menuBar()->hide();
    titleBar_->setMenu(menu);

    canvas_ = new WidgetCanvas(this);

    auto* container = new QWidget(this);
    auto* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    vbox->addWidget(titleBar_);
    vbox->addWidget(canvas_, 1);
    setCentralWidget(container);
}



void DashboardWindow::applyWindowSize() {
    int percent = config_.value("window/sizePercent", 60).toInt();
    const QRect screen = QGuiApplication::primaryScreen()->availableGeometry();
    resize(screen.width() * percent / 100, screen.height() * percent / 100);
}

void DashboardWindow::saveWindowGeometry() {
    config_.setValue("window/geometry", saveGeometry());
}

void DashboardWindow::restoreWindowGeometry() {
    QByteArray geo = config_.value("window/geometry").toByteArray();
    if (!geo.isEmpty() && restoreGeometry(geo)) {
        // Verify the restored window intersects at least one available screen.
        // If not (e.g. the monitor it was on is gone), fall back to primary screen.
        const QRect windowRect = frameGeometry();
        bool onScreen = false;
        for (QScreen* screen : QGuiApplication::screens()) {
            if (screen->availableGeometry().intersects(windowRect)) {
                onScreen = true;
                break;
            }
        }
        if (!onScreen) {
            applyWindowSize();
            const QRect primary = QGuiApplication::primaryScreen()->availableGeometry();
            move(primary.center() - rect().center());
        }
    } else {
        // First launch — use percentage-based size, centered on primary screen.
        applyWindowSize();
        const QRect primary = QGuiApplication::primaryScreen()->availableGeometry();
        move(primary.center() - rect().center());
    }
}

void DashboardWindow::onScreensChanged() {
    // When screens are added or removed, ensure the window is still visible.
    const QList<QScreen*> screens = QGuiApplication::screens();
    if (screens.isEmpty()) return;

    const QRect windowRect = frameGeometry();
    bool onScreen = false;
    for (QScreen* screen : screens) {
        if (screen->availableGeometry().intersects(windowRect)) {
            onScreen = true;
            break;
        }
    }
    if (!onScreen) {
        const QRect primary = QGuiApplication::primaryScreen()->availableGeometry();
        move(primary.topLeft() + QPoint(40, 40));
    }

    // Re-clamp widget positions in case the canvas size changed.
    canvas_->clampFramePositions();
}

void DashboardWindow::applyTitleBarHeight() {
    int height = config_.value("window/titleBarHeight", 36).toInt();
    titleBar_->setFixedHeight(height);
}

void DashboardWindow::openSettings() {
    auto* dialog = new SettingsDialog(config_, this);
    connect(dialog, &SettingsDialog::backgroundChanged, this, [this]() {
        canvas_->applyBackground(config_);
    });
    connect(dialog, &SettingsDialog::windowSizeChanged, this, [this]() {
        applyWindowSize();
    });
    connect(dialog, &SettingsDialog::titleBarHeightChanged, this, [this]() {
        applyTitleBarHeight();
    });
    dialog->exec();
    dialog->deleteLater();
}

void DashboardWindow::openAddWidget() {
    auto* dialog = new AddWidgetDialog(widgetManager_, this);
    if (dialog->exec() == QDialog::Accepted) {
        int offset = 0;
        for (auto* widget : dialog->selectedWidgets()) {
            auto meta = widget->metadata();
            QPoint center = canvas_->centerPosition(meta.defaultSize);
            canvas_->addWidget(widget, QPoint(center.x() + offset, center.y() + offset));
            offset += 30;
        }
    }
    dialog->deleteLater();
}

void DashboardWindow::saveLayout() {
    if (!layoutReady_) {
        return;
    }
    // Persist each widget's own data to its dedicated file
    for (auto* frame : canvas_->frames()) {
        if (IWidget* w = frame->iwidget()) {
            QJsonObject data = w->serialize();
            if (!data.isEmpty()) {
                WidgetDataStore::save(frame->widgetId(), data);
            }
        }
    }
    layoutEngine_.saveToFile(LayoutEngine::layoutFilePath());
}

void DashboardWindow::onWidgetAdded(WidgetFrame* frame) {
    // Connect move/resize signals to persist layout changes
    connect(frame, &WidgetFrame::moved, this, [this, frame]() {
        onWidgetMoved(frame);
    });
    connect(frame, &WidgetFrame::resized, this, [this, frame]() {
        onWidgetResized(frame);
    });

    if (!layoutReady_) {
        // During restore, the layout engine already has the entries;
        // the instance ID is set by restoreLayout after this returns.
        return;
    }

    QString instanceId =
        layoutEngine_.addWidget(frame->pluginName(), frame->pos(), frame->size());
    frame->setWidgetId(instanceId);
    saveLayout();
}

void DashboardWindow::onWidgetMoved(WidgetFrame* frame) {
    layoutEngine_.updatePosition(frame->widgetId(), frame->pos());
    saveLayout();
}

void DashboardWindow::onWidgetResized(WidgetFrame* frame) {
    layoutEngine_.updateSize(frame->widgetId(), frame->size());
    saveLayout();
}

void DashboardWindow::onWidgetRemoved(const QString& instanceId) {
    layoutEngine_.removeWidget(instanceId);
    WidgetDataStore::remove(instanceId);
    saveLayout();
}

}  // namespace dashboard
