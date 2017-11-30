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
#include "gimatrix.h"

#include <QtConcurrent/QtConcurrentRun>

BackEnd::BackEnd(QObject *parent) :
    QObject(parent)
{}

BackEnd::~BackEnd()
{
    if (::s_hypers) {
        delete ::s_hypers;
        ::s_hypers = nullptr;
    }
    delete m_reprMatrix;
}

facet_pair BackEnd::getFacetPair(int index) const
{
    if (::s_hypers = nullptr) {
        return facet_pair(std::vector<Vertex>(), std::vector<Vertex>());
    }
    return ::s_hypers->getFacetPair(index);
}

void BackEnd::createHypersimplex(int d, int k)
{
    m_ready = false;
    emit readyChanged();

    setVtxTrSubgroups(std::vector<std::string>());
    setSelectedSubgroup(0);

    QtConcurrent::run(::s_createHypersimplex, d, k);

    m_checkReadyTimer = new QTimer(this);
    connect(m_checkReadyTimer, SIGNAL(timeout()), this, SLOT(checkReady()));
    m_checkReadyTimer->start(1000);
}

void BackEnd::setSelectedSubgroup(int set) {
    if (m_selectedSubgroup != set) {
        m_selectedSubgroup = set;
        setGiMatrix(set);
        setEecWraps();
        emit selectedSubgroupChanged();
    }
}

void BackEnd::setSelEigenvectMode(int mode)
{
    if (m_selEigenvectMode != mode) {
        m_selEigenvectMode = mode;
        if(m_reprMatrix) {
            m_reprMatrix->setSelEigenvectMode(mode);
            m_reprMatrix->calcNullspaceRepr();
        }
        emit selEigenvectModeChanged();
        emit geometryInitNeeded();
    }
}

void BackEnd::setEecWraps() {
    auto clear = [this]() {
        for (auto eW : m_eecWraps) {
            delete eW;
        }
        m_eecWraps.clear();
    };

    if (m_reprMatrix) {
        clear();
        auto vars = m_reprMatrix->getVars();
        auto mults = m_reprMatrix->getMultiplicities();
        for (int i = 0; i < vars.size(); i++) {
            auto eecWrap = new EecWrap(this, vars[i], mults[i]);
            m_eecWraps.append(eecWrap);
        }
        emit eecWrapsChanged();
    } else if (!m_eecWraps.isEmpty()) {
        clear();
        emit eecWrapsChanged();
    }
}

void BackEnd::setVars(QList<double > vars)
{
    m_reprMatrix->setVars(vars.toVector().toStdVector());
    emit geometryUpdateNeeded();
}

void BackEnd::setGiMatrix(int subgroup)
{
    delete m_reprMatrix;

    m_reprMatrix = new GiMatrix(::s_hypers->getGiMatrix(subgroup));
    m_reprMatrix->setSelEigenvectMode(m_selEigenvectMode);
    m_reprMatrix->init();

    emit geometryInitNeeded();
}

void BackEnd::checkReady()
{
    if (!::s_ready) {
        return;
    }

    if (!m_ready) {
        m_ready = ::s_ready;
        emit readyChanged();

        if (m_checkReadyTimer) {
            delete m_checkReadyTimer;
            m_checkReadyTimer = nullptr;
        }
        setVtxTrSubgroups(::s_hypers->getVtxTrSubgroupNames());
    }
}

void BackEnd::setVtxTrSubgroups(std::vector<std::string> subNames)
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
        setGiMatrix(0);
        setEecWraps();
        emit vtxTrSubgroupsChanged();
    }
}
