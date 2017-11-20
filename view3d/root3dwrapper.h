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

#ifndef ROOT3DWRAPPER_H
#define ROOT3DWRAPPER_H

#include "../backend.h"

#include <Qt3DCore/QEntity>
#include <QObject>

class Root3DWrapper : public QObject {
    Q_OBJECT

    Q_PROPERTY(Qt3DCore::QEntity * root3DPtr READ root3DPtr WRITE setRoot3DPtr NOTIFY root3DPtrChanged)
    Q_PROPERTY(BackEnd * backEnd READ backEnd WRITE setBackEnd NOTIFY backEndChanged)

    Q_PROPERTY(int projFacet READ projFacet WRITE setProjFacet NOTIFY projFacetChanged)
    Q_PROPERTY(bool projToLargerFacet READ projToLargerFacet WRITE setProjToLargerFacet NOTIFY projToLargerFacetChanged)
public:
    Root3DWrapper(QObject *parent = nullptr);

    Qt3DCore::QEntity *root3DPtr() {
        return m_root3d;
    }
    void setRoot3DPtr(Qt3DCore::QEntity *set);

    int projFacet() {
        return m_projFacet;
    }
    void setProjFacet(int set);
    bool projToLargerFacet() {
        return m_projToLargerFacet;
    }
    void setProjToLargerFacet(bool set);

    BackEnd *backEnd() {
        return m_backEnd;
    }
    void setBackEnd(BackEnd *set);

    Q_INVOKABLE void initGeometries();
    Q_INVOKABLE void clearGeometries();
    Q_INVOKABLE void updateGeometries();

Q_SIGNALS:
    void root3DPtrChanged();
    void backEndChanged();
    void projFacetChanged();
    void projToLargerFacetChanged();

private:
    void setEntityData();
    Qt3DCore::QEntity *m_root3d = nullptr;
    BackEnd *m_backEnd = nullptr;
    int m_projFacet = 0;
    bool m_projToLargerFacet = true;
};

#endif // ROOT3DWRAPPER_H
