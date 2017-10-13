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

#include "mainwindow.h"
#include <hypersimplex.h>

#include <QtConcurrent/QtConcurrentRun>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow)
{
    setWindowTitle("Hypersimplex Representer");
//    resize(400, 300);
    m_ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    // TODO: Make sure running s_createHypersimplex calls are aborted

    if (::s_hypers) {
        delete ::s_hypers;
        ::s_hypers = nullptr;
    }
}

void MainWindow::getHypersimplex(int d, int k)
{
    m_ready = false;
//    emit readyChanged();

    setVtxTrSubgroups(std::vector<std::string>());
//    setSelectedSubgroup(0);

    QtConcurrent::run(::s_createHypersimplex, d, k);

    m_checkReadyTimer = new QTimer(this);
    connect(m_checkReadyTimer, SIGNAL(timeout()), this, SLOT(checkReady()));
    m_checkReadyTimer->start(1000);
}

void MainWindow::checkReady()
{
    if (!::s_ready) {
        return;
    }

    if (!m_ready) {
        m_ready = ::s_ready;

        if (m_checkReadyTimer) {
            delete m_checkReadyTimer;
            m_checkReadyTimer = nullptr;
        }
        setVtxTrSubgroups(::s_hypers->getVtxTrSubgroupNames());
        qDebug() << "checkReady";
    }
}

void MainWindow::setVtxTrSubgroups(std::vector<std::string> subNames)
{
    bool changed = false;
    if (subNames.size() != m_vtxTrSubgroups.size()) {
        changed = true;
    }

    if (!changed) {
        int index = 0;
        for (auto s : m_vtxTrSubgroups) {
            if (QString::compare(s, subNames[index].c_str()) != 0) {
                changed = true;
                break;
            }
            index++;
        }
    }

    if (changed) {
        m_vtxTrSubgroups.clear();
        for (std::string sub : subNames) {
            m_vtxTrSubgroups.append(QString(sub.c_str()));
        }
    }

    qDebug() << m_vtxTrSubgroups;
    m_ui->vtxTrSubgrCombo->clear();
    m_ui->vtxTrSubgrCombo->insertItems(0, m_vtxTrSubgroups);
    m_ui->vtxTrSubgrCombo->setCurrentIndex(0);
}

void MainWindow::on_dSpin_valueChanged(int value)
{
    auto kValue = m_ui->kSpin->value();
    if (kValue >= value) {
        m_ui->kSpin->setValue(value - 1);
    }
    setButtonsEnabled(isChangedSpinValues());
}

void MainWindow::on_kSpin_valueChanged(int value)
{
    auto dValue = m_ui->dSpin->value();
    if (dValue <= value) {
        m_ui->kSpin->setValue(dValue - 1);
    }
    setButtonsEnabled(isChangedSpinValues());
}

void MainWindow::on_ApplyButton_clicked()
{
    m_curD = m_ui->dSpin->value();
    m_curK = m_ui->kSpin->value();
    setButtonsEnabled(false);

    getHypersimplex(m_curD, m_curK);
}

void MainWindow::on_ResetButton_clicked()
{
    m_ui->dSpin->setValue(m_curD);
    m_ui->kSpin->setValue(m_curK);
}

void MainWindow::setButtonsEnabled(bool enable)
{
    m_ui->ApplyButton->setEnabled(enable);
    m_ui->ResetButton->setEnabled(enable);
}

bool MainWindow::isChangedSpinValues()
{
    return m_curD != m_ui->dSpin->value() || m_curK != m_ui->kSpin->value();
}
