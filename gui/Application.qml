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

import subdiff.de.math.hypersimplex.representation 1.0

Item {
    width: 1000
    height: 500

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
                enabled: backend.ready && (dSpin.value != curD || kSpin.value != curK);
                onClicked: initHypers(dSpin.value, kSpin.value)
            }
            Button {
                id: resetButton
                text: "Reset"
                enabled: (dSpin.value != curD || kSpin.value != curK) && curD != 0;
                onClicked: {
                    dSpin.value = curD;
                    kSpin.value = curK;
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
}
