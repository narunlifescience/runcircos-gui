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

#include "circosbindir_set.h"
#include "ui_circosbindir_set.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

circosbindir_set::circosbindir_set(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::circosbindir_set) {
  ui->setupUi(this);
  QFile inputFile(QDir::homePath() + "/.ncircos/circosbin.ncd");
  if (inputFile.open(QIODevice::ReadOnly)) {
    QTextStream in(&inputFile);
    QString line = in.readLine();
    ui->setcircosbindir_plainTextEdit->setText(line.trimmed());
    inputFile.close();
  }
}

circosbindir_set::~circosbindir_set() {
  delete ui;
}

void circosbindir_set::on_setcircosbindir_pushButton_clicked() {
  QString set_circos_bindir = QFileDialog::getExistingDirectory(0,
      "Caption",QString(),QFileDialog::ShowDirsOnly);
  ui->setcircosbindir_plainTextEdit->setText(set_circos_bindir);
}

void circosbindir_set::on_pushButton_clicked() {
  QDir dir(QDir::homePath() + "/.ncircos");
  if (!dir.exists()) {
    dir.mkpath(".");
  }
  QString filename = QDir::homePath() + "/.ncircos/circosbin.ncd";
  QFile file( filename );
  if ( file.open(QIODevice::ReadWrite| QIODevice::Truncate
                 | QIODevice::Text) ) {
    QTextStream stream( &file );
    stream << ui->setcircosbindir_plainTextEdit->text().trimmed();
  }
  close();
}

void circosbindir_set::on_cancel_pushButton_clicked() {
  close();
}

