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

#include "module_installer.h"
#include "ui_module_installer.h"

#include "icon_loader.h"

#include <QProcess>
#include <QFileDialog>
#include <QDesktopServices>
#include <QTextStream>
#include <QAction>
#include <QFileInfo>
#include <QMessageBox>
#include <QDesktopWidget>

module_installer::module_installer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::module_installer)
{
    ui->setupUi(this);

    // Load icons
    setWindowIcon(icon_loader::load("package-install"));

    // Signal slot connections
    connect(ui->install_pushButton, SIGNAL(clicked()),
            SLOT(install_perl_module()));
}

module_installer::~module_installer()
{
    delete ui;
}

void module_installer::install_perl_module()
{
        if (ui->module_lineEdit->text().trimmed() != "GD")
        {
        QDir dir(QDir::homePath() + "/.ncircos");
        if (!dir.exists()) { dir.mkpath("."); }
        QString filename = QDir::homePath() + "/.ncircos/installmodule.sh";
        QFile file( filename );
        QString first, second, final, pressenter1, pressenter2;
        first = "#!/bin/bash";
        second = "echo \"Installing " +ui->module_lineEdit->text().trimmed() + " module by CPAN method ....\"";
        final = "sudo cpan install " + ui->module_lineEdit->text().trimmed();
        pressenter1 = "echo -n \"Close xterm  or press [ENTER] to exit\"";
        pressenter2 = "read -n 1 -s";
        if ( file.open(QIODevice::ReadWrite| QIODevice::Truncate | QIODevice::Text) )
        {
            QTextStream stream( &file );
            stream << first << endl << second << endl << final << endl << pressenter1 << endl << pressenter2;
            file.close();
        }
        modinstall  = new QProcess(this);
        connect(modinstall, SIGNAL(finished(int)), this, SLOT(chmodExit()));
        modinstall->start("bash -c \"chmod a+x "+ filename + "\"");
        }
}

void module_installer::chmodExit()
{
  modinstall = new QProcess(this);
  modinstall->startDetached("bash -c \"xterm " + QDir::homePath() + "/.ncircos/installmodule.sh\"");
  this->close();
}
