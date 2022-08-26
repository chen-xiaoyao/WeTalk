import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.0 as QtPlatform
import "./components"

ColumnLayout {

    spacing: 12
    anchors.margins: 24
    property int myUid: ClientInfo.id
    property string myName: ClientInfo.name
    property string myAvatar: ClientInfo.avatar
    property int targetId: 1

    property string targetName: "ll"
    property string targetAvatar: "https://tse2-mm.cn.bing.net/th/id/OIP-C.cS6phGwfJ3qgAtvSXv0rugAAAA?pid=ImgDet&rs=1"
    objectName: "chatScreen" //这句话用于C++抓取

    // 信号，当前端发送了一条信息时调用
    // targetId: 发给谁
    // message: 消息
    // time: 消息发送的 unix 时间戳（类似 1660893694）

    signal sendFileSignal(int targetId, string fileUrl, int time)
    signal sendMessageSignal(int targetId, string message, int time)

    // 拉取聊天记录（页面加载完后）
    signal requestMessageSignal(int sendId, int targetId)

    // 函数： 当c++层接收到新的消息时调用，往UI里添加一条消息(暂时不需要，或许直接appenddata就行)
    function sendChatMessageBack(sendId,content,time)
    {
        console.log(content)
    }

    // data 样例见下面的 chatListModel
    function appendData(data){
        if(data["groupId"] >= 600000){ // 群发
            if(data["groupId"] === targetId) {
                chatListModel.append(data)
            }
            updateHistorySignal(targetId, data["message"], data["time"]);
        }else{
            if(data["uid"] === targetId || data["uid"] === myUid) {
                chatListModel.append(data)
            }
            updateHistorySignal(targetId, data["message"], data["time"]);
        }
    }

    // 函数：设置消息列表。会替换当前所有内容为新内容
    function setMessages(messages){
        chatListModel.clear()
        for(let each of messages){
            chatListModel.append(each);
        }
    }

    // 下面的方法是qml用的
    signal updateHistorySignal(int targetId, string message, int time)

    // 设置聊天对象
    function setArg(uid){
        console.log("targetId:" + uid);
        targetId = uid;
    }

    function appendInputText(text){
        messageField.append(text)
    }

    function chatWithUid(uid){
        for(i in chatListModel){
            if(chatListModel.get(i).uid === uid){
                chatListView.currentIndex = i
            }
        }
    }

    QtPlatform.FileDialog {
        id: openFileDialog
        title: "打开文件"
        nameFilters: ["All files(*.*)"]
        onAccepted: {
            try{
                var url = openFileDialog.file.toString()
                var time = Date.parse(new Date()) / 1000
                var fileName = url.substring(url.lastIndexOf("/"))
                var data = {
                    "uid": myUid,
                    "name": myName,
                    "time": time,

                    "fileName": fileName,
                    "fileSize": QFileUtils.getFileSize(url),
                    "localPath": url,
                    "message": "[文件]",

                    "avatar": myAvatar,
                    "type": 1 // 1代表文件
                }
                console.log(fileName + "  " + data["fileSize"])
                appendData(data)
                sendFileSignal(targetId, url, time);
                console.log("发送文件：" + url)
            }catch(e){
                console.trace("打开文件出错："+ e);
            }finally{

            }
        }
        onRejected: console.log("取消打开文件")
    }

    ListModel {
        id: chatListModel
    }


    Rectangle {
        radius: 4
        Layout.fillWidth: true
        Layout.fillHeight: true

        ListView {
            id: chatListView
            objectName: "chatListView"
            anchors.fill: parent
            anchors.margins: 16
            clip: true
            spacing: 8
            model: chatListModel
            delegate: ChatListItem{} // getWidget(type)
        }
    }


    Rectangle {
        id: rect
        radius: 4
        Layout.fillWidth: true
        Layout.preferredHeight: childrenRect.height
        Layout.alignment: Qt.AlignBottom
        anchors.margins: 16

        Item {
            id: imgContainer
            width : 100
            height: 20
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 12
            anchors.topMargin: 8
            RowLayout
            {
                Rectangle
                {
                    id: emojiImgArea
                    width : 20
                    height: 20
                    Image {
                        id: emojiImg
                        source: "./images/emoji.png"
                        anchors.fill: parent
                        MouseArea {
                            anchors.fill: emojiImg
                            onClicked: {
                                console.log("clickedEmoji")
                                emojiChoose.show()
                            }
                            hoverEnabled: true
                            onHoveredChanged: cursorShape = Qt.PointingHandCursor
                        }
                    }
                    ColorOverlay {
                        anchors.fill: emojiImg
                        source: emojiImg
                        color: Material.primary
                    }
                }
                Rectangle
                {
                    width : 20
                    height: 20
                    Layout.leftMargin: 5
                    Image {
                        id: folderImg
                        source: "./images/icon_folder.png"
                        anchors.fill: parent
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                openFileDialog.open();
                            }
                            hoverEnabled: true
                            onHoveredChanged: cursorShape = Qt.PointingHandCursor
                        }
                    }
                    ColorOverlay {
                        anchors.fill: folderImg
                        source: folderImg
                        color: Material.primary
                    }
                }
                Rectangle
                {
                    width : 22
                    height: 22
                    Layout.leftMargin: 5
                    Image {
                        id: pictureImg
                        source: "./images/picture.png"
                        anchors.fill: parent
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                openFileDialog.open()
                            }
                            hoverEnabled: true
                            onHoveredChanged: cursorShape = Qt.PointingHandCursor
                        }
                    }
                    ColorOverlay {
                        anchors.fill: pictureImg
                        source: pictureImg
                        color: Material.primary
                    }
                }

            }

        }

        RowLayout {

            width: parent.width
            height: 64
            anchors.top: imgContainer.bottom
            anchors.left: parent.left
            anchors.leftMargin: 12

            ScrollView {
                Layout.fillWidth: true
                TextArea {
                    id: messageField
                    width: maximumWidth
                    placeholderText: qsTr("请输入……")
                    wrapMode: TextArea.Wrap
                    focus: true
                    selectByMouse: true
                    background: null

                    Keys.onReturnPressed: {
                        sendButton.onClicked()
                    }
                }
            }



            Button {
                id: sendButton
                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 16
                text: `<font color="${sendButton.enabled?'white':'green'}">发送</font>`
                enabled: messageField.length > 0
                Material.background: "#90CAF9"
                //                Material.color: sendButton.enabled ? "white" : "lightgrey"
                flat: true
                onClicked: {
                    var message = messageField.text
                    if(message === "") return
                    var time = Date.parse(new Date())/ 1000
                    chatListModel.append({
                                             "uid": myUid,
                                             "name": myName,
                                             "time": Date.parse(new Date())/ 1000,
                                             "message": message,
                                             "avatar": myAvatar
                                         })
                    messageField.clear()
                    chatListView.currentIndex = chatListModel.count - 1;
                    sendMessageSignal(targetId, message, time)
                    updateHistorySignal(targetId, message, time)
                }
            }
        }
    }

    function isValidId(id){return 100000<=id&&id<=999999}

    //初始化测试
    Component.onCompleted: {
        console.log(`ClientInfo: id:${ClientInfo.id}`)
        chatListModel.append({
                                 "uid": 100002,
                                 "name": "李四",
                                 "time": 1660893694,
                                 "message": `{"fileName": "来来来.txt","fileSize": 1389420,"localPath": "d:/test/来来来.txt"}`,
                                 "avatar": "https://ts1.cn.mm.bing.net/th/id/R-C.1eed2de61a172c6ca2d79fc5ea62eb01?rik=c7W7KrSN7xFOIg&riu=http%3a%2f%2fimg.crcz.com%2fallimg%2f202003%2f10%2f1583821081100057.jpg&ehk=q%2f9lt0hQhwZzKFdRKYyG2g4zxQKgTWKJ4gHeelom3Mo%3d&risl=&pid=ImgRaw&r=0&sres=1&sresct=1",
                                 "type": 1 // 1代表文件
                             })
        chatListModel.append({
                                "uid": 100002,
                                "name": "李四",
                                "time": 1660893694,

                                "message": "这是一条很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长很长的消息",

                                "avatar": "https://ts1.cn.mm.bing.net/th/id/R-C.1eed2de61a172c6ca2d79fc5ea62eb01?rik=c7W7KrSN7xFOIg&riu=http%3a%2f%2fimg.crcz.com%2fallimg%2f202003%2f10%2f1583821081100057.jpg&ehk=q%2f9lt0hQhwZzKFdRKYyG2g4zxQKgTWKJ4gHeelom3Mo%3d&risl=&pid=ImgRaw&r=0&sres=1&sresct=1",
                                "type": 0
                            })
        chatListModel.append({
                                "uid": 100001,
                                "name": "cyy",
                                "time": 1660893694,

                                "message": "你好，我是丁真",
                                "avatar": "https://ts1.cn.mm.bing.net/th/id/R-C.1eed2de61a172c6ca2d79fc5ea62eb01?rik=c7W7KrSN7xFOIg&riu=http%3a%2f%2fimg.crcz.com%2fallimg%2f202003%2f10%2f1583821081100057.jpg&ehk=q%2f9lt0hQhwZzKFdRKYyG2g4zxQKgTWKJ4gHeelom3Mo%3d&risl=&pid=ImgRaw&r=0&sres=1&sresct=1",
                                "type": 0
                            })

        // 组件加载完后自动拉消息
        if(isValidId(myUid) && isValidId(targetId)){
            requestMessageSignal(myUid, targetId);
            console.log(`开始拉取 myUid(${myUid}) 和 targetId(${targetId}) 间的聊天记录……`);
        }else{
            console.log(`myUid(${myUid})或者targetId(${targetId})不正确，不拉消息`);
        }
    }



}
