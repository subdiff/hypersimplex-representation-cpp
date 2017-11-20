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

#include "root3dwrapper.h"

#include "root3dentity.h"
#include "../gimatrix.h"

#include <QDebug>

Root3DWrapper::Root3DWrapper(QObject *parent)
    : QObject(parent)
{
}

void Root3DWrapper::setRoot3DPtr(Qt3DCore::QEntity *set)
{
    if (m_root3d != set) {
        m_root3d = set;
    }
    emit root3DPtrChanged();
}

void Root3DWrapper::setBackEnd(BackEnd *set)
{
    if (m_backEnd != set) {
        m_backEnd = set;
    }
    emit backEndChanged();
}

void Root3DWrapper::setProjFacet(int set)
{
    if (m_projFacet != set) {
        m_projFacet = set;

        initGeometries();
        emit projFacetChanged();
    }
}

void Root3DWrapper::setProjToLargerFacet(bool set)
{
    if (m_projToLargerFacet != set) {
        m_projToLargerFacet = set;

        initGeometries();
        emit projToLargerFacetChanged();
    }
}

void Root3DWrapper::setEntityData()
{
    if (!m_root3d) {
        return;
    }
    Root3DEntity *root_entity = dynamic_cast<Root3DEntity *>(m_root3d);

    root_entity->setProjFacet(m_projFacet);
    root_entity->setProjToLargerFacet(m_projToLargerFacet);
}

void Root3DWrapper::initGeometries()
{
    if (!m_root3d) {
        return;
    }
    Root3DEntity *root_entity = dynamic_cast<Root3DEntity *>(m_root3d);
    root_entity->clearGeometries();
    setEntityData();

    GiMatrix *matrix = m_backEnd->getGiMatrix();
    if (!matrix) {
        return;
    }
    root_entity->initGeometries(matrix);
}

void Root3DWrapper::clearGeometries()
{
    if (!m_root3d) {
        return;
    }
    Root3DEntity *root_entity = dynamic_cast<Root3DEntity *>(m_root3d);

    root_entity->clearGeometries();
}

void Root3DWrapper::updateGeometries()
{
    if (!m_root3d) {
        return;
    }
    Root3DEntity *root_entity = dynamic_cast<Root3DEntity *>(m_root3d);

    GiMatrix *matrix = m_backEnd->getGiMatrix();
    if (!matrix) {
        return;
    }

    root_entity->updateGeometries(matrix);
}
