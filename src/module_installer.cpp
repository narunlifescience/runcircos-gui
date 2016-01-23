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

#include "iconloader.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>

module_installer::module_installer(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::module_installer) {
  ui->setupUi(this);

  // Load icons
  setWindowIcon(IconLoader::load("package-install"));

  // Signal slot connections
  connect(ui->install_pushButton, SIGNAL(clicked()),
      SLOT(install_perl_module()));
}

module_installer::~module_installer() {
  delete ui;
}

void module_installer::install_perl_module() {
#ifdef Q_OS_WIN
  if (ui->module_lineEdit->text().trimmed().count() != 0) {
#else
  // NOTE: GD module cannot be installed from runcircos-gui for
  // linux at this point. Each distribution provides their own
  // precompiled custom library. may be in future this can be
  // added.
  if (ui->module_lineEdit->text().trimmed() != "GD" &&
      ui->module_lineEdit->text().trimmed().count() != 0) {

    QDir dir(QDir::homePath() + "/.runcircos-gui");
    if (!dir.exists()) {
      dir.mkpath(".");
    }

    // Create a perl module installer bash script file.
    const QString filename = QDir::homePath() +
        "/.runcircos-gui/installmodule.sh";
    QFile file(filename);
    QStringList bash_script;
    bash_script << "#!/bin/bash"
                << "echo \"Installing " +
                   ui->module_lineEdit->text().trimmed() +
                   " module by CPAN method ....\""
                << "sudo cpan install " +
                   ui->module_lineEdit->text().trimmed();
#ifdef Q_OS_UNIX
    QStringList linux_bash_script_end;
    linux_bash_script_end << "echo -n \"Close xterm or "
                             "press [ENTER] to exit\""
                          <<"read -n 1 -s";
#endif
    if (file.open(QIODevice::ReadWrite| QIODevice::Truncate
                   | QIODevice::Text)) {
      QTextStream stream(&file);
      for (int i = 0; i < bash_script.size(); ++i) {
        stream << bash_script.at(i) << endl;
      }
#ifdef Q_OS_UNIX
      for (int i = 0; i < linux_bash_script_end.size(); ++i) {
        stream << linux_bash_script_end.at(i) << endl;
      }
#endif
      file.close();
    }

    // Call external process first to change execution
    // mode of bash script & then run the script.
    modinstall  = new QProcess(this);
    connect(modinstall, SIGNAL(finished(int)), this,
            SLOT(start_install_module_process()));
    modinstall->start("bash -c \"chmod a+x "+ filename + "\"");
#endif
  } else {
    if (ui->module_lineEdit->text().trimmed().count() == 0) {
      show_blank_module_name_msgbox();
    } else if (ui->module_lineEdit->text().trimmed() == "GD") {
      // Q_OS_WIN will never reach this if statement.
#ifdef Q_OS_UNIX
      QMessageBox msgbox;
      msgbox.setWindowTitle(tr("No GD!"));
      msgbox.setText(tr("GD module cannot be installed!"));
      msgbox.setInformativeText(tr("You cannot install GD perl module "
                                "using runcircos-gui. Please install "
                                "precompiled GD module from software "
                                "repository of your respective linux "
                                "distribution."));
      msgbox.setStandardButtons(QMessageBox::Cancel);
      msgbox.exec();
#elif defined(Q_OS_DARWIN)
      // Generate a custom script to install GD.
      QDir dir(QDir::homePath() + "/.runcircos-gui");
      if (!dir.exists()) {
        dir.mkpath(".");
      }
      const QString filename = QDir::homePath() +
                               "/.runcircos-gui/installmodule.sh";
      QFile file(filename);
      QStringList bash_script;
      bash_script
          << "#!/bin/sh"
          << "echo \"Installing GD module by custom method ....\"";
          << "cd " + QDir::homePath() + "/.runcircos-gui";
          << "curl -O http://www.researcharun.com/narayanankutty/gd.tar.gz";
          << "tar -zxf gd.tar.gz";
          << "cd gd";
          << "echo \"><><><><><><><><><\"";
          << "echo \"Installing zlib\"";
          << "echo \"><><><><><><><><><\"";
          << "tar -zxf zlib-1.2.8.tar.xz";
          << "cd zlib-1.2.8";
          << "./configure";
          << "make";
          << "sudo make install";
          << "cd ..";
          << "echo \"Installing libpng\"";
          << "tar -zxf libpng-1.6.6.tar.xz";
          << "cd libpng-1.6.6";
          << "echo \"Installing libjpeg\"";
          << "tar -zxf jpegsrc.v9.tar.gz";
          << "cd jpeg-9";
          << "echo \"Installing freetype\"";
          << "tar -zxf freetype-2.5.0.1.tar.bz2";
          << "cd freetype-2.5.0.1";
          << "echo \"Installing libgd\"";
          << "tar -zxf libgd-2.1.0.tar.xz";
          << "cd libgd-2.1.0";
          << "./configure --with-zlib=/usr/local --with-jpeg=/usr/local "
             "--with-png=/usr/local --with-freetype=/usr/local";
          << "echo \"Installing GD\"";
          << "tar -zxf GD-2.50.tar.gz";
          << "cd GD-2.50";
          << "perl Makefile.PL";
          << "rm -rf zlib-1.2.8 libpng-1.6.6 jpeg-9 freetype-2.5.0.1 "
             "libgd-2.1.0 GD-2.50";
          << "echo \"Check GD Installation Status\"";
          << "echo \"GD\" | xargs -I MODULE perl -e  'print eval \"use "
             "MODULE;1\"?\"\\n\\n\\nOK\\n\\n\\n\":\"\\n\\n\\nFail\\n\\n\\n\"'";

      if (file.open(QIODevice::ReadWrite| QIODevice::Truncate |
                    QIODevice::Text)) {
        QTextStream stream( &file );
        for (int i = 0; i < bash_script.size(); ++i) {
          stream << bash_script.at(i) << endl;
        }
        file.close();
      }

      modinstall  = new QProcess(this);
      connect(modinstall, SIGNAL(finished(int)), this,
              SLOT(start_install_module_process()));
      modinstall->start("bash -c \"chmod a+x "+ filename + "\"");
#endif
    }
  }
}

void module_installer::start_install_module_process() {
  // Start the terminal as detached process
  modinstall = new QProcess(this);
#ifdef Q_OS_UNIX
  // TODO: use the default terminal available
  modinstall->startDetached("bash -c \"xterm " + QDir::homePath() +
                            "/.runcircos-gui/installmodule.sh\"");
#elif defined(Q_OS_DARWIN)
  modinstall->startDetached("open -a Terminal " + QDir::homePath() +
                            "/.runcircos-gui/installmodule.sh");
#elif defined(Q_OS_WIN)
  modinstall->startDetached("cmd.exe /K cpan " +
                            ui->module_lineEdit->text().trimmed());
#endif
  this->close();
}

void module_installer::show_blank_module_name_msgbox() {
  QMessageBox msgbox;
  msgbox.setWindowTitle(tr("Blank module name!"));
  msgbox.setText("Please give a perl module name!");
  msgbox.setInformativeText(tr("You cannot leave perl module "
                            "name as blank. Please give a valid "
                            "module name."));
  msgbox.setWindowModality(Qt::NonModal);
  msgbox.exec();
}
