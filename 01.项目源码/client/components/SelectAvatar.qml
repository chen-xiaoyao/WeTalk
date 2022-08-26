import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Window {
    id:selectAvatar
    width: 350
    height:260
    visible: false


    ColumnLayout
    {
        anchors.fill: parent
        Layout.margins: 15
        Layout.alignment: Qt.AlignCenter

        RowLayout
        {
            Layout.fillWidth: true
            spacing: 20
            Rectangle
            {
                width: 100
                height: 100
                Image {
                    id: img1
                    source: "https://c-ssl.dtstatic.com/uploads/item/201403/08/20140308212425_n8JsB.thumb.1000_0.jpeg"
                    anchors.fill: parent
                    MouseArea
                    {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked:
                        {
                            personalHead = img1.source
                            selectAvatar.visible = false
                        }
                    }
                }
            }
            Rectangle
            {
                width: 100
                height: 100
                Image {
                    id: img2
                    source: "https://c-ssl.duitang.com/uploads/blog/201408/15/20140815095903_ttcnF.jpeg"
                    anchors.fill: parent
                    MouseArea
                    {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked:
                        {
                            personalHead = img2.source
                            selectAvatar.visible = false
                        }
                    }
                }
            }
            Rectangle
            {
                width: 100
                height: 100
                Image {
                    id: img3
                    source: "https://c-ssl.dtstatic.com/uploads/item/201403/08/20140308212431_eFs8Y.thumb.1000_0.jpeg"
                    anchors.fill: parent
                    MouseArea
                    {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked:
                        {
                            personalHead = img3.source
                            selectAvatar.visible = false
                        }
                    }
                }
            }


        }

        RowLayout
        {
            Layout.fillWidth: true
            spacing: 20
            Rectangle
            {
                width: 100
                height: 100
                Image {
                    id: img4
                    source: "https://c-ssl.dtstatic.com/uploads/item/201403/08/20140308212528_ymkrF.thumb.1000_0.jpeg"
                    anchors.fill: parent
                    MouseArea
                    {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked:
                        {
                            personalHead = img4.source
                            selectAvatar.visible = false
                        }
                    }
                }
            }
            Rectangle
            {
                width: 100
                height: 100
                Image {
                    id: img5
                    source: "https://c-ssl.dtstatic.com/uploads/blog/201408/14/20140814181135_wNBzN.thumb.1000_0.jpeg"
                    anchors.fill: parent
                    MouseArea
                    {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked:
                        {
                            personalHead = img5.source
                            selectAvatar.visible = false
                        }
                    }
                }
            }
            Rectangle
            {
                width: 100
                height: 100
                Image {
                    id: img6
                    source: "https://c-ssl.dtstatic.com/uploads/item/201403/08/20140308212511_LYudw.thumb.1000_0.jpeg"
                    anchors.fill: parent
                    MouseArea
                    {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        onClicked:
                        {
                            personalHead = img6.source
                            selectAvatar.visible = false
                        }
                    }
                }
            }


        }
    }
}
