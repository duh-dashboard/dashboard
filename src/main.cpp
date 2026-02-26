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

#include "app/DashboardApp.h"

#include <QLockFile>
#include <QStandardPaths>

int main(int argc, char* argv[]) {
    // Single-instance guard
    QString lockPath =
        QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/dashboard.lock";
    QLockFile lockFile(lockPath);

    if (!lockFile.tryLock(100)) {
        qWarning("Another instance of Dashboard is already running.");
        return 1;
    }

    dashboard::DashboardApp app(argc, argv);
    return app.run();
}
