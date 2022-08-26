import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3

Rectangle
{

    width:parent.width

    MessageDialog
    {//不可搜索自身提示
            id: cantSearchMyself
            title: "提示"
            icon: StandardIcon.Warning
            text: "不可搜索自身"
            standardButtons: StandardButton.Cancel
    }

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
        Button
        {
            id: addFriendButton
            width: 20
            height: 20
            background: Item{
                opacity:1
            }
            Layout.leftMargin: 70
            icon.source: addFriendButton.pressed? "qrc:/images/addFriend1.png":
                                 addFriendButton.hovered? "qrc:/images/addFriend1.png" :
                                                ("qrc:/images/addFriend.png")
            icon.color: "transparent"
            onClicked:
            {
                // 不可搜索自身id
                if(ClientInfo.id == userid)
                {
                    cantSearchMyself.open()
                    return
                }

                //发出好友（群聊）添加信号
                addSignal(userid);
            }
        }
    }
}
