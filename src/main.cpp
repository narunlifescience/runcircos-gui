/* This file is part of runcircos-gui.
   Copyright 2014 - 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   runcircos-gui is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License 2 as published by
   the Free Software Foundation. However GPL version 3, or any other GPL
   version are icompatible with this software.

   runcircos-gui is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with runcircos-gui.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QTimer>


int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // Show splashscreen
  QPixmap pixmap (":/splash.png");
  QSplashScreen *splash = new QSplashScreen(pixmap);
  splash->show();
  
  MainWindow w;

  // Close splashscreen after 3 sec & show mainwindow
  QTimer::singleShot(3000, splash, SLOT(close()));
  QTimer::singleShot(3000, &w, SLOT(show()));
  
  return a.exec();
}
