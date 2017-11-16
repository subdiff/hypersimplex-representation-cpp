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

class GiMatrix;

class BackEnd : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(QStringList vtxTrSubgroups READ vtxTrSubgroups NOTIFY vtxTrSubgroupsChanged)
    Q_PROPERTY(int selectedSubgroup READ selectedSubgroup WRITE setSelectedSubgroup NOTIFY selectedSubgroupChanged)

public:
    explicit BackEnd(QObject *parent = nullptr);
    ~BackEnd();

    Q_INVOKABLE void getHypersimplex(int d, int k);
    Q_INVOKABLE void calcNullSpRepr();

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
    void setSelectedSubgroup(int set) {
        if (m_selectedSubgroup != set) {
            m_selectedSubgroup = set;
            setGiMatrix(set);
            calcNullSpRepr();
            emit selectedSubgroupChanged();
        }
    }

    GiMatrix *getGiMatrix() {
        return m_reprMatrix;
    }

public Q_SLOTS:
    void checkReady();

Q_SIGNALS:
    void readyChanged();
    void vtxTrSubgroupsChanged();
    void selectedSubgroupChanged();

private:
    void setGiMatrix(int subgroup);

    QTimer *m_checkReadyTimer = nullptr;
    bool m_ready = true;

    QStringList m_vtxTrSubgroups;
    int m_selectedSubgroup = 0;
    GiMatrix *m_reprMatrix = nullptr;
};

#endif // BACKEND_H
