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

#include <QGuiApplication>
#include <QQuickView>

#include "hypersimplex.h"
#include "backend.h"
#include "view3d/root3dwrapper.h"
#include "view3d/root3dentity.h"

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("applications-education-mathematics")));

//    s_createHypersimplex(4,2);

    qmlRegisterType<BackEnd>("subdiff.de.math.hypersimplex.representation", 1, 0, "BackEnd");
    qmlRegisterType<Root3DWrapper>("subdiff.de.math.hypersimplex.representation", 1, 0, "Root3DWrapper");
    qmlRegisterType<Root3DEntity>("subdiff.de.math.hypersimplex.representation", 1, 0, "Root3DEntity");

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/gui/Application.qml"));
    view.show();

    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));

//    return 0;

    return app.exec();
}
