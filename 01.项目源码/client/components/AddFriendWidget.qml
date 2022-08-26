import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.0
import "../"

Rectangle
{
    width: 40
    height: 40
    radius: width/2

    AddFriendWindow{id:addFriendWindow; visible: false}

    Image {
        id: addImg
        source: "../images/add.png"
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("添加好友");
                addFriendWindow.show()
            }
            hoverEnabled: true
            onHoveredChanged:
            {
                cursorShape = Qt.PointingHandCursor
            }
        }
    }
    ColorOverlay {
        anchors.fill: addImg
        source: addImg
        color: Material.primary
    }
}
