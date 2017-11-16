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

#include "edge3dentity.h"
#include "vertex3dentity.h"

#include <QDebug>

Edge3DEntity::Edge3DEntity(QNode *parent)
    : Qt3DCore::QEntity(parent)
{
    m_v[0] = nullptr;
    m_v[1] = nullptr;

    m_mesh = new Qt3DExtras::QCylinderMesh();
    m_mesh->setRadius(0.05);
    m_mesh->setLength(1);
    m_mesh->setRings(100);
    m_mesh->setSlices(20);

    m_material = new Qt3DExtras::QPhongMaterial(this);
    m_material->setDiffuse(QColor(QRgb(0x928327)));

    m_transform = new Qt3DCore::QTransform();
//    m_transform->setScale(1.5f);

    addComponent(m_mesh);
    addComponent(m_material);
    addComponent(m_transform);
}


Edge3DEntity::Edge3DEntity(QNode *parent, Vertex3DEntity *v, Vertex3DEntity *w)
    : Edge3DEntity(parent)
{
    init(v,w);
}

void Edge3DEntity::init (Vertex3DEntity *v, Vertex3DEntity *w)
{
    m_v[0] = v;
    m_v[1] = w;

    updateGeometry();
}

void Edge3DEntity::updateGeometry()
{
    const auto posV = m_v[0]->pos();
    const auto posW = m_v[1]->pos();

    const auto oldDir = m_dir.isNull() ? QVector3D(0.0f, 1.0f, 0.0f) : m_dir;
    const auto newDir = posW - posV;

//    qDebug() << "Edge" << posV << posW << "|" << oldDir << newDir;

    m_mesh->setLength(newDir.length());
    m_transform->setTranslation(posV + newDir / 2);

    QQuaternion rotQ = QQuaternion::rotationTo(oldDir, newDir);
    m_transform->setRotation(rotQ);

    m_dir = newDir;
}
