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

int main(int argc, char** argv)
{
    QGuiApplication app(argc, argv);
//    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("plasmadiscover")));
//    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    Hypersimplex *test_hyper = createHypersimplex(3,1);
    delete test_hyper;

//    Hypersimplex *test_hyper2 = createHypersimplex(5,2);
//    delete test_hyper2;

    QQuickView view;
    view.setSource(QUrl("qrc:/gui/Application.qml"));
    view.show();

    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));

//    delete test_hyper;
//    delete test_hyper2;

    return 0;

    return app.exec();
}
