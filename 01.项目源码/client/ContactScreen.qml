import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3

import "./components"

Rectangle {
    id: rectangle
    radius: 4
    objectName: "contactScreen"

    // 信号，qml用，点击和某人/群聊天触发，参数为对应项的data
    signal messageWithConact(int id, string name, string avatar)
    signal messageWithGroup(int id, string name, string avatar)

    // 信号，C++用，打开通讯录页面时获取联系人列表和群组列表
    signal requestContactSignal()
    signal requestGroupSignal()

    //创建群聊信号
    signal createGroupSignal(var mId)

    // 获取到联系人后调这个函数，messages为QJsonArray。每一项的数据data参考如下
    /**
      userId: 1
      userName: "张三"
      avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
    */
    function setContacts(contacts){
        contactListModel.clear()
        for(let each of contacts){
            contactListModel.append(each)
        }
    }

    // 添加一条好友信息
    function appendContact(data){
        contactListModel.append(data)
    }

    // 设置群列表
    function setGroups(groups){
        groupListModel.clear()
        for(let each of groups){
            groupListModel.append(each)
        }
    }

    // 添加一条群组信息
    function appendGroup(data){
        groupListModel.append(data)
    }


    // 下面是qml用的
    function sendRequestDataSignal(){
        console.log("获取通讯录信息……")
        requestContactSignal()
        requestGroupSignal()
    }

    AddFriendWindow{
        id : addFriendWindow; visible: false;
        onPassSignal: (uid, avatar, name, isPerson) => {
            if(isPerson) appendContact({"userId":uid,"userName":name,"avatar":avatar})
            else appendGroup({"userId":uid,"userName":name,"avatar":avatar})
        }
    }
    CreateGroup{
        id:createGroup;
        visible: false;
        objectName: "createGroup"
    }

    /*
      获取好友/群聊信息=>信息界面
      */

    PersonalInf{id: personalInf; visible: false}
    GroupInf{id:groupInf; visible:false}

    //向服务器发送获取个人或群聊信息的信号
    signal infSignal(int uid, bool isGroup)

    //接收返回的个人信息信号
    function personalInfBack(data)
    {

        personalInf.personalSaying = data.signature
        personalInf.sex_num = Number(data.gender)
        personalInf.birthday = data.birth
        personalInf.areaFrom = data.area
        personalInf.edu = data.education
        personalInf.personalHead = data.icon
        personalInf.personalName = data.name
        personalInf.personalID = data.id

        personalInf.visible = true
    }
    function openPInfoWindow(data,){
        //获得用户信息
        console.log("emit")
        infSignal(data["userId"], false)

        personalInf.isMe = false
        personalInf.personalHead = data["avatar"]
        personalInf.personalName = data["userName"]
        personalInf.personalID   = data["userId"]

//        personalInf.visible = true // 与服务器通讯后注释
    }

    //接收返回的群组信息信号
    function groupInfBack(data)
    {
        groupInf.groupNotic = data.groupNotice //群公告
        groupInf.groupSummary = data.groupSummary //群简介
        groupInf.isOwner = data.isOwner

        //groupInf.visible = true


    }


    function openGInfoWindow(data){
        //获得群组信息
        infSignal(data["userId"],true)

        groupInf.groupHead = data["avatar"]
        groupInf.groupName = data["userName"]
        groupInf.groupID   = data["userId"]

        groupInf.visible = true //与服务器通讯后注释
    }

    function createGroupIsSuccess(v)
    {
        (0 != v)? (createGroupSuccess.open()):(createGroupError.open())
    }

    MessageDialog
    {//创建群聊成功提示
            id: createGroupSuccess
            title: "提示"
            icon: StandardIcon.Warning
            text: "创建群聊成功"
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

    /*
      ui界面
      */
    ListModel {
        id: contactListModel
        ListElement {
            userId: 100001
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 100001
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: false
        }
    }

    ListModel {
        id: groupListModel
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: true
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: true
        }
        ListElement {
            userId: 1
            userName: "张三"
            avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
            isGroup: true
        }
    }

    Component {
        id: contactListItem
        RowLayout
        {

            width: 240
            height: 64
            RoundImage
            {
                img_src: avatar
                width: 40
                height: width
                color: "black"
                Layout.alignment: Qt.AlignLeft

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onHoveredChanged:
                    {
                        cursorShape = Qt.PointingHandCursor
                    }
                    acceptedButtons: Qt.LeftButton
                    onClicked:
                    {
                        if(!isGroup&&Number(userId)<600000)
                        {
                            openPInfoWindow(contactListModel.get(index))
                        }
                        else
                        {
                            openGInfoWindow(groupListModel.get(index))
                        }
                    }

                }
            }
            Text
            {
                Layout.leftMargin: 5
                text: userName + qsTr("<font color=\"#54b4ef\">(") + userId + qsTr(")</font>")
                elide: Text.ElideRight
                Layout.alignment: Qt.AlignLeft
            }
            Button
            {
                id: contactButton
                width: 20
                height: 20
                background: Item{
                    opacity:1
                }
                onClicked: {

                    if(100000 <= userId && userId <= 599999){
                        messageWithConact(userId, userName, avatar);
                    }else messageWithGroup(userId, userName, avatar);
                }

                Layout.alignment: Qt.AlignRight
                icon.source: "./images/icon_chat.png"
                icon.color: contactButton.hovered ? Material.accent : "transparent"
            }
        }

    }


    // 好友群聊显示界面
    ColumnLayout {
        anchors.margins: 12
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 400
        spacing: 0
        anchors.left: parent.left
        anchors.leftMargin: 12

        Label {
            text: "好友"
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.leftMargin: 4
            Layout.bottomMargin: 4
        }

        ListView {
            id: contactListView
            model: contactListModel
            spacing: 8
            focus: true
            Layout.fillWidth: true
            height: Math.min(contactListView.contentHeight, 280)
            //            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            clip: true
            delegate: contactListItem
        }

        Label {
            text: "群聊"
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            Layout.leftMargin: 4
            Layout.bottomMargin: 4
        }

        ListView {
            id: groupListView
            model: groupListModel
            spacing: 8
            focus: true
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignLeft | Qt.AlignTop
            clip: true
            delegate: contactListItem

        }
        RowLayout
        {
            Button {// 添加好友
                height: 56; width: 56
                background: Rectangle {
                    color: "transparent"
                }
                icon.source: "../images/addFriend.png"
                icon.width: 32
                icon.height: 32

                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    addFriendWindow.show()
                }
            }
            Button {// 创建群聊
                height: 56; width: 56
                background: Rectangle {
                    color: "transparent"
                }
                icon.source: "../images/addGroup.png"
                icon.width: 25
                icon.height: 25

                Layout.alignment: Qt.AlignLeft
                onClicked: {
//                    createGroup.show()
                    createGroupSignal(1)
                }
            }
        }



    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
