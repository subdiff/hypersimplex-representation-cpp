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

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3 as Layouts
import QtQuick.Scene3D 2.0

import subdiff.de.math.hypersimplex.representation 1.0

Layouts.RowLayout {

    Column {
        id: ctrls

        Layouts.Layout.alignment: Qt.AlignTop

        spacing: 10
        padding: 10

        property int curD: 0
        property int curK: 0

        function initHypers(d, k) {
            curD = d;
            curK = k;
            backend.createHypersimplex(d, k);
        }

        Row {
            spacing: 10
            padding: 10

            anchors.horizontalCenter: parent.horizontalCenter

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

            width: eigenVectorSelectionCol.width

            enabled: count > 1
            model: backend.vtxTrSubgroups

            currentIndex: backend.selectedSubgroup
            onCurrentIndexChanged: backend.selectedSubgroup = currentIndex;
        }

        GroupBox {
            id: eigenVectorSelectionCol

            enabled: backend.ready

            title: "Eigenvectors by:"

            property int mode: 0


            Column {
                height: childrenRect.height
                width: childrenRect.width
                spacing: 10

                ExclusiveGroup { id: eigenVectorSelectionGroup }

                RadioButton {
                    id: eigenVectorSelectionPerMultiplicity
                    checked: true
                    exclusiveGroup: eigenVectorSelectionGroup
                    text: "Similar multiplicity"
                    onCheckedChanged: {
                        if (checked) {
                            eigenVectorSelectionCol.mode = 0;
                        }
                    }
                }

                RadioButton {
                    id: eigenVectorSelection2ndHighestAndBelow
                    exclusiveGroup: eigenVectorSelectionGroup
                    text: "2nd highest Eigenvalue and below"
                    onCheckedChanged: {
                        if (checked) {
                            eigenVectorSelectionCol.mode = 1;
                        }
                    }
                }

                RadioButton {
                    id: eigenVectorSelection2ndHighestOnly
                    exclusiveGroup: eigenVectorSelectionGroup
                    text: "2nd highest Eigenvalue only"
                    onCheckedChanged: {
                        if (checked) {
                            eigenVectorSelectionCol.mode = 2;
                        }
                    }
                }
            }
        }

        SliderRepeater {
            width: eigenVectorSelectionCol.width
        }
    }

    Item {
        Layouts.Layout.minimumWidth: 1000
        Layouts.Layout.minimumHeight: 800
        Layouts.Layout.fillHeight: true
        Layouts.Layout.fillWidth: true

        Column {
            anchors.fill: parent

            spacing: 10
            anchors.margins: 10

            Row {
                id: ctrls3D
                anchors.horizontalCenter: parent.horizontalCenter
                height: childrenRect.height

                spacing: 20

                Row {
                    height: childrenRect.height
                    spacing: 10

                    SpinBox {
                        id: projFacetSpin
                        width: dSpin.width
                        enabled: ctrls.curD == 5
                        value: 1
                        minimumValue: 1
                        maximumValue: ctrls.curD
                    }
                    Label {
                        enabled: projFacetSpin.enabled
                        text: "Projection facet"
                    }
                }

                Row {
                    height: childrenRect.height
                    spacing: 10

                    property bool singleVertexFacet: ctrls.curK == 1 || ctrls.curK == ctrls.curD - 1
                    CheckBox {
                        id: projToLargerFacetCheckbox
                        enabled: projFacetSpin.enabled && !parent.singleVertexFacet
                        checked: true
                    }
                    Label {
                        enabled: projFacetSpin.enabled
                        text: "Project to larger facet"
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: parent.height - ctrls3D.height
                color: "yellow"

                Scene3D {
                    id: scene3D
                    anchors.fill: parent
                    focus: true
                    aspects: ["input", "logic"]
                    cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

                    Root3DEntity {
                        id: root3D
                    }
                }
            }
        }
    }

    BackEnd {
        id: backend

        selEigenvectMode: eigenVectorSelectionCol.mode
        onGeometryInitNeeded: wrap3D.initGeometries()
        onGeometryUpdateNeeded: wrap3D.updateGeometries()
    }

    Root3DWrapper {
        id: wrap3D
        root3DPtr: root3D
        backEnd: backend
        projToLargerFacet: projToLargerFacetCheckbox.checked
        projFacet: projFacetSpin.value - 1
    }
}
