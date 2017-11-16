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

#include "root3dentity.h"
#include "vertex3dentity.h"
#include "edge3dentity.h"
#include "../gimatrix.h"

#include <QDebug>

#include <QRenderSettings>
#include <QForwardRenderer>
#include <QInputSettings>

#include "qorbitcameracontroller.h"

Root3DEntity::Root3DEntity(QNode *parent)
    : Qt3DCore::QEntity(parent)
{
    // Camera
    Qt3DRender::QCamera *camera = new Qt3DRender::QCamera(this);
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 4.0f));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // Camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(this);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    Qt3DExtras::QForwardRenderer *forwRnd = new Qt3DExtras::QForwardRenderer();
    forwRnd->setCamera(camera);
    forwRnd->setClearColor(QColor("transparent"));

    Qt3DRender::QRenderSettings *renderSettings = new Qt3DRender::QRenderSettings(this);
    renderSettings->setActiveFrameGraph(forwRnd);
    addComponent(renderSettings);

    Qt3DInput::QInputSettings *inputSettings = new Qt3DInput::QInputSettings(this);
    addComponent(inputSettings);
}

void Root3DEntity::initGeometries(GiMatrix *matrix)
{
    qDebug() << "initGeometries";

    m_vertices.clear();
    auto nullSpRepr = matrix->getNullspaceRepr();

    for (auto v : nullSpRepr) {
        Vertex3DEntity *v3d = new Vertex3DEntity(this, v);
        m_vertices.push_back(v3d);
        qDebug() << v3d;
    }
}

void Root3DEntity::clearGeometries()
{
    for(auto e : m_edges) {
        delete e;
        e = nullptr;
    }
    m_edges.clear();

    for(auto v : m_vertices) {
        delete v;
        v = nullptr;
    }
    m_vertices.clear();
}

void Root3DEntity::updateGeometries(GiMatrix *matrix)
{
    auto nullSpRepr = matrix->getNullspaceRepr();
    for (int i = 0; i<= nullSpRepr.size(); i++) {
        m_vertices[i]->updateGeometry(nullSpRepr[i]);
    }
}
