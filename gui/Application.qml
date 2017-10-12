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
    }

    Rectangle {
        id: rect
        anchors.centerIn: parent
        width: parent.width / 2
        height: parent.height/2
        color: "yellow"

    Row {
        anchors.centerIn: parent
        spacing: 10
        Label {
            text: "d:"
        }
        SpinBox {
            id: dSpin
            minimumValue: 3
            maximumValue: 20
        }
        Label {
            text: "k:"
        }
        SpinBox {
            id: kSpin
            minimumValue: 1
            maximumValue: dSpin.value - 1
        }
        Button {
            text: "Apply"
            enabled: backend.ready && (dSpin.value != curD || kSpin.value != curK);
            onClicked: initHypers(dSpin.value, kSpin.value)
        }
    }
    }
}
