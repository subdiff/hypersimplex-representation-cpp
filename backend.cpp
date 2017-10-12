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

#include "backend.h"
#include "hypersimplex.h"

#include <QtConcurrent/QtConcurrentRun>

BackEnd::BackEnd(QObject *parent) :
    QObject(parent)
{
}

BackEnd::~BackEnd()
{
    // TODO: Make sure running s_createHypersimplex calls are aborted

    if (::s_hypers) {
        delete ::s_hypers;
        ::s_hypers = nullptr;
    }
}

bool BackEnd::ready()
{
    return m_ready;
}

void BackEnd::getHypersimplex(int d, int k)
{
    m_ready = false;
    readyChanged();

    QtConcurrent::run(::s_createHypersimplex, d, k);

    m_checkReadyTimer = new QTimer(this);
    connect(m_checkReadyTimer, SIGNAL(timeout()), this, SLOT(checkReady()));
    m_checkReadyTimer->start(1000);
}

void BackEnd::checkReady()
{
    if (::s_ready && !m_ready) {
        m_ready = ::s_ready;
        readyChanged();

        if (m_checkReadyTimer) {
            delete m_checkReadyTimer;
            m_checkReadyTimer = nullptr;
        }
    }
}
