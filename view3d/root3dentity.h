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

#ifndef ROOT3DENTITY_H
#define ROOT3DENTITY_H

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>

#include <eigen3/Eigen/Dense>

#include <vector>

class Vertex3DEntity;
class Edge3DEntity;
class GiMatrix;

class Root3DEntity : public Qt3DCore::QEntity {
public:
    Root3DEntity(QNode *parent = nullptr);

    void setProjFacet(int set) {
        m_projFacet = set;
    }
    void setProjToLargerFacet(bool set) {
        m_projToLargerFacet = set;
    }

    void initGeometries(GiMatrix *matrix);
    void clearGeometries();
    void updateGeometries(GiMatrix *matrix);

private:
    void createCoordOrigin();
    void createCoordAxes();
    std::vector<Eigen::VectorXd> getNullspaceRepr(GiMatrix *matrix);
    std::vector<Vertex3DEntity *> m_vertices;
    std::vector<Edge3DEntity *> m_edges;

    int m_projFacet = 0;
    bool m_projToLargerFacet = true;
};

#endif // ROOT3DENTITY_H
