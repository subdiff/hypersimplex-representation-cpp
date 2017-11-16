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

#include "vertex3dentity.h"

#include <QDebug>
#include <iostream>

Vertex3DEntity::Vertex3DEntity(QNode *parent)
    : Qt3DCore::QEntity(parent)
{
    m_mesh = new Qt3DExtras::QSphereMesh(this);
    m_mesh->setRadius(0.1);
    m_mesh->setRings(100);
    m_mesh->setSlices(20);

    m_material = new Qt3DExtras::QPhongMaterial(this);
    m_material->setDiffuse(QColor(QRgb(0x928327)));

    m_transform = new Qt3DCore::QTransform(this);
//    m_transform->setScale(1.5f);

    addComponent(m_mesh);
    addComponent(m_material);
    addComponent(m_transform);
}

Vertex3DEntity::Vertex3DEntity(QNode *parent, Eigen::VectorXd pos)
    : Vertex3DEntity(parent)
{
    updateGeometry(pos);
}

void Vertex3DEntity::updateGeometry(Eigen::VectorXd pos)
{
    std::cout << pos  << std::endl;

    if (pos.rows() == 2) {
        m_pos = QVector3D(pos[0], pos[1], 0);
    } else if (pos.rows() == 3) {
        m_pos = QVector3D(pos[0], pos[1], pos[2]);
    }
    m_transform->setTranslation(m_pos);
}
