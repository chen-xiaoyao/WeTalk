import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3

Rectangle {

    radius: 4

    // 信号，点击item触发，参数为对应项的data
    // data 的数据参考同下

    signal clickHistoryMessageItem(var data)
    signal refreshChatListSignal(int targetId)

    // 获取到聊天记录后调这个函数，messages为json数组。每一项的数据data参考如下
    /**
      userId: 1
      userName: "张三"
      avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
      latestTime: 1661053691
      latestMessage: "你说咱们要不就继续加油吧"
    */

    function setMessages(messages){
        historyMessageListModel.clear()
        for(each of messages){
            historyMessageListModel.append(each)
        }
    }

    function setMessageWithUid(uid, message, time){
        for(let i =0; i < historyMessageListModel.count; i++){
            let data = historyMessageListModel.get(i);
            if(data.userId === uid){
                data["latestTime"] = time
                data["latestMessage"] = message
                historyMessageListModel.set(i, data);
                return
            }
        }
    }

    function startChatWith(uid, name, avatar){
        for(let i =0; i < historyMessageListModel.count; i++){
            if(historyMessageListModel.get(i).userId === uid){
                historyMessageListView.currentIndex = i
                clickHistoryMessageItem(historyMessageListModel.get(i))
                refreshChatListSignal(historyMessageListModel.get(i).userId)
                console.log(`找到了和${uid}的历史聊天记录，位于${i}`)
                return
            }
        }
        console.log(`没找到和${uid}的历史聊天记录，新建一条`);
        historyMessageListModel.insert(0, {
                                           "userId": uid,
                                           "userName": name,
                                           "avatar": avatar,
                                           "latestTime": Date.parse(new Date())/ 1000,
                                           "latestMessage": ""
                                       })
        historyMessageListView.currentIndex = 0
        clickHistoryMessageItem(historyMessageListModel.get(0))
        refreshChatListSignal(historyMessageListModel.get(0).userId)
    }

    ListModel {
        id: historyMessageListModel
    }

    ListView {
        id: historyMessageListView
        model: historyMessageListModel
        anchors.fill: parent
        spacing: 8
        focus: true
        anchors.margins: 8

        delegate: HistoryMessageListItem{
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    historyMessageListView.currentIndex = index
                    clickHistoryMessageItem(historyMessageListModel.get(index))
                    refreshChatListSignal(historyMessageListModel.get(index).userId)
                }
            }
        }
    }
}


