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

#include "icon_loader.h"

#include <QtDebug>
#include <QFile>

QIcon icon_loader::load(const QString &name) {
  QIcon ret;

  if (name.isEmpty())
    return ret;

#if QT_VERSION >= 0x040600
  // Try to load it from the theme initially
  ret = QIcon::fromTheme(name);
  if (!ret.isNull())
    return ret;
#endif

  // Otherwise use our fallback theme
  const QString filename(":/icons/" + name + ".png");
  // TODO: add different size icons 16x16, 22x22 etc..

  if (QFile::exists(filename))
    ret.addFile(filename);

  if (ret.isNull())
    qDebug() << "Couldn't load icon" << name;
  return ret;
} 
