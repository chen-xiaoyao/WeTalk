import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.3

// 搜索栏
RowLayout
{
//    anchors.horizontalCenter: parent.horizontalCenter
//    anchors.top: parent.top
//    anchors.topMargin: 10


    MessageDialog
    {
            id: noInputText
            title: "提示"
            icon: StandardIcon.Warning
            text: "请输入搜索内容"
            standardButtons: StandardButton.Cancel
    }
    MessageDialog
    {
            id: notNumInputText
            title: "提示"
            icon: StandardIcon.Warning
            text: "请输入数字"
            standardButtons: StandardButton.Cancel
    }

    MessageDialog
    {
            id: cantAddMyself
            title: "提示"
            icon: StandardIcon.Warning
            text: "不能添加自己为好友"
            standardButtons: StandardButton.Cancel
    }

    Rectangle
    {
        id: imgSearch
        width: 16
        height: 16
        Image {
            Layout.preferredHeight: 16
            anchors.fill:imgSearch
            source: "qrc:/images/search.png"
            fillMode:Image.PreserveAspectFit
        }
    }


    TextField
    {
        id: inputID
        Layout.leftMargin: 12


        padding: 5
        placeholderText: qsTr("<font size='2' family='LiSu'>好友账号/群号/群名</font>")


        background: Rectangle
        {
            implicitWidth: 200
            implicitHeight: 10
            radius: implicitHeight/2
            border.color: "grey"

        }
    }


    RoundButton
    {
        // 搜索好友/群
        Layout.leftMargin: 5
        id: search
        text: qsTr("搜索")
        flat: true

        onClicked:
        {
            if("" == inputID.text)
            {
                noInputText.open()
            }
            else if(isNaN(inputID.text))
            {
                notNumInputText.open()
                inputID.text = ""
            }
            else if(ClientInfo.id == Number(inputID.text))
            {
                cantAddMyself.open()
                inputID.text = ""
            }

            else
            {
                //将搜索内容发送
                userid = Number(inputID.text)
                searchSignal(inputID.text)
                console.log("search")
                console.log(inputID.text)

            }


        }

    }
}

