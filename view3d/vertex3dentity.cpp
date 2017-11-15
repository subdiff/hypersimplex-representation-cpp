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

Vertex3DEntity::Vertex3DEntity(QNode *parent)
    : Qt3DCore::QEntity(parent)
{
    m_mesh = new Qt3DExtras::QSphereMesh();
    m_mesh->setRadius(2);
    m_mesh->setRings(100);
    m_mesh->setSlices(20);

    m_material = new Qt3DExtras::QPhongMaterial(this);
    m_material->setDiffuse(QColor("blue"));

    m_transform = new Qt3DCore::QTransform();
//    m_transform->setScale(1.5f);
    m_transform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
    m_transform->setTranslation(QVector3D(-5.0f, 4.0f, -1.5));

    addComponent(m_mesh);
    addComponent(m_material);
    addComponent(m_transform);
}
