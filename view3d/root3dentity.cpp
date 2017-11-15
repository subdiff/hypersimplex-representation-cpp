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

#include <QDebug>

#include <QRenderSettings>
#include <QForwardRenderer>
#include <QInputSettings>

#include "qorbitcameracontroller.h"

Root3DEntity::Root3DEntity(QNode *parent)
    : Qt3DCore::QEntity(parent)
{
    // Material
//    Qt3DRender::QMaterial *material = new Qt3DExtras::QPhongMaterial(this);

    // Camera
    Qt3DRender::QCamera *camera = new Qt3DRender::QCamera(this);
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
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


    // Cylinder shape data
    Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
    cylinder->setRadius(1);
    cylinder->setLength(10);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    // CylinderMesh Transform
    Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform();
//    cylinderTransform->setScale(1.5f);
    cylinderTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
    cylinderTransform->setTranslation(QVector3D(-5.0f, 4.0f, -1.5));

    Qt3DExtras::QPhongMaterial *cylinderMaterial = new Qt3DExtras::QPhongMaterial();
    cylinderMaterial->setDiffuse(QColor(QRgb(0x928327)));

    // Cylinder
    Qt3DCore::QEntity *cylinderEntity = new Qt3DCore::QEntity(this);
    cylinderEntity->addComponent(cylinder);
    cylinderEntity->addComponent(cylinderMaterial);
    cylinderEntity->addComponent(cylinderTransform);
}
