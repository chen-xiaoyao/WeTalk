import QtQuick 2.12
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import "./js/config_utils.js" as ConfigUtils
import "./components"

Window {
    id: registerWindows
    width: 248
    height: 400
    visible: false //窗口是否可见
    title: qsTr("注册") //窗口标题
    objectName: "registerWindow"

    //发送信号
    signal registerSignal(string usrName,string usrPassword)

    property var sendArg


    MessageDialog
    {//注册失败提示
            id: wrongRegister
            title: "提示"
            icon: StandardIcon.Warning
            text: "注册失败"
            standardButtons: StandardButton.Cancel
    }

    MessageDialog
    {//验证失败提示
            id: wrongCheck
            title: "提示"
            icon: StandardIcon.Warning
            text: "验证失败"
            standardButtons: StandardButton.Cancel
    }
    MessageDialog
    {//邮箱输入错误提示
            id: wrongEmail
            title: "提示"
            icon: StandardIcon.Warning
            text: "邮箱输入错误"
            standardButtons: StandardButton.Cancel
    }

    Dialog
    {//返回注册结果
        id: idDialog
        width: 300;
        height: 300
        title: qsTr("注册信息确认")
        standardButtons: StandardButton.Ok

        Text {
            id: text
            font.pixelSize: 15
            text: qsTr("您的账号：")  +  usrID +  qsTr("\n您的用户名：") + usrName
            anchors.centerIn: parent
        }


        onAccepted:
        {
            console.log("Having checked the new usr inf")
            mainWindow.show()
            registerWindows.hide()
        }
    }



    //接收服务器返回的账号，打印后进入主界面
    function registerBack(pID)
    {
        console.log(1)
        if (0 == pID)
        {
            wrongRegister.open()
        }
        else
        {
            idDialog.open()
            usrID = pID.toString()
            inputID.text = usrID
            inputPSW.text = usrPassword
            Config.write("myID",usrID)

        }
    }


    // POST
    function post(url, arg,flag)
    {

        var xhr = new XMLHttpRequest;
        xhr.open("POST", url);
        xhr.setRequestHeader("Content-Length", arg.length);
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded;"); //post必备
        xhr.onreadystatechange = function() {
            handleResponse(xhr,flag);
        }
        xhr.send(arg);
    }

    property int check : 0

    // 处理返回值
    function handleResponse(xhr,flag){
        if (xhr.readyState == XMLHttpRequest.DONE)
        {
            print("DONE")
            const response = JSON.parse(xhr.responseText.toString())
            console.log(response.code)

            if(50 != response.code)
            {
                console.log(response.error_msg)
            }
            else
            {
                console.log(response.message)
            }

            if(response.code == -1)
            {
                if(flag == 1)
                {
                    wrongEmail.open()
                }
                else if(flag == 0)
                {
                    wrongCheck.open()
                    check = 1
                }
            }
            else
            {
                if(flag == 0)
                {
                    console.log("inf config ok")
                    registerSignal(usrName,usrPassword)

                    loginWindows.show()
                    close()
                }
            }



        }
        else if(xhr.readyState == XMLHttpRequest.HEADERS_RECEIVED)
        {
            print('HEADERS_RECEIVED')
        }
    }




    MessageDialog
    {
            id:wrongPSW
            title: "提示"
            icon: StandardIcon.Warning
            text: "输入密码不匹配"
            standardButtons: StandardButton.Cancel
    }
    MessageDialog
    {
            id: noUsrName
            title: "提示"
            icon: StandardIcon.Warning
            text: "请输入用户名"
            standardButtons: StandardButton.Cancel
    }
    MessageDialog
    {
            id: noPassword
            title: "提示"
            icon: StandardIcon.Warning
            text: "请输入密码"
            standardButtons: StandardButton.Cancel
    }


    ColumnLayout
    {
        Layout.alignment: Qt.AlignHCenter
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -20


        TextField
        {
            Layout.alignment: Qt.AlignHCenter
            id: usr
            width:200
            height: 50
            leftPadding: 4
            placeholderText: qsTr("<center>用户名</center>")
        }
        TextField
        {
            Layout.alignment: Qt.AlignHCenter
            id: password
            Layout.topMargin: 15
            width:200
            height: 50
            leftPadding: 4
            echoMode: TextInput.Password
            placeholderText: qsTr("<center>密码</center>")
        }
        TextField
        {
            Layout.alignment: Qt.AlignHCenter
            id: passwordAgain
            Layout.topMargin: 15
            width:200
            height: 50
            leftPadding: 4
            echoMode: TextInput.Password
            placeholderText: qsTr("<center>确认密码</center>")
        }
        TextField
        {
            Layout.alignment: Qt.AlignHCenter
            id: emailAddress
            Layout.topMargin: 15
            width:200
            height: 50
            leftPadding: 4
            placeholderText: qsTr("<center>邮件地址</center>")
        }
        Rectangle
        {
            Layout.alignment: Qt.AlignHCenter
            width: 200
            Layout.topMargin: 15
            TextField
            {
                id: captcha
                width:100
                height: 50
                leftPadding: 4
                placeholderText: qsTr("<center>验证码</center>")
            }
            Button
            {
                anchors.left: captcha.right
                anchors.leftMargin: 10
                id:sendEmail
                text: "发送验证码"
                onClicked:
                {
                    if("" == usr.text)
                    {
                        noUsrName.open()
                    }
                    else
                    {
                        //"email=xxx.@xxxx.com&username=xxx"

                        sendArg = qsTr("email=") + emailAddress.text + qsTr("&username=") +usr.text
                        //发送验证码
                        post("http://wetalk.funnysaltyfish.fun/send_verify_email",sendArg, 1 )
                    }
                }
            }
        }



        RoundButton
        {
            //注册，自动分配账号，并弹出message写出对应的账号和用户名，跳过登陆步骤进入主界面
            id: register
            Layout.topMargin: 50
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("注册")
            flat: true

            onClicked:
            {
                usrName = usr.text
                usrPassword = password.text
                sendArg = qsTr("email=") + emailAddress.text + qsTr("&verify_code=") +captcha.text

                post("http://wetalk.funnysaltyfish.fun/verify_email",sendArg,0)



                if(passwordAgain.text != password.text)
                {
                    console.log("psw not match.")
                    wrongPSW.open()

                }
                else if(usrName == "")
                {
                    console.log("no usr name")
                    noUsrName.open()
                }
                else if(password.text == "")
                {
                    console.log("no password")
                    noPassword.open()
                }
                else
                {
                    post("http://wetalk.funnysaltyfish.fun/verify_email",sendArg,0)

                }

            }

            anchors.verticalCenterOffset: 125
        }

    }
    Button
    {
        id: backToLogin
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: "返回"
        onClicked:
        {
            loginWindows.show()
            close()
        }

    }


}
