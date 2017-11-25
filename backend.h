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

#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QTimer>
#include <QStringList>

#include <vector>

#include <vertex.h>

class GiMatrix;

typedef std::pair<std::vector<Vertex>, std::vector<Vertex> > facet_pair;

class EecWrap : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double val READ val NOTIFY valChanged)
    Q_PROPERTY(int mult READ mult NOTIFY multChanged)

public:
    explicit EecWrap(QObject *parent, double val, int mult)
        : m_val(val),
          m_mult(mult)
    {}

    double val() const {
        return m_val;
    }
    double mult() const {
        return m_mult;
    }

Q_SIGNALS:
    void valChanged();
    void multChanged();

private:
    double m_val;
    int m_mult;
};

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QStringList vtxTrSubgroups READ vtxTrSubgroups NOTIFY vtxTrSubgroupsChanged)
    Q_PROPERTY(int selectedSubgroup READ selectedSubgroup WRITE setSelectedSubgroup NOTIFY selectedSubgroupChanged)
    Q_PROPERTY(int selEigenvectMode READ selEigenvectMode WRITE setSelEigenvectMode NOTIFY selEigenvectModeChanged)

    Q_PROPERTY(QList<QObject*> eecWraps READ eecWraps NOTIFY eecWrapsChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    Q_INVOKABLE void createHypersimplex(int d, int k);
    Q_INVOKABLE void setVars(QList<double > vars);

    bool ready() const {
        return m_ready;
    }
    QList<QString> vtxTrSubgroups() const {
        return m_vtxTrSubgroups;
    }
    void setVtxTrSubgroups(std::vector<std::string> subNames);

    int selectedSubgroup() const {
        return m_selectedSubgroup;
    }
    void setSelectedSubgroup(int set);

    int selEigenvectMode() const {
        return m_selEigenvectMode;
    }
    void setSelEigenvectMode(int mode);

    GiMatrix *getGiMatrix() {
        return m_reprMatrix;
    }

    QList<QObject*> eecWraps() const {
        return m_eecWraps;
    }
    void setEecWraps();

    facet_pair getFacetPair(int index = 0) const;

public Q_SLOTS:
    void checkReady();

Q_SIGNALS:
    void readyChanged();
    void vtxTrSubgroupsChanged();
    void selectedSubgroupChanged();
    void eecWrapsChanged();
    void geometryInitNeeded();
    void geometryUpdateNeeded();
    void selEigenvectModeChanged();

private:
    void setGiMatrix(int subgroup);

    QTimer *m_checkReadyTimer = nullptr;
    bool m_ready = true;

    QStringList m_vtxTrSubgroups;
    int m_selectedSubgroup = 0;
    GiMatrix *m_reprMatrix = nullptr;

    QList<QObject*> m_eecWraps;
    int m_selEigenvectMode = 0;
};

#endif // BACKEND_H
