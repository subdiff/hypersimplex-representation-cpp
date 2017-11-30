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

#ifndef EDGE3DENTITY_H
#define EDGE3DENTITY_H

#include <vector>

#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>

class Vertex3DEntity;

class Edge3DEntity : public Qt3DCore::QEntity {
public:
    Edge3DEntity(QNode *parent = nullptr);
    Edge3DEntity(QNode *parent, Vertex3DEntity *v, Vertex3DEntity *w);
    void init (Vertex3DEntity *v, Vertex3DEntity *w);

    void updateGeometry();

private:
    Qt3DExtras::QCylinderMesh *m_mesh;
    Qt3DExtras::QPhongMaterial *m_material;
    Qt3DCore::QTransform *m_transform;

    Vertex3DEntity *m_v[2];
};

#endif // EDGE3DENTITY_H
