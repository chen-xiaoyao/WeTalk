import QtQuick 2.12
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
Item {
    id: tabWidget

    // Setting the default property to stack.children means any child items
    // of the TabWidget are actually added to the 'stack' item's children.
    // See the "Property Binding"
    // documentation for details on default properties.
    default property alias content: stack.children
    //property list content

    property int current: 0

    property var icons: ["../images/icon_chat.png", "../images/icon_contact.png"]

    signal changeToContact()

    onCurrentChanged: stack.replace(null, content[current])


    Column {
        id: side
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        Repeater {
            model: icons.length
            delegate: Rectangle {
                height: 56; width: 56
                color: tabWidget.current == index ? "white" : "transparent"
                radius: 8

                Image {
                    id: img
                    source: icons[index]
                    width: 32
                    height: 32
                    anchors.centerIn: parent
                }

                ColorOverlay {
                    anchors.fill: img
                    source: img
                    color: tabWidget.current == index ? Material.primary : "black"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        tabWidget.current = index
                        if (1 == index)//联系人界面
                        {
                            changeToContact()
                        }
                    }
                }
            }
        }
    }

    StackView {
        id: stack
        height: tabWidget.height
        anchors.left: side.right; anchors.right: tabWidget.right
//        anchors.fill: parent
        anchors.leftMargin: 12
        initialItem: content[0]
    }

}
