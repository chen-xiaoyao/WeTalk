import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import QtQml 2.0

Window {
    //与AddFriendWindow公用InfListItem和SearchWidget （不知道全局变量相同名字是否会影响赋值
    id: friendList
    width: 300
    height: 500
    visible: true
    title: qsTr("加好友/群")


    property string headImg: "https://c-ssl.dtstatic.com/uploads/blog/202203/25/20220325232426_17909.thumb.1000_0.jpeg"

    // 发送搜索信号
    signal sendSearch(string num)

    //接收搜索结果 json数组
    function receiveSearchResult(data)
    {
        var pData = data.pData
        var gData = data.gData

    }


    // 搜索栏

    SearchWidget
    {
        id: searchWidget
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20

    }
    Rectangle
    {
        id:searchResult
        width: parent.width
        height: parent.height
        anchors.top: searchWidget.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 8

            Text {
                id: pInf
                text: qsTr("<font color='#767777'>联系人</font>")
            }
            Rectangle
            {
                id: pInfList
                anchors.top: pInf.bottom
                width: parent.width

                //无法计算listView当前含有内容=>对应应该展示的高度
                height: parent.height*0.4

                //好友搜索结果表
                ListModel {
                    id: pInfModel
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                }
                ListView {
                    id: pInfListView
                    anchors.fill: parent
                    spacing: 60
                    focus: true
                    anchors.margins: 8


                    model: pInfModel
                    delegate:InfListItem
                    {
                        width: parent.width
                    }

                    //listView 内部滚动条设置
                    orientation: ListView.Vertical//垂直列表
                    clip: true
                    ScrollBar.vertical: ScrollBar {
                        id: scrollBar
//                        policy: ScrollBar.AlwaysOn
                    }
                    flickableDirection: Flickable.VerticalFlick
                    boundsBehavior: Flickable.StopAtBounds

                }




            Text {
                id: gInf

                anchors.top: pInfList.bottom
                anchors.topMargin: 0
                text: qsTr("<font color='#767777'>群组</font>")
            }

            Rectangle
            {
                id: gInfList
                anchors.top: gInf.bottom
                anchors.topMargin: 5
                width: parent.width

                //无法计算listView当前含有内容=>对应应该展示的高度
                height: parent.height*0.9
    //            height: parent.height/3

                //好友搜索结果表
                ListModel {
                    id: gInfModel
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                    ListElement {
                        userName: "张三"
                        userid: 123
                        avatar: "https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg"
                    }
                }
                ListView {
                    id: gInfListView
                    anchors.fill: parent
                    spacing: 60


                    focus: true
                    anchors.margins: 8

                    model: gInfModel
                    delegate:InfListItem
                    {
                        width: parent.width
                    }
                    //listView 内部滚动条设置
                    orientation: ListView.Vertical//垂直列表
                    clip: true
                    ScrollBar.vertical: ScrollBar {
                        id: scrollBar2
//                        policy: ScrollBar.AlwaysOn
                    }
                    flickableDirection: Flickable.VerticalFlick
                    boundsBehavior: Flickable.StopAtBounds
                }
            }

        }
    }






















}
