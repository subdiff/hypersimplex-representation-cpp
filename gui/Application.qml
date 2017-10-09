import QtQuick 2.3
import QtQuick.Controls 1.4

Item {
    width: 1000
    height: 500

    Rectangle {
        id: rect
        anchors.centerIn: parent
        width: parent.width / 2
        height: parent.height/2
        color: "yellow"

        MouseArea {
            anchors.fill: parent
            onClicked: Qt.quit()
        }
    }

    Row {
        anchors.centerIn: parent
        spacing: 10
        Label {
            text: "d:"
        }
        SpinBox {
            id: dSpin
            minimumValue: 4
            maximumValue: 99
        }
        Label {
            text: "k:"
        }
        SpinBox {
            id: kSpin
            minimumValue: 1
            maximumValue: dSpin.value - 1
        }
    }
}
