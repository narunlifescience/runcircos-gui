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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QAction>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_run_ncircos_button_clicked();

    void on_conf_file_pushButton_clicked();

    void on_circos_bindir_pushButton_clicked();

    void on_out_folder_pushButton_clicked();

    void on_edit_cmdline_checkBox_clicked();

    void on_ext_op_args_enable_checkBox_clicked();

    void on_nt_default_radioButton_clicked();

    void updateText();

    void on_nt_png_radioButton_clicked();

    void on_nt_nopng_radioButton_clicked();

    void on_nt_svg_radioButton_clicked();

    void on_nt_nosvg_radioButton_clicked();

    void on_nt_warnings_radioButton_clicked();

    void on_nt_nowarnings_radioButton_clicked();

    void on_nt_paranoid_radioButton_clicked();

    void on_nt_noparanoid_radioButton_clicked();

    void on_nt_showticks_radioButton_clicked();

    void on_nt_noshowticks_radioButton_clicked();

    void on_nt_showticklabels_radioButton_clicked();

    void on_nt_noshowticklabels_radioButton_clicked();

    void on_t_default_radioButton_clicked();

    void on_t_image_map_use_radioButton_clicked();

    void on_t_silent_radioButton_clicked();

    void on_t_cdump_radioButton_clicked();

    void on_t_debug_radioButton_clicked();

    void on_t_help_radioButton_clicked();

    void on_t_man_radioButton_clicked();

    void on_t_version_radioButton_clicked();

    void on_actionOnline_documentation_triggered();

    void update_cmdfinal();

    void on_circos_bindir_plainTextEdit_textChanged();

    void on_conf_file_plainTextEdit_textChanged();

    void on_out_folder_plainTextEdit_textChanged();

    void on_out_file_plainTextEdit_textChanged();

    void on_ext_op_args_plainTextEdit_textChanged();

    void on_actionAbout_triggered();

    void on_actionCheck_version_triggered();

    void on_actionNew_triggered();

    void on_actionClear_exec_status_triggered();

    void on_actionSet_circos_bin_directory_triggered();

    void updateExit();

    void updateErr();

    void on_pushButton_clicked();

    void on_Save_stdout_pushButton_clicked();

    void on_actionQuick_referance_manuel_QRM_triggered();

    void on_actionEdit_conf_file_triggered();

    void on_actionEdit_other_files_triggered();

    void viewimage ();

    void disablectrl_runtime();

    void enablectrl_runtime();

    void updateExitperlcheck();

    void updateExitperlcheckcomplete();

    void on_imageciew_checkBox_clicked();

    void on_actionQuit_triggered();

    void setcircosbindirdefault ();

    void on_actionSave_settings_triggered();

    void on_actionOpen_settings_triggered();

    void disablehelpmanversion ();

    void enablehelpmanversion ();

    int delete_existingfiles ();

    void on_actionCommand_line_CMD_triggered();

    void chmodExit();

    void modulecheckbashscriptExit();

    void on_actionInstall_perl_package_triggered();

private:
    Ui::MainWindow *ui;
    QProcess *process;

};

#endif // MAINWINDOW_H
