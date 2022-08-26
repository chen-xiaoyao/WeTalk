import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import "./components/"

Window {
    id:mainWindowP
    visible: false
    width: 1000
    height: 600
    title: qsTr("WeTalk")
    color: "#edf5f9"
    objectName: "mainWindow"

    // 信号：开始请求历史消息，参数为我的UID
    signal requestHistoryMessage(int myUid)

    // 设置历史聊天记录，
    // 参数：json数组，每项格式如下
    /**
      userId: 1
      userName: "张三"
      avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
      latestTime: 1661053691 // 最后一条消息时间
      latestMessage: "你说咱们要不就继续加油吧" // 最后一条消息内容
    */

    function setHistoryMessage(messages){
        historyMessageScreen.setMessages(messages);
    }

    function startChatTo(uid, name, avatar){
        console.log(`开始和${name}聊天……`)
        historyMessageScreen.startChatWith(uid, name, avatar);
        tabWidget.current = 0
    }

    Component.onCompleted: {
        ClientInfo.id = "100001"
        ClientInfo.name = "Shen"
        ClientInfo.avatar = "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
        requestHistoryMessage(ClientInfo.id);
    }

    EmojiChoose{
        id: emojiChoose;
        onClickEmoji: (text)=>chatScreen.appendInputText(text)
    }

    VerticalTabWidget {
        id: tabWidget
        anchors.rightMargin: 12
        anchors.leftMargin: 12
        anchors.bottomMargin: 12
        anchors.topMargin: 12
        anchors.fill: parent
        objectName: "chatWindow1"
        onChangeToContact: contactScreen.sendRequestDataSignal()

        // 聊天页面
        RowLayout {
            id:rowLayout
            spacing: 12
//            anchors.fill: parent
            objectName: "chatWindow2"

            HistoryMessageScreen {
                id: historyMessageScreen
                Layout.fillHeight: true
                width: 270
                objectName: "historyMessageScreen"
                Component.onCompleted: {
                    clickHistoryMessageItem.connect((data)=>{
                        // 历史消息列表与聊天窗口的同步
                        chatScreen.setArg(data["userId"])
                    })
                }
            }

            ChatScreen {
                id: chatScreen
                Layout.fillHeight: true
                onUpdateHistorySignal: (id, message, time)=>historyMessageScreen.setMessageWithUid(id, message, time)
            }
        }

        // 联系人页面
        ContactScreen {
            // anchors.fill: parent
            id: contactScreen
            visible: false
            radius: 4
            onMessageWithConact: (uid, name, avatar)=>startChatTo(uid, name, avatar)
            onMessageWithGroup: (uid, name, avatar)=>startChatTo(uid, name, avatar)
        }


    }
}
