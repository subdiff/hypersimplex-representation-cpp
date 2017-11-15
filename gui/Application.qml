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

import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3 as Layouts

import QtQuick.Scene3D 2.0
//import QtCanvas3D 1.1

import subdiff.de.math.hypersimplex.representation 1.0

Layouts.RowLayout {

    spacing: 50
    Item {
        id: ctrls
        width: childrenRect.width//1000
        height: childrenRect.height//500

        property int curD: 0
        property int curK: 0

        function initHypers(d, k) {
            curD = d;
            curK = k;
            backend.getHypersimplex(d, k);
        }

        BackEnd {
            id: backend

            onSelectedSubgroupChanged: subgroupSelector.currentIndex = selectedSubgroup;
        }

        Row {
            id: hypersSelector
            anchors {
                left: parent.left
                leftMargin: 10
            }
            spacing: 10

            Row {
                id: parametersRow
                spacing: 10

                anchors.verticalCenter: controlButtonsCol.verticalCenter

                Label {
                    text: "d:"
                }
                SpinBox {
                    id: dSpin
                    width: applyButton.width / 2
                    minimumValue: 3
                    maximumValue: 20
                }
                Label {
                    text: "k:"
                }
                SpinBox {
                    id: kSpin
                    width: dSpin.width
                    minimumValue: 1
                    maximumValue: dSpin.value - 1
                }
            }
            Column {
                id: controlButtonsCol
                spacing: 10
                anchors {
                    top: parent.top
                    topMargin: 10
                }

                Button {
                    id: applyButton
                    text: "Apply"
                    enabled: backend.ready && (dSpin.value != ctrls.curD || kSpin.value != ctrls.curK);
                    onClicked: ctrls.initHypers(dSpin.value, kSpin.value)
                }
                Button {
                    id: resetButton
                    text: "Reset"
                    enabled: (dSpin.value != ctrls.curD || kSpin.value != ctrls.curK) && ctrls.curD != 0;
                    onClicked: {
                        dSpin.value = ctrls.curD;
                        kSpin.value = ctrls.curK;
                    }
                }
            }
        }

        ComboBox {
            id: subgroupSelector
            anchors {
                top: hypersSelector.bottom
                left: hypersSelector.left
                right: hypersSelector.right
                topMargin: 20
            }
            enabled: count > 1
            model: backend.vtxTrSubgroups

            currentIndex: backend.selectedSubgroup
            onCurrentIndexChanged: backend.selectedSubgroup = currentIndex;
        }

        TextArea {
            id: logOutput
            anchors {
                top: subgroupSelector.bottom
                left: hypersSelector.left
                right: hypersSelector.right
                topMargin: 10
            }
            readOnly: true
            selectByKeyboard: true

            text: "TEST"
        }

        Column {
            id: sliders
            anchors {
                top: logOutput.bottom
                left: logOutput.left
                right: logOutput.right
                topMargin: 10
            }
            Slider {
                width: parent.width

            }
        }
    }

    Rectangle {
        width: 600
        height: 400
        color: "yellow"

//        Canvas3D {
//            id: canvas3d
//            width: 500
//            height: 200

//            focus: true
//            property var gl

//            onInitializeGL: {
//                gl = canvas3d.getContext("experimental-webgl");

//                // Setup clear color to be a random color
//                gl.clearColor(Math.random(), Math.random(), Math.random(), 1.0);

//                // Setup viewport
//                gl.viewport(0, 0, canvas3d.width * canvas3d.devicePixelRatio, canvas3d.height * canvas3d.devicePixelRatio);
//            }

//            onPaintGL: {
//                // Clear background to current clear color
//                gl.clear(gl.COLOR_BUFFER_BIT);
//            }

//            onResizeGL: {
//                var pixelRatio = canvas3d.devicePixelRatio;
//                canvas3d.pixelSize = Qt.size(canvas3d.width * pixelRatio, canvas3d.height * pixelRatio);
//                if (gl)
//                    gl.viewport(0, 0, canvas3d.width * canvas3d.devicePixelRatio, canvas3d.height * canvas3d.devicePixelRatio);
//            }
//        }

        Rectangle {
            id: scene
            anchors.fill: parent
            anchors.margins: 50
//            color: "darkRed"

            Scene3D {
                id: scene3d
                anchors.fill: parent
                anchors.margins: 10
                focus: true
                aspects: ["input", "logic"]
                cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

                Entity3D {}
            }
        }
    }
}
