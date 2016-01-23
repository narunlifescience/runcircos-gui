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
#include "ui_mainwindow.h"

#include "aboutbox.h"
#include "setbindir.h"
#include "iconloader.h"
#include "module_installer.h"
#include "utils.h"

#include <QAction>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>
#include <QtCore>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      image_show_status(false) {
  // start mainwindow
  ui->setupUi(this);

  //  Load icons
  setWindowIcon(IconLoader::load("runcircos-gui"));
  ui->circos_bindir_pushButton->setIcon(IconLoader::load(
      "document-open-folder"));
  ui->out_folder_pushButton->setIcon(IconLoader::load(
      "document-open-folder"));
  ui->conf_file_pushButton->setIcon(IconLoader::load(
      "document-open"));
  ui->run_ncircos_button->setIcon(IconLoader::load(
      "media-playback-start"));
  ui->Save_stdout_pushButton->setIcon(IconLoader::load(
      "document-save"));
  ui->stop_pushButton->setIcon(IconLoader::load("stop"));
  ui->actionNew->setIcon(IconLoader::load("document-new"));
  ui->actionOpen_settings->setIcon(IconLoader::load(
      "document-open"));
  ui->actionSave_settings->setIcon(IconLoader::load(
      "document-save"));
  ui->actionEdit_conf_file->setIcon(IconLoader::load(
      "configure"));
  ui->actionEdit_other_files->setIcon(IconLoader::load(
      "document-edit"));
  ui->actionClear_exec_status->setIcon(IconLoader::load(
      "edit-clear"));
  ui->actionQuit->setIcon(IconLoader::load(
      "application-exit"));
  ui->actionCheck_version->setIcon(IconLoader::load(
      "application-x-shellscript"));
  ui->actionInstall_perl_package->setIcon(IconLoader::load(
      "package-install"));
  ui->actionCommand_line_CMD->setIcon(IconLoader::load(
      "terminal"));
  ui->actionSet_circos_bin_directory->setIcon(IconLoader::load(
      "folder"));
  ui->actionOnline_documentation->setIcon(IconLoader::load(
      "help-contents"));
  ui->actionQuick_referance_manuel_QRM->setIcon(IconLoader::load(
      "application-pdf"));
  ui->actionAbout->setIcon(IconLoader::load("help-about"));

  // Signal slot connections

  connect(ui->stop_pushButton, SIGNAL(clicked()),
          SLOT(stopExecution()));
  connect(ui->actionInstall_perl_package, SIGNAL(triggered()),
          SLOT(installPerlModule()));
  connect(ui->actionQuit, SIGNAL(triggered()),
          SLOT(exit()));
  connect(ui->actionSet_circos_bin_directory, SIGNAL(triggered()),
          SLOT(setBinDir()));
  connect(ui->actionOnline_documentation, SIGNAL(triggered()),
          SLOT(onlineDocumentation()));
  connect(ui->actionQuick_referance_manuel_QRM, SIGNAL(triggered()),
          SLOT(quickReferanceManuel()));
  connect(ui->actionAbout, SIGNAL(triggered()),
          SLOT(about()));

  QRect desktopRect = QApplication::desktop()->availableGeometry(this);
  QPoint center = desktopRect.center();
  this->move((center.x() - (width()/2)), (center.y() - (height()/2)) );
  circosbindir = "[circos/bin_dir...]";
  conffile = "[.conf_file...]";
  ui->nt_default_radioButton->setChecked(true);
  ui->t_default_radioButton->setChecked(true);
  ui->nto_default_groupBox->setStyleSheet("QGroupBox {border : 0;}");
  ui->nto_png_nopng_groupBox->setStyleSheet("QGroupBox {border : 0;}");
  ui->nto_svg_nosvg_groupBox->setStyleSheet("QGroupBox {border : 0;}");
  ui->nto_warnings_nowarnings_groupBox->setStyleSheet("QGroupBox {border : 0;}");
  ui->nto_paranoid_noparanoid_groupBox->setStyleSheet("QGroupBox {border : 0;}");
  ui->nto_showticks_noshowticks_groupBox->setStyleSheet("QGroupBox {border : 0;}");
  ui->nto_showticklabels_noshowticklabels_groupBox->setStyleSheet("QGroupBox {border : 0;}");
  ui->mandatory_label->setStyleSheet("QLabel { color : red; }");
  ui->mandatory_circosbindir_label->setStyleSheet("QLabel { color : red; }");
  ui->mandatory_conffile_label->setStyleSheet("QLabel { color : red; }");
  ui->exec_status_plainTextEdit->setStyleSheet("QPlainTextEdit { background-color : lightblack; color : green; border-radius: 5px; }");
  ui->exec_status_err_textEdit->setStyleSheet("QPlainTextEdit { background-color : lightblack; color : red; border-radius: 5px; }");
  ui->exec_status_plainTextEdit->setReadOnly(true);
  ui->exec_status_err_textEdit->setReadOnly(true);
  ui->cmdfinal_textEdit->setReadOnly(true);
  ui->cmdfinal_textEdit->setStyleSheet("QTextEdit { background-color : lightgrey;}");
  ui->ext_op_args_plainTextEdit->setReadOnly(true);
  ui->ext_op_args_plainTextEdit->setStyleSheet("QLineEdit { background-color : lightgrey;}");
  setcircosbindirdefault ();
  if (ui->circos_bindir_plainTextEdit->text().trimmed().count() == 0) {
     ui->circos_bindir_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid red; border-radius: 2px; }");
  }
  if (ui->conf_file_plainTextEdit->text().trimmed().count() == 0) {
    ui->conf_file_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid red; border-radius: 2px; }");
  }


  QStringList list=(QStringList()<<"png"<<"svg");
  ui->comboBox->addItems(list);
  ui->comboBox->setVisible(false);
  enablectrl_runtime();
  update_cmdfinal();
}

//close main window
MainWindow::~MainWindow() {
  delete ui;
}



// run Circos with given arguments
void MainWindow::on_run_ncircos_button_clicked()
{
    int del;
    del = delete_existingfiles ();
    if (del == 1 || del == 2)
    {
    disablectrl_runtime();
    ui->progressBar->setMaximum(0);
    ui->exec_status_plainTextEdit->setPlainText("");
    ui->exec_status_err_textEdit->setPlainText("");
    extopargs = ui->ext_op_args_plainTextEdit->text();
    process  = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(updateErr()));
    connect(process, SIGNAL(finished(int)), this, SLOT(updateExit()));
    cmdfinal = ui->cmdfinal_textEdit->toPlainText().trimmed();
    process->start("bash -c \""+ cmdfinal + "\"");
    }
}

void MainWindow::updateText()
{
    QString appendText(process->readAllStandardOutput());
    ui->exec_status_plainTextEdit->appendPlainText(appendText);
}

void MainWindow::updateExit()
{
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    enablectrl_runtime();
    if(ui->t_version_radioButton->isChecked() || ui->t_help_radioButton->isChecked() || ui->t_man_radioButton->isChecked()) {disablehelpmanversion();}
    if (image_show_status == true && !ui->t_help_radioButton->isChecked() && !ui->t_version_radioButton->isChecked() && !ui->t_man_radioButton->isChecked() && !ui->t_cdump_radioButton->isChecked())
    {
        if(!ui->exec_status_err_textEdit->toPlainText().contains("*****!!! PROCESS ABORTED BY USER !!!*****")) { viewimage (); }
    }
}

void MainWindow::updateErr()
{
    QString appendErr(process->readAllStandardError());
    ui->exec_status_err_textEdit->appendPlainText(appendErr);
}

// setting up circos/bin directory
void MainWindow::on_circos_bindir_pushButton_clicked()
{
    QString circos_bindir = QFileDialog::getExistingDirectory(0,"Caption",QString(),QFileDialog::ShowDirsOnly);
    ui->circos_bindir_plainTextEdit->setText(circos_bindir);
}

// setting up the configuration file
void MainWindow::on_conf_file_pushButton_clicked()
{
    QString conf_fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/path/to/file/",tr("Configuration Files (*.conf)"));
    ui->conf_file_plainTextEdit->setText(conf_fileName);
}

// setting up the circos output directory
void MainWindow::on_out_folder_pushButton_clicked()
{
    QString output_dir = QFileDialog::getExistingDirectory(0,"Caption",QString(),QFileDialog::ShowDirsOnly);
    ui->out_folder_plainTextEdit->setText(output_dir);
}

// enabling and disabling extended optional arguments editing
void MainWindow::on_ext_op_args_enable_checkBox_clicked()
{
    if (ui->ext_op_args_enable_checkBox->isChecked())
    {
        ui->ext_op_args_plainTextEdit->setReadOnly(false);
        ui->ext_op_args_plainTextEdit->setStyleSheet("QLinetEdit { background-color : white;}");

    } else
    {
        ui->ext_op_args_plainTextEdit->setReadOnly(true);
        ui->ext_op_args_plainTextEdit->setStyleSheet("QLineEdit { background-color : lightgrey;}");
    }
}


// enabling and disabling commandline final editing
void MainWindow::on_edit_cmdline_checkBox_clicked()
{
    if(ui->edit_cmdline_checkBox->isChecked())
    {
        ui->cmdfinal_textEdit->setReadOnly(false);
        ui->cmdfinal_textEdit->setStyleSheet("QTextEdit { background-color : white;}");
    } else
    {
        ui->cmdfinal_textEdit->setReadOnly(true);
        ui->cmdfinal_textEdit->setStyleSheet("QTextEdit { background-color : lightgrey;}");
    }
}

// exclusive radiobutton checking and unchecking negatable toggle options (default button click)
void MainWindow::on_nt_default_radioButton_clicked()
{

    nto_png = "";
    nto_svg = "";
    nto_warnings = "";
    nto_paranoid = "";
    nto_showticks = "";
    nto_showticklabels = "";

    ui->nt_default_radioButton->setChecked(true);

    ui->nt_png_radioButton->setAutoExclusive(false);
    ui->nt_nopng_radioButton->setAutoExclusive(false);
    ui->nt_svg_radioButton->setAutoExclusive(false);
    ui->nt_nosvg_radioButton->setAutoExclusive(false);
    ui->nt_warnings_radioButton->setAutoExclusive(false);
    ui->nt_nowarnings_radioButton->setAutoExclusive(false);
    ui->nt_paranoid_radioButton->setAutoExclusive(false);
    ui->nt_noparanoid_radioButton->setAutoExclusive(false);
    ui->nt_showticks_radioButton->setAutoExclusive(false);
    ui->nt_noshowticks_radioButton->setAutoExclusive(false);
    ui->nt_showticklabels_radioButton->setAutoExclusive(false);
    ui->nt_noshowticklabels_radioButton->setAutoExclusive(false);

    ui->nt_png_radioButton->setChecked(false);
    ui->nt_nopng_radioButton->setChecked(false);
    ui->nt_svg_radioButton->setChecked(false);
    ui->nt_nosvg_radioButton->setChecked(false);
    ui->nt_warnings_radioButton->setChecked(false);
    ui->nt_nowarnings_radioButton->setChecked(false);
    ui->nt_paranoid_radioButton->setChecked(false);
    ui->nt_noparanoid_radioButton->setChecked(false);
    ui->nt_showticks_radioButton->setChecked(false);
    ui->nt_noshowticks_radioButton->setChecked(false);
    ui->nt_showticklabels_radioButton->setChecked(false);
    ui->nt_noshowticklabels_radioButton->setChecked(false);

    ui->nt_png_radioButton->setAutoExclusive(true);
    ui->nt_nopng_radioButton->setAutoExclusive(true);
    ui->nt_svg_radioButton->setAutoExclusive(true);
    ui->nt_nosvg_radioButton->setAutoExclusive(true);
    ui->nt_warnings_radioButton->setAutoExclusive(true);
    ui->nt_nowarnings_radioButton->setAutoExclusive(true);
    ui->nt_paranoid_radioButton->setAutoExclusive(true);
    ui->nt_noparanoid_radioButton->setAutoExclusive(true);
    ui->nt_showticks_radioButton->setAutoExclusive(true);
    ui->nt_noshowticks_radioButton->setAutoExclusive(true);
    ui->nt_showticklabels_radioButton->setAutoExclusive(true);
    ui->nt_noshowticklabels_radioButton->setAutoExclusive(true);
    update_cmdfinal();
}

void MainWindow::on_nt_png_radioButton_clicked()
{
    nto_png = " -png";
    if ( ui->nt_default_radioButton->isChecked()) { ui->nt_default_radioButton->setChecked(false); }
    update_cmdfinal();
}

void MainWindow::on_nt_nopng_radioButton_clicked()
{
    nto_png = " -nopng";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_svg_radioButton_clicked()
{
    nto_svg = " -svg";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_nosvg_radioButton_clicked()
{
    nto_svg = " -nosvg";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_warnings_radioButton_clicked()
{
    nto_warnings = " -warnings";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_nowarnings_radioButton_clicked()
{
    nto_warnings = " -nowarnings";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_paranoid_radioButton_clicked()
{
    nto_paranoid = " -paranoid";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_noparanoid_radioButton_clicked()
{
    nto_paranoid = " -noparanoid";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_showticks_radioButton_clicked()
{
    nto_showticks = " -show_ticks";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_noshowticks_radioButton_clicked()
{
    nto_showticks = " -noshow_ticks";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_showticklabels_radioButton_clicked()
{
    nto_showticklabels = " -show_tick_labels";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}

void MainWindow::on_nt_noshowticklabels_radioButton_clicked()
{
    nto_showticklabels = " -noshow_tick_labels";
    if ( ui->nt_default_radioButton->isChecked())
    {
        ui->nt_default_radioButton->setChecked(false);
    }
    update_cmdfinal();
}


void MainWindow::on_t_default_radioButton_clicked()
{
    to = "";
    ui->t_default_radioButton->setChecked(true);
    update_cmdfinal();
    enablehelpmanversion();
}

void MainWindow::on_t_image_map_use_radioButton_clicked()
{
    to = " -param image_map_use";
    update_cmdfinal();
    enablehelpmanversion();
}

void MainWindow::on_t_silent_radioButton_clicked()
{
    to = " -silent";
    update_cmdfinal();
    enablehelpmanversion();
}

void MainWindow::on_t_cdump_radioButton_clicked()
{
    to = " -cdump";
    update_cmdfinal();
    enablehelpmanversion();
}

void MainWindow::on_t_debug_radioButton_clicked()
{
    to = " -debug";
    update_cmdfinal();
    enablehelpmanversion();
}

void MainWindow::on_t_help_radioButton_clicked()
{
    to = " -help";
    update_cmdfinal();
    disablehelpmanversion();
}

void MainWindow::on_t_man_radioButton_clicked()
{
    to = " -man";
    update_cmdfinal();
    disablehelpmanversion();
}

void MainWindow::on_t_version_radioButton_clicked()
{
    to = " -version";
    update_cmdfinal();
    disablehelpmanversion();
}

void MainWindow::update_cmdfinal ()
{
    QString tmp = ui->ext_op_args_plainTextEdit->text().trimmed();
    if(tmp.count() != 0)
    {
        extopargs = " " + extopargs.trimmed();
    }
    if(ui->t_version_radioButton->isChecked() || ui->t_help_radioButton->isChecked() || ui->t_man_radioButton->isChecked())
    {
        ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
        ui->cmdfinal_textEdit->setText("perl ");
        ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
        if(circosbindir.contains("[circos/bin_dir...]")) {ui->cmdfinal_textEdit->setTextColor( QColor( "red" ) );} else {ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );}
        ui->cmdfinal_textEdit->insertPlainText(circosbindir);
        ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
        ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
        ui->cmdfinal_textEdit->insertPlainText("/circos ");
        ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
        ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
        ui->cmdfinal_textEdit->insertPlainText(to);
    } else
    {
    ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
    ui->cmdfinal_textEdit->setText("perl ");
    ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
    if(circosbindir.contains("[circos/bin_dir...]")) {ui->cmdfinal_textEdit->setTextColor( QColor( "red" ) );} else {ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );}
    ui->cmdfinal_textEdit->insertPlainText(circosbindir);
    ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
    ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
    ui->cmdfinal_textEdit->insertPlainText("/circos -conf ");
    ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
    if(conffile.contains("[.conf_file...]")) {ui->cmdfinal_textEdit->setTextColor( QColor( "red" ) );} else {ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );}
    ui->cmdfinal_textEdit->insertPlainText(conffile);
    ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
    ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
    ui->cmdfinal_textEdit->insertPlainText(outputdir + outfile + to + nto_png + nto_svg + nto_warnings + nto_paranoid + nto_showticks + nto_showticklabels + extopargs);
    }
    ui->progressBar->setValue(0);
    ui->exec_status_plainTextEdit->setPlainText("");
    ui->exec_status_err_textEdit->setPlainText("");
}

void MainWindow::on_circos_bindir_plainTextEdit_textChanged()
{
    if(ui->circos_bindir_plainTextEdit->text().trimmed().contains(" ") == false)
    {
    circosbindir = ui->circos_bindir_plainTextEdit->text();
    if (ui->circos_bindir_plainTextEdit->text().trimmed().count() == 0)
    {
        ui->circos_bindir_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid red; border-radius: 2px; }");
        circosbindir = "[circos/bin_dir...]";
    } else
    {
        ui->circos_bindir_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid green; border-radius: 2px; }");
    }
    update_cmdfinal();
    } else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("No space allowed!");
        msgBox.setText("'"+ ui->circos_bindir_plainTextEdit->text().trimmed()+ "' "+ "contains blank space. This is not allowed ! Please give a correct circos/bin path.");
        msgBox.exec();
        ui->circos_bindir_plainTextEdit->setText("");
    }
}



void MainWindow::on_conf_file_plainTextEdit_textChanged()
{
    if(ui->conf_file_plainTextEdit->text().trimmed().contains(" ") == false)
    {
    conffile = ui->conf_file_plainTextEdit->text();
    if (ui->conf_file_plainTextEdit->text().trimmed().count() == 0)
    {
        ui->conf_file_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid red; border-radius: 2px; }");
        conffile = "[.conf_file...]";
    } else
    {
        ui->conf_file_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid green; border-radius: 2px; }");
    }
    update_cmdfinal();
    } else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("No space allowed!");
        msgBox.setText("'"+ ui->conf_file_plainTextEdit->text().trimmed()+ "' "+ "contains blank space. This is not allowed ! Please give a new .conf file path.");
        msgBox.exec();
        ui->conf_file_plainTextEdit->setText("");
    }
}

void MainWindow::on_out_folder_plainTextEdit_textChanged()
{
    if(ui->out_folder_plainTextEdit->text().trimmed().contains(" ") == false)
    {
        if(ui->out_folder_plainTextEdit->text().trimmed().count() != 0)
        {
        outputdir = " -outputdir "+ ui->out_folder_plainTextEdit->text().trimmed();
        update_cmdfinal();
        } else
        {
        outputdir = "";
        update_cmdfinal();
        }
    } else
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("No space allowed!");
        msgBox.setText("'"+ ui->out_folder_plainTextEdit->text().trimmed()+ "' "+ "contains blank space. This is not allowed ! Please give a new output directory path.");
        msgBox.exec();
        ui->out_folder_plainTextEdit->setText("");
    }
}

void MainWindow::on_out_file_plainTextEdit_textChanged()
{
    if(ui->out_file_plainTextEdit->text().trimmed().contains(" ") == false)
    {
    if(ui->out_file_plainTextEdit->text().trimmed().count() != 0)
    {

        outfile = " -outputfile "+ ui->out_file_plainTextEdit->text().trimmed();
        update_cmdfinal();
    } else
    {
       outfile = "";
       update_cmdfinal();
    }
    } else
    {
        QMessageBox Msgbox;
        Msgbox.setWindowTitle("No space allowed!");
         Msgbox.setText("'"+ ui->out_file_plainTextEdit->text().trimmed()+ "' "+ "contains blank space. This is not allowed ! Please give a new filename.");
         Msgbox.exec();
         ui->out_file_plainTextEdit->setText("");
    }
}

void MainWindow::on_ext_op_args_plainTextEdit_textChanged()
{
    extopargs = ui->ext_op_args_plainTextEdit->text().trimmed();
    update_cmdfinal();
}

void MainWindow::on_actionCheck_version_triggered()
{
    ui->exec_status_plainTextEdit->setPlainText("");
    ui->exec_status_err_textEdit->setPlainText("");
    process  = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(updateErr()));
    connect(process, SIGNAL(finished(int)), this, SLOT(updateExitperlcheck()));
    QString perlw = "which perl";
    process->start("bash -c \""+ perlw + "\"");
    ui->exec_status_plainTextEdit->appendPlainText("><><><><><><><><>< PERL BIN DIRECTORY (which perl) ><><><><><><><><><");
}
void MainWindow::updateExitperlcheck()
{
    process  = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(updateErr()));
    connect(process, SIGNAL(finished(int)), this, SLOT(updateExitperlcheckcomplete()));
    QString perlv = "perl -v";
    process->start("bash -c \""+ perlv + "\"");
    ui->exec_status_plainTextEdit->appendPlainText("><><><><><><><><>< PERL VERSION (perl -v) ><><><><><><><><><");
}

void MainWindow::updateExitperlcheckcomplete()
{
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    enablectrl_runtime();
}

void MainWindow::on_actionNew_triggered()
{
    on_nt_default_radioButton_clicked();
    on_t_default_radioButton_clicked();
    ui->ext_op_args_enable_checkBox->setChecked(false);
    ui->ext_op_args_plainTextEdit->setText("");
    on_ext_op_args_enable_checkBox_clicked();
    ui->circos_bindir_plainTextEdit->setText("");
    ui->conf_file_plainTextEdit->setText("");
    ui->out_folder_plainTextEdit->setText("");
    ui->out_file_plainTextEdit->setText("");
    ui->edit_cmdline_checkBox->setChecked(false);
    ui->exec_status_plainTextEdit->setPlainText("");
    ui->exec_status_err_textEdit->setPlainText("");
    on_edit_cmdline_checkBox_clicked();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    ui->imageciew_checkBox->setChecked(false);
    ui->comboBox->setCurrentIndex(0);
    ui->comboBox->setVisible(false);
    setcircosbindirdefault ();
}

void MainWindow::on_actionClear_exec_status_triggered()
{
    ui->exec_status_plainTextEdit->setPlainText("");
    ui->exec_status_err_textEdit->setPlainText("");
}

void MainWindow::on_Save_stdout_pushButton_clicked()
{
    QString output_fileName = QFileDialog::getSaveFileName(this, "Save file", "", "text files(*.txt)");
    if (output_fileName.contains(".txt") == false)
    {output_fileName = output_fileName + ".txt";}
    QFile file( output_fileName );
    if ( file.open(QIODevice::ReadWrite| QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        stream <<"#########################*** COMMANDLINE ***##########################"<< endl << ui->cmdfinal_textEdit->toPlainText() << endl << endl << "#######################*** STANDARD OUTPUT ***########################"<< endl<< ui->exec_status_plainTextEdit->toPlainText() << endl << "#######################!!! STANDARD ERROR !!!#########################" << endl << ui->exec_status_err_textEdit->toPlainText();
        file.close();
    }
}

void MainWindow::on_actionEdit_conf_file_triggered()
{
    QString conf_fileName_edit = QFileDialog::getOpenFileName(this, tr("Open File"),"/path/to/file/",tr("Configuration Files (*.conf)"));
    QDesktopServices::openUrl(QUrl(conf_fileName_edit));
}

void MainWindow::on_actionEdit_other_files_triggered()
{
    QString other_fileName_edit = QFileDialog::getOpenFileName(this, tr("Open File"),"/path/to/file/",tr("All Files (*.*)"));
    QDesktopServices::openUrl(QUrl(other_fileName_edit));
}

void MainWindow::viewimage()
{
    QString s, ext, cmdfinaltest, ofile, odir, oconf;
    cmdfinaltest = ui->cmdfinal_textEdit->toPlainText().trimmed();
    QStringList query = cmdfinaltest.split(" ");
    for (int i=0; i < query.size(); i++)
    {
        if (query.at(i).contains("-conf"))
        {
            oconf = query.at(i+1).trimmed();
        }
        if (query.at(i).contains("-outputdir"))
        {
            odir = query.at(i+1).trimmed();
        }
        if (query.at(i).contains("-outputfile"))
        {
            ofile = query.at(i+1).trimmed();
        }
    }
    ext = ui->comboBox->currentText().trimmed();
    QFileInfo info1(oconf);
    if (odir.trimmed() == "") { odir = info1.absolutePath();}
    if (ofile.trimmed() == "") { ofile = "circos";}
    QFile imagefile(odir + "/" + ofile + "." + ext);
    if(!imagefile.exists()) { QDesktopServices::openUrl(QUrl(QCoreApplication::applicationDirPath() + "/ncirco_no_image.png")); } else { QDesktopServices::openUrl(QUrl(odir + "/" + ofile + "." + ext)); }
}

void MainWindow::disablectrl_runtime()
{
    ui->toggle_groupBox->setEnabled(false);
     ui->ntoggle_groupBox->setEnabled(false);
      ui->ext_op_args_groupBox->setEnabled(false);
      ui->circos_bindir_plainTextEdit->setEnabled(false);
      ui->circos_bindir_pushButton->setEnabled(false);
      ui->conf_file_plainTextEdit->setEnabled(false);
      ui->conf_file_pushButton->setEnabled(false);
      ui->out_folder_plainTextEdit->setEnabled(false);
      ui->out_folder_pushButton->setEnabled(false);
      ui->out_file_plainTextEdit->setEnabled(false);
      ui->edit_cmdline_checkBox->setEnabled(false);
      ui->cmdfinal_textEdit->setEnabled(false);
      ui->run_ncircos_button->setEnabled(false);
      ui->Save_stdout_pushButton->setEnabled(false);
      ui->menuBar->setEnabled(false);
      ui->comboBox->setEnabled(false);
      ui->imageciew_checkBox->setEnabled(false);
      ui->circos_bindir_label->setEnabled(false);
      ui->conf_file_label->setEnabled(false);
      ui->out_folder_label->setEnabled(false);
      ui->out_file_label->setEnabled(false);
      ui->cmdline_label->setEnabled(false);
      ui->mandatory_circosbindir_label->setEnabled(false);
      ui->mandatory_conffile_label->setEnabled(false);
      ui->mandatory_label->setEnabled(false);
      ui->stop_pushButton->setEnabled(true);
}

void MainWindow::enablectrl_runtime()
{
    ui->toggle_groupBox->setEnabled(true);
     ui->ntoggle_groupBox->setEnabled(true);
      ui->ext_op_args_groupBox->setEnabled(true);
      ui->circos_bindir_plainTextEdit->setEnabled(true);
      ui->circos_bindir_pushButton->setEnabled(true);
      ui->conf_file_plainTextEdit->setEnabled(true);
      ui->conf_file_pushButton->setEnabled(true);
      ui->out_folder_plainTextEdit->setEnabled(true);
      ui->out_folder_pushButton->setEnabled(true);
      ui->out_file_plainTextEdit->setEnabled(true);
      ui->edit_cmdline_checkBox->setEnabled(true);
      ui->cmdfinal_textEdit->setEnabled(true);
      ui->run_ncircos_button->setEnabled(true);
      ui->Save_stdout_pushButton->setEnabled(true);
      ui->menuBar->setEnabled(true);
      ui->comboBox->setEnabled(true);
      ui->imageciew_checkBox->setEnabled(true);
      ui->circos_bindir_label->setEnabled(true);
      ui->conf_file_label->setEnabled(true);
      ui->out_folder_label->setEnabled(true);
      ui->out_file_label->setEnabled(true);
      ui->cmdline_label->setEnabled(true);
      ui->mandatory_circosbindir_label->setEnabled(true);
      ui->mandatory_conffile_label->setEnabled(true);
      ui->mandatory_label->setEnabled(true);
      ui->stop_pushButton->setEnabled(false);
}

void MainWindow::enablehelpmanversion()
{
    ui->ntoggle_groupBox->setEnabled(true);
    ui->ext_op_args_groupBox->setEnabled(true);
    ui->conf_file_plainTextEdit->setEnabled(true);
    ui->conf_file_pushButton->setEnabled(true);
    ui->out_folder_plainTextEdit->setEnabled(true);
    ui->out_folder_pushButton->setEnabled(true);
    ui->out_file_plainTextEdit->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->imageciew_checkBox->setEnabled(true);
    ui->conf_file_label->setEnabled(true);
    ui->out_folder_label->setEnabled(true);
    ui->out_file_label->setEnabled(true);
    ui->cmdline_label->setEnabled(true);
    ui->mandatory_circosbindir_label->setEnabled(true);
    ui->mandatory_conffile_label->show();
    if (ui->conf_file_plainTextEdit->text().trimmed().count() == 0)
    { ui->conf_file_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid red; border-radius: 2px; background-color : white;}"); } else
    { ui->conf_file_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid green; border-radius: 2px; background-color : white;}"); }
}

void MainWindow::disablehelpmanversion()
{
      ui->ntoggle_groupBox->setEnabled(false);
      ui->ext_op_args_groupBox->setEnabled(false);
      ui->conf_file_plainTextEdit->setEnabled(false);
      ui->conf_file_pushButton->setEnabled(false);
      ui->out_folder_plainTextEdit->setEnabled(false);
      ui->out_folder_pushButton->setEnabled(false);
      ui->out_file_plainTextEdit->setEnabled(false);
      ui->comboBox->setEnabled(false);
      ui->imageciew_checkBox->setEnabled(false);
      ui->conf_file_label->setEnabled(false);
      ui->out_folder_label->setEnabled(false);
      ui->out_file_label->setEnabled(false);
      ui->cmdline_label->setEnabled(false);
      ui->mandatory_circosbindir_label->setEnabled(false);
      ui->mandatory_conffile_label->hide();
      ui->conf_file_plainTextEdit->setStyleSheet("QLineEdit { border: 1px solid grey; border-radius: 2px; background-color : lightgrey;}");
}

void MainWindow::on_imageciew_checkBox_clicked()
{
    if(ui->imageciew_checkBox->isChecked())
    {
        ui->comboBox->setVisible(true);
        image_show_status = true;
    } else
    {
        ui->comboBox->setVisible(false);
        image_show_status = false;
    }
}

void MainWindow::setcircosbindirdefault ()
{
QFile inputFile(QDir::homePath() +"/.ncircos/circosbin.ncd");
   if (inputFile.open(QIODevice::ReadOnly))
   {
      QTextStream in(&inputFile);
      QString line = in.readLine();
      ui->circos_bindir_plainTextEdit->setText(line.trimmed());
      inputFile.close();
   } else ui->circos_bindir_plainTextEdit->setText("");
}

void MainWindow::on_actionSave_settings_triggered()
{
    QString settings_fileName = QFileDialog::getSaveFileName(this, "Save file", "", "ncircos settings file(*.ncs)");
    if (settings_fileName.contains(".ncs") == false)
    {settings_fileName = settings_fileName + ".ncs";}
    QFile file( settings_fileName );
    if ( file.open(QIODevice::ReadWrite| QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        stream << to << endl << nto_png << endl << nto_svg << endl << nto_warnings << endl << nto_paranoid << endl << nto_showticks << endl << nto_showticklabels << endl;
        if (ui->ext_op_args_enable_checkBox->isChecked()) {stream << "true" << endl;} else {stream << "false" << endl;}
        stream << extopargs << endl << circosbindir << endl << conffile << endl << ui->out_folder_plainTextEdit->text() << endl << ui->out_file_plainTextEdit->text() << endl;
        if (ui->imageciew_checkBox->isChecked()) {stream << "true" << endl;} else {stream << "false" << endl;}
        stream << ui->comboBox->currentText()<< endl;
        if (ui->edit_cmdline_checkBox->isChecked()) {stream << "true" << endl;} else {stream << "false" << endl;}
        stream << ui->cmdfinal_textEdit->toPlainText();
        file.close();
    }
}

void MainWindow::on_actionOpen_settings_triggered()
{
    QString settings_filename = QFileDialog::getOpenFileName(this, tr("Open File"),"/path/to/file/",tr("ncircos settings file(*.ncs)"));
    QString cnt = "";
    QFile file( settings_filename );
    if ( file.open(QIODevice::ReadOnly) )
    {
        QTextStream stream( &file );
        QString line = stream.readLine();
        if(line.trimmed().count() != 0)
        {
        if(line.contains("-image_map_use")) {ui->t_image_map_use_radioButton->setChecked(true); on_t_image_map_use_radioButton_clicked();}
        else if (line.contains("-silent")) {ui->t_silent_radioButton->setChecked(true); on_t_silent_radioButton_clicked();}
        else if (line.contains("-cdump")) {ui->t_cdump_radioButton->setChecked(true); on_t_cdump_radioButton_clicked();}
        else if (line.contains("-debug")) {ui->t_debug_radioButton->setChecked(true); on_t_debug_radioButton_clicked();}
        else if (line.contains("-help")) {ui->t_help_radioButton->setChecked(true); on_t_help_radioButton_clicked();}
        else if (line.contains("-man")) {ui->t_man_radioButton->setChecked(true); on_t_man_radioButton_clicked();}
        else if (line.contains("-version")) {ui->t_version_radioButton->setChecked(true); on_t_version_radioButton_clicked();}
        } else {ui->t_default_radioButton->setChecked(true); on_t_default_radioButton_clicked();}
        line = stream.readLine();
        cnt = cnt + line;
        if(line.contains("-png")) {ui->nt_png_radioButton->setChecked(true); on_nt_png_radioButton_clicked();}
        else if (line.contains("-nopng")) {ui->nt_nopng_radioButton->setChecked(true); on_nt_nopng_radioButton_clicked();}
        line = stream.readLine();
        cnt = cnt + line;
        if(line.contains("-svg")) {ui->nt_svg_radioButton->setChecked(true); on_nt_svg_radioButton_clicked();}
        else if (line.contains("-nosvg")) {ui->nt_nosvg_radioButton->setChecked(true); on_nt_nosvg_radioButton_clicked();}
        line = stream.readLine();
        cnt = cnt + line;
        if(line.contains("-warnings")) {ui->nt_warnings_radioButton->setChecked(true); on_nt_warnings_radioButton_clicked();}
        else if (line.contains("-nowarnings")) {ui->nt_nowarnings_radioButton->setChecked(true); on_nt_nowarnings_radioButton_clicked();}
        line = stream.readLine();
        cnt = cnt + line;
        if(line.contains("-paranoid")) {ui->nt_paranoid_radioButton->setChecked(true); on_nt_paranoid_radioButton_clicked();}
        else if (line.contains("-noparanoid")) {ui->nt_noparanoid_radioButton->setChecked(true); on_nt_noparanoid_radioButton_clicked();}
        line = stream.readLine();
        cnt = cnt + line;
        if(line.contains("-show_ticks")) {ui->nt_showticks_radioButton->setChecked(true);on_nt_showticks_radioButton_clicked();}
        else if (line.contains("-noshow_ticks")) {ui->nt_noshowticks_radioButton->setChecked(true); on_nt_noshowticks_radioButton_clicked();}
        line = stream.readLine();
        cnt = cnt + line;
        if(line.contains("-show_tick_labels")) {ui->nt_showticklabels_radioButton->setChecked(true); on_nt_showticklabels_radioButton_clicked();}
        else if (line.contains("-noshow_tick_labels")) {ui->nt_noshowticklabels_radioButton->setChecked(true); on_nt_noshowticklabels_radioButton_clicked();}
        if (cnt.trimmed().count() == 0) {ui->nt_default_radioButton->setChecked(true); on_nt_default_radioButton_clicked(); cnt = "";}
        line = stream.readLine();
        if (line.contains("true")) {ui->ext_op_args_enable_checkBox->setChecked(true);} else {ui->ext_op_args_enable_checkBox->setChecked(false);}
        on_ext_op_args_enable_checkBox_clicked();
        line = stream.readLine();
        ui->ext_op_args_plainTextEdit->setText(line.trimmed());
        line = stream.readLine();
        if(line.contains("[circos/bin_dir...]")) { ui->circos_bindir_plainTextEdit->setText(""); } else { ui->circos_bindir_plainTextEdit->setText(line.trimmed()); }
        line = stream.readLine();
        if(line.contains("[.conf_file...]")) {ui->conf_file_plainTextEdit->setText("");} else { ui->conf_file_plainTextEdit->setText(line.trimmed()); }
        line = stream.readLine();
        ui->out_folder_plainTextEdit->setText(line.trimmed());
        line = stream.readLine();
        ui->out_file_plainTextEdit->setText(line.trimmed());
        line = stream.readLine();
        if (line.contains("true")) {ui->imageciew_checkBox->setChecked(true);} else {ui->imageciew_checkBox->setChecked(false);}
        line = stream.readLine();
        if (line.contains("png")) {ui->comboBox->setCurrentIndex(0);} else {ui->comboBox->setCurrentIndex(1);}
        line = stream.readLine();
        if (line.contains("true")) {ui->edit_cmdline_checkBox->setChecked(true);} else {ui->edit_cmdline_checkBox->setChecked(false);}
        on_edit_cmdline_checkBox_clicked();
        line = stream.readLine();
        QStringList query = line.split(" ");
        ui->cmdfinal_textEdit->setPlainText("");
        for (int i=0; i < query.size(); i++)
        {
            if (query.at(i).contains("[circos/bin_dir...]/circos"))
            {
                QStringList cbind = query.at(i).split("/");

                ui->cmdfinal_textEdit->setTextColor( QColor( "red" ) );
                ui->cmdfinal_textEdit->insertPlainText(cbind.at(0) + "/");
                ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
                ui->cmdfinal_textEdit->insertPlainText(cbind.at(1));
                ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
                ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
                ui->cmdfinal_textEdit->insertPlainText("/" + cbind.at(2) + " ");
            }
            else if (query.at(i).contains("[.conf_file...]"))
            {
                ui->cmdfinal_textEdit->setTextColor( QColor( "red" ) );
                ui->cmdfinal_textEdit->insertPlainText(query.at(i) + " ");
                ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
                ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
            } else
            {
                ui->cmdfinal_textEdit->setTextColor( QColor( "black" ) );
                ui->cmdfinal_textEdit->insertPlainText(query.at(i) + " ");
                ui->cmdfinal_textEdit->moveCursor (QTextCursor::End);
            }
        }
        //ui->cmdfinal_textEdit->setText(ui->cmdfinal_textEdit->toPlainText().trimmed());
        file.close();
    }
    //if(ui->nt_png_radioButton->isChecked() || ui->nt_nopng_radioButton->isChecked() || ui->nt_svg_radioButton->isChecked() || ui->nt_nosvg_radioButton->isChecked() || ui->nt_warnings_radioButton->isChecked() || ui->nt_nowarnings_radioButton->isChecked() || ui->nt_paranoid_radioButton->isChecked() || ui->nt_noparanoid_radioButton->isChecked() || ui->nt_showticks_radioButton->isChecked() || ui->nt_noshowticks_radioButton->isChecked() || ui->nt_showticklabels_radioButton->isChecked() || ui->nt_noshowticklabels_radioButton->isChecked())
    //{
     //   ui->nt_default_radioButton->setChecked(false);
    //} else ui->nt_default_radioButton->setChecked(true);
    if(ui->imageciew_checkBox->isChecked())
    {
        ui->comboBox->setVisible(true);
        image_show_status = true;
    } else
    {
        ui->comboBox->setVisible(false);
        image_show_status = false;
    }
}

int MainWindow::delete_existingfiles ()
{
    QString ext, cmdfinaltest, ofile, odir, oconf;
    int del = 2;
    cmdfinaltest = ui->cmdfinal_textEdit->toPlainText().trimmed();
    QStringList query = cmdfinaltest.split(" ");
    for (int i=0; i < query.size(); i++)
    {
        if (query.at(i).contains("-conf"))
        {
            oconf = query.at(i+1).trimmed();
        }
        if (query.at(i).contains("-outputdir"))
        {
            odir = query.at(i+1).trimmed();
        }
        if (query.at(i).contains("-outputfile"))
        {
            ofile = query.at(i+1).trimmed();
        }
    }
    ext = ui->comboBox->currentText().trimmed();
    QFileInfo info1(oconf);
    if (odir.trimmed() == "") { odir = info1.absolutePath();}
    if (ofile.trimmed() == "") { ofile = "circos";}
    QFile imagefilepng(odir + "/" + ofile + ".png");
    QFile imagefilesvg(odir + "/" + ofile + ".svg");
    if(imagefilepng.exists() && imagefilesvg.exists())
    {
       QMessageBox msgBox;
       msgBox.setText("Delete existing .svg & .png file(s)");
       msgBox.setWindowModality(Qt::NonModal);
       msgBox.setInformativeText(odir + "/" + ofile + ".png" + " & " + odir + "/" + ofile + ".svg" + " files exist(s) in the respective directory. Press Ok to delete this file(s) or  press Cancel to abort circos run");
       msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
       if (msgBox.exec() == QMessageBox::Ok) { del = 1; imagefilepng.remove(); imagefilesvg.remove();} else { del = 0; }
    }
    if(imagefilepng.exists() && !imagefilesvg.exists())
    {
       QMessageBox msgBox;
       msgBox.setText("Delete existing .png file");
       msgBox.setWindowModality(Qt::NonModal);
       msgBox.setInformativeText(odir + "/" + ofile + ".png" + " file exist(s) in the respective directory. Press Ok to delete this file or  press Cancel to abort circos run");
       msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
       if (msgBox.exec() == QMessageBox::Ok) { del = 1; imagefilepng.remove();} else { del = 0; }
    }
    if(!imagefilepng.exists() && imagefilesvg.exists())
    {
       QMessageBox msgBox;
       msgBox.setText("Delete existing .svg file");
       msgBox.setWindowModality(Qt::NonModal);
       msgBox.setInformativeText(odir + "/" + ofile + ".svg" + " file exist(s) in the respective directory. Press Ok to delete this file or  press Cancel to abort circos run");
       msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
       if (msgBox.exec() == QMessageBox::Ok) { del = 1; imagefilesvg.remove();} else { del = 0; }
    }
    return del;
}

void MainWindow::on_actionCommand_line_CMD_triggered()
{
    ui->exec_status_plainTextEdit->setPlainText("");
    ui->exec_status_err_textEdit->setPlainText("");
    disablectrl_runtime();
    ui->progressBar->setMaximum(0);
    QStringList circosdir = circosbindir.split("/");
    QString circosdirfinal;
    for (int i = 0; i<circosdir.size()-1; i++)
    {
        circosdirfinal = circosdirfinal + "/" + circosdir.at(i);
    }
    ui->exec_status_plainTextEdit->appendPlainText("circos/bin directory : " + circosbindir);
    ui->exec_status_plainTextEdit->appendPlainText("\n Creating " + QDir::homePath() + "/.ncircos/testmobule_bashscript.sh bashscript ....");
    QDir dir(QDir::homePath() + "/.ncircos");
    if (!dir.exists()) { dir.mkpath("."); }
    QString filename = QDir::homePath() + "/.ncircos/testmobule_bashscript.sh";
    QFile file( filename );
    QString first, second, final;
    first = "#!/bin/bash";
    second = "echo \"Required Perl Modules For running circos and their status :\"";
    final = "awk '!/^[\\t ]*use /{next};$2~/^(lib|Circos.*|base|strict|vars|warnings);?$/{next};{sub(\";\",\"\",$2);print $2}' " + circosdirfinal.trimmed() + "/bin/circos " + circosdirfinal.trimmed() + "/lib/Circos/*pm " + circosdirfinal.trimmed() + "/lib/Circos.pm " + circosdirfinal.trimmed() + "/lib/Circos/*/*pm | sort -u | xargs -I MODULE perl -I../lib -e  'print eval \"use MODULE;1\"?\"ok   MODULE\":\"fail MODULE is not usable (it or a sub-module is missing)\\n\"'";
    if ( file.open(QIODevice::ReadWrite| QIODevice::Truncate | QIODevice::Text) )
    {
        QTextStream stream( &file );
        stream << first << endl << second << endl << final;
        file.close();
        ui->exec_status_plainTextEdit->appendPlainText(QDir::homePath() + "/.ncircos/testmobule_bashscript.sh bashscript file creation successfull ....");
    }
    ui->exec_status_plainTextEdit->appendPlainText("making the bash script executable \"chmod a+x " + QDir::homePath() + "/.ncircos/testmobule_bashscript.sh\" ....");
    process  = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(updateErr()));
    connect(process, SIGNAL(finished(int)), this, SLOT(chmodExit()));
    process->start("bash -c \"chmod a+x "+ filename + "\"");
}

void MainWindow::chmodExit()
{
    ui->exec_status_plainTextEdit->appendPlainText("bash script chmod successfull ....");
    ui->exec_status_plainTextEdit->appendPlainText("Executing the bash script ....");
    process  = new QProcess(this);
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateText()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(updateErr()));
    connect(process, SIGNAL(finished(int)), this, SLOT(modulecheckbashscriptExit()));
    process->start(QDir::homePath() + "/.ncircos/testmobule_bashscript.sh");
}

void MainWindow::modulecheckbashscriptExit()
{
    enablectrl_runtime();
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(100);
    if(ui->exec_status_plainTextEdit->toPlainText().contains("fail") || ui->exec_status_err_textEdit->toPlainText().trimmed().count() != 0)
    {ui->exec_status_err_textEdit->appendPlainText("Some modules/sub-modules are not poperly installed");} else
    {ui->exec_status_plainTextEdit->appendPlainText("\n All perl modules/sub-modules needed to run circos are properly installed!");}
}

void MainWindow::stopExecution() {
  process->terminate();
  if (process->pid() > 0) {
    ui->exec_status_err_textEdit->appendPlainText(
        "*****!!! PROCESS ABORTED BY USER !!!*****");
  }
}

void MainWindow::installPerlModule() {
  module_installer mod_installer;
  mod_installer.setModal(true);
  mod_installer.exec();
}

void MainWindow::exit() {
  close();
}

void MainWindow::setBinDir() {
    SetBinDir setbindir;
    setbindir.setModal(true);
    setbindir.exec();
}

void MainWindow::onlineDocumentation() {
  QDesktopServices::openUrl(QUrl("http://www.researcharun.com/"
                                 "ncircos_documentation_linux.html"));
}

void MainWindow::quickReferanceManuel() {
  QDesktopServices::openUrl(QUrl(Utils::getConfigPath(Utils::Manual)));
}

void MainWindow::about() {
  AboutBox about;
  about.setModal(true);
  about.exec();
}
