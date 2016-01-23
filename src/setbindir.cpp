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

#include "setbindir.h"
#include "ui_setbindir.h"

#include "iconloader.h"
#include "utils.h"

#include <QFileDialog>
#include <QSettings>

SetBinDir::SetBinDir(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::SetBinDir) {
  ui->setupUi(this);

  // Load icons
  setWindowIcon(IconLoader::load("folder"));
  ui->setcircosbindir_pushButton->setIcon(IconLoader::load(
      "document-open-folder"));

  // Signal slot connections
  connect(ui->setcircosbindir_pushButton, SIGNAL(clicked()),
          SLOT(browseBinDir()));
  connect(ui->ok_pushButton, SIGNAL(clicked()),
          SLOT(setBinDir()));
  connect(ui->cancel_pushButton, SIGNAL(clicked()),
          SLOT(close()));

  // Load settings
  settingsfile = Utils::getConfigPath(Utils::ConfFile);
  loadBinDirSettings();
}

SetBinDir::~SetBinDir() {
  delete ui;
}

void SetBinDir::browseBinDir() {
  QString set_circos_bindir = QFileDialog::getExistingDirectory(0,
      "Caption",QString(),QFileDialog::ShowDirsOnly);
  ui->setcircosbindir_plainTextEdit->setText(set_circos_bindir);
}

void SetBinDir::setBinDir() {
  QSettings settings(settingsfile, QSettings::NativeFormat);
  settings.setValue("CircosBinDirectory",
      ui->setcircosbindir_plainTextEdit->text().trimmed());
  close();
}

void SetBinDir::loadBinDirSettings() {
  QSettings settings(settingsfile, QSettings::NativeFormat);
  QString bindir = settings.value("CircosBinDirectory", "").toString();
  ui->setcircosbindir_plainTextEdit->setText(bindir);
}
