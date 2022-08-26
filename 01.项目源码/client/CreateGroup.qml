import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import QtQml 2.0

import "./components"

Window {
    id: createGroupWindow
    width: 300
    height: 200
    visible: false

    title: qsTr("创建群聊")

    flags:(Qt.Window | Qt.CustomizeWindowHint)


    property string avatarImg: "https://c-ssl.dtstatic.com/uploads/blog/202203/25/20220325232426_17909.thumb.1000_0.jpeg"
    property string userName: "XXX"
    property int userid: 12345
    property var memberId: []
    property string printStr : ""

    //创建群聊信号
    signal createGroupSignal(var mId)


    //返回生成的群聊号和拉群成功指令
    function createGroupBack(groupID)
    {
        if(0 == groupID)
        {
            createGroupError.open()
        }
        else
        {
            createGroupSuccess.open()
        }
    }



    function printMember()
    {
        printStr = "确认拉取成员创建群聊？\n"
        return printStr
    }

    MessageDialog
    {//拉群人数少于3
            id: lessThanThree
            title: "提示"
            icon: StandardIcon.Warning
            text: "3人以下无法建群"
            standardButtons: StandardButton.Cancel
    }
    MessageDialog
    {//创建群聊失败提示
            id: createGroupError
            title: "提示"
            icon: StandardIcon.Warning
            text: "创建群聊失败"
            standardButtons: StandardButton.Cancel
    }
    MessageDialog
    {//创建群聊成功提示
            id: createGroupSuccess
            title: "提示"
            icon: StandardIcon.Warning
            text: "创建群聊成功"
            standardButtons: StandardButton.Cancel
    }

    Dialog
    {//添加该成员提示
        id: idDialog
        width: 200;
        height: 100
        title: qsTr("添加成员确认")
        standardButtons: StandardButton.Ok|StandardButton.Cancel

        Text {
            id: text
            font.pixelSize: 15
            text: printMember()
            anchors.centerIn: parent
        }

        onAccepted:
        {
            console.log("create")
            createGroupSignal(memberId)
        }
    }


    Rectangle{
            id:closeButton
            width:10
            height:10
            radius: closeButton.width/2
            anchors.top: parent.top
            anchors.right: parent.right

            Rectangle{
                width:15
                height:15
                radius: 15/2
                anchors{
                    right:closeButton.right
                    top:closeButton.top
                }
                Text {
                    id: name
                    anchors.centerIn: parent
                    text: qsTr("X")
                    color:"#FFFFFF"
                    font.pixelSize: 16
                }
                color:"green"
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        createGroupWindow.visible = false
                    }
                    hoverEnabled: true
                    onHoveredChanged: cursorShape = Qt.PointingHandCursor
                }
            }
        }


    // 搜索栏

    GroupAddWidget
    {
        id: searchWidget
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20   
        onIdpp:
        {
            userid = idN
            searchResult.visible = true
        }

    }
    Rectangle
    {
        z:1
        id:searchResult
        width: parent.width
        height: parent.height
        anchors.top: searchWidget.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 8

        visible: false

        Text {
            id: pInf
            text: qsTr("<font color='#767777'>联系人</font>")
        }
        Rectangle
        {
            z:1
            id: pInfList
            anchors.top: pInf.bottom
            anchors.topMargin: 10
            width: parent.width
            height: parent.height*0.4

            //好友搜索结果表
            Rectangle
            {
                width:parent.width
                RowLayout
                {
                    Layout.fillWidth: true
                    RoundImage
                    {
                        id: avatarImgP
                        img_src: avatarImg

                        width: 40
                        height: avatarImgP.width
                        color: "black"
                    }
                    Text
                    {
                        id: searchName
                        Layout.leftMargin: 20
                        text: userName + qsTr("<font color=\"#54b4ef\">(") + userid + qsTr(")</font>")
                    }
                    Rectangle
                    {
                        z:100
                        width: 20
                        height: 20
                        Layout.leftMargin: 80
                        Image
                        {
                            id:addMember
                            anchors.fill: parent
                            source: "qrc:/images/addGroup.png"
                            MouseArea
                            {

                                anchors.fill: parent
                                hoverEnabled: true
                                onHoveredChanged: cursorShape = Qt.PointingHandCursor
                                acceptedButtons: Qt.LeftButton
                                onClicked:
                                {
                                    if(userid != memberId[memberId.length-1])
                                    {
                                        //发出创建群聊添加成员的信号
                                        memberId.push(userid)
                                        console.log("addFriend")
                                        console.log(memberId)

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    Rectangle
    {
        z:100
        width: 100
        height: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
        Button
        {
            anchors.fill: parent
            text: "拉取群聊"

            onClicked:
            {
                if(memberId.length < 2)
                {
                    lessThanThree.open()
                }
                else
                {
                    idDialog.open()
                    console.log("拉群成功")
                    createGroup.close()
                }
            }
        }
    }


}























