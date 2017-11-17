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

import subdiff.de.math.hypersimplex.representation 1.0

Layouts.RowLayout {

    spacing: 50
    Item {
        id: ctrls
        width: childrenRect.width//1000
        height: childrenRect.height//500

        Layouts.Layout.alignment: Qt.AlignTop

        property int curD: 0
        property int curK: 0

        function initHypers(d, k) {
            curD = d;
            curK = k;
            backend.getHypersimplex(d, k);
        }

        BackEnd {
            id: backend
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
            id: slidersCol
            anchors {
                top: logOutput.bottom
                left: logOutput.left
                right: logOutput.right
                topMargin: 10
            }
            spacing: 5

            Repeater {
                id: slidersRepeater

                model: 4//backend.eecCount    //TODO test
                delegate: VarSlider{}

                function distrVals(changedIndex) {
                    if (model < 2) {
                        return;
                    }
                    var chItem = itemAt(changedIndex);
                    console.log("Changed val:", chItem.oldVal, "to", chItem.val);

                    var diff = (chItem.val - chItem.oldVal) * chItem.mult;
                    console.log("diff:", diff, chItem.val, chItem.oldVal, chItem.mult);

                    chItem.oldVal = chItem.val;

                    var avails = [];
                    for (var i = 0; i < model; i++) {
                        var itm = itemAt(i);

                        if (itm == null) {
                            return;
                        }
                        if (i == changedIndex) {
                            continue;
                        }

                        var availVal = (diff > 0 ? itm.val : 1 - itm.val) * itm.mult;
                        console.log("availVal", + i + ":", availVal);

                        avails.push([i, availVal]);
                    }
//                    console.log("avails1:", avails);

                    avails.sort(function(a, b){return a[1] - b[1]});

                    fillOrDistr(avails, diff);
                }

                function fillOrDistr(avails, diff) {
                    console.log("avails:", avails);

                    if (!avails.length) {
                        return;
                    }
                    console.log("fillOrDistr:", avails[0][1], diff);

                    if (avails[0][1] >= Math.abs(diff) / avails.length) {
                        // enough available on all - just distribute the rest
                        distr(avails, diff);
                    } else {
                        fillFirst(avails, diff);
                    }
                }

                function distr(avails, diff) {
                    console.log("distr:", diff);
                    var diffDistr = diff / avails.length;
                    console.log("distr:", diffDistr);

                    for (var i = 0; i < avails.length; i++) {
                        var index = avails[i][0];
                        setItemVal(index, itemAt(index).val - diffDistr / itemAt(index).mult);
//                        setItemVal(index, (avails[i][1] - diffDistr) / itemAt(index).mult);
                    }
                }

                function fillFirst(avails, diff) {
                    console.log("fillFirst:", diff);
                    var newVal;
                    var mp = avails[0][1];

                    if (diff > 0) {
                        newVal = 0;
                        diff -= mp;
                    } else {
                        newVal = 1;
                        diff += mp;
                    }
                    setItemVal(avails[0][0], newVal);
                    avails.shift();
                    fillOrDistr(avails, diff);
                }

                function setItemVal(index, val) {
                    itemAt(index).oldVal = val;
                    itemAt(index).val = val;
                }
            }
        }
    }

    Rectangle {
        width: 1000
        height: 800
        color: "yellow"

            Scene3D {
                id: scene3D
                anchors.fill: parent
                anchors.margins: 10
                focus: true
                aspects: ["input", "logic"]
                cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

                Root3DEntity {
                    id: root3D
                }
            }
    }

    Root3DWrapper {
        id: wrap3D
        root3DPtr: root3D
        backEnd: backend
    }

    Button {
        text: "TEST INIT"
        onClicked: wrap3D.initGeometries()
    }
    Button {
        text: "TEST CLEAR"
        onClicked: wrap3D.clearGeometries()
    }
}
