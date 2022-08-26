import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import QtQml 2.0
import "./components"

Window {
    id: addFriendWindow
    width: 300
    height: 120
    visible: false
    title: qsTr("加好友/群")
    objectName: "addFriendWindow"

    flags:(Qt.Window | Qt.CustomizeWindowHint)


    property string avatarImg: "https://c-ssl.dtstatic.com/uploads/blog/202203/25/20220325232426_17909.thumb.1000_0.jpeg"
    property string userName: "balabala"
    property int userid: 12345
    property bool isPerson: true //用户：true 群聊：false
    property bool finded: false // 是否查找到相应的用户/群聊

    //发送搜索内容（id）
    signal searchSignal(int idN)
    //发送添加信号
    signal addSignal(int idN)
    //已添加新好友（群聊）信号
    signal passSignal(int idN,string usrAvatar,string usrName,bool isPerson)

    //返回搜索结果
    function searchBack(data)
    {
        finded = data.finded
        if(false == finded)
        {
            findFalse.open()
            return;
        }
        userName = data.pName
        avatarImg = data.headImg
        isPerson = data.isPerson
        addFriendWindow.height = 180
        searchResult.visible = true
    }
    //返回添加成功信号
    function addBack(v)
    {
        if(0 == v)
        {//添加失败
            addFalse.open()
        }
        else
        {
            //这里应该把好友/群聊添加到通讯录
            passSignal(userid,avatarImg,userName,isPerson)
            addOk.open()
        }
    }

    MessageDialog
    {
        id:findFalse
        standardButtons: MessageDialog.Cancel
        text: "查无该用户/群聊"
    }
    MessageDialog
    {
        id:addFalse
        standardButtons: MessageDialog.Cancel
        text: "添加失败"
    }
    MessageDialog
    {
        id:addOk
        standardButtons: MessageDialog.Ok
        text: "添加成功"
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
                        addFriendWindow.visible = false
                    }
                    hoverEnabled: true
                    onHoveredChanged: cursorShape = Qt.PointingHandCursor
                }
            }
        }

    // 搜索栏

    SearchWidget
    {
        id: searchWidget
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 40

    }
    Rectangle
    {
        id:searchResult
        visible: false
        width: parent.width
        height: parent.height
        anchors.top: searchWidget.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 8

            Text {
                id: pInf
                text: qsTr("<font color='#767777'>联系人</font>")
                visible: isPerson
            }

            Text {
                id: gInf
                anchors.top: pInfList.bottom
                anchors.topMargin: 0
                text: qsTr("<font color='#767777'>群组</font>")
                visible: !isPerson
            }
            Rectangle
            {
                id: pInfList
                anchors.top: pInf.bottom
                width: parent.width
                height: parent.height*0.4

                //好友搜索结果表
                InfListItem
                {
                    id:resultList
                    width: parent.width
                }
            }
    }

}























