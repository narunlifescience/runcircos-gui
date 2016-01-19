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

#ifndef CIRCOSBINDIR_SET_H
#define CIRCOSBINDIR_SET_H

#include <QDialog>

namespace Ui {
  class circosbindir_set;
}

class circosbindir_set : public QDialog {
  Q_OBJECT

 public:
  explicit circosbindir_set(QWidget *parent = 0);
  ~circosbindir_set();

 private slots:
  void on_setcircosbindir_pushButton_clicked();
  void on_pushButton_clicked();
  void on_cancel_pushButton_clicked();


 private:
  Ui::circosbindir_set *ui;
};

#endif // CIRCOSBINDIR_SET_H
