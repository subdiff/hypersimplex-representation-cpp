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

#include <vector>
#include <eigen3/Eigen/Dense>

#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>


class Vertex3DEntity : public Qt3DCore::QEntity {
public:
    Vertex3DEntity(QNode *parent = nullptr);
    Vertex3DEntity(QNode *parent, Eigen::VectorXd pos);

//    void init(Eigen::VectorXd pos);

    QVector3D pos() const {
        return m_pos;
    }

    void updateGeometry(Eigen::VectorXd pos);

private:
    Qt3DExtras::QSphereMesh *m_mesh;
    Qt3DExtras::QPhongMaterial *m_material;
    Qt3DCore::QTransform *m_transform;

    QVector3D m_pos;
};
