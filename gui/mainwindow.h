/*********************************************************************
Hypersimplex Representer
Copyright (C) 2017 Roman Gilg

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QStringList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void getHypersimplex(int d, int k);
    void setVtxTrSubgroups(std::vector<std::string> subNames);

public Q_SLOTS:
    void checkReady();

private Q_SLOTS:
    void on_dSpin_valueChanged(int value);
    void on_kSpin_valueChanged(int value);

    void on_ApplyButton_clicked();
    void on_ResetButton_clicked();

private:
    bool isChangedSpinValues();
    void setButtonsEnabled(bool enable);

    Ui::MainWindow *m_ui;

    QTimer *m_checkReadyTimer = nullptr;
    bool m_ready;
    QStringList m_vtxTrSubgroups;
    int m_selectedSubgroup = -1;

    int m_curD = 3;
    int m_curK = 1;
};

#endif  // MAINWINDOW_H
