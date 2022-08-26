import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3


Rectangle {
    property string _fileName: "张三.txt"
    property int _fileSize: 10000
    property double _progress: 0.5

    id: fileContainer
    width: 180
    height: 80
    radius: 4
    color: "#edf5f9"
    z: 0

    MouseArea {

        anchors.fill: parent
        onClicked: {

        }
    }

    function bytesToSize(bytes) {
        if (bytes === 0) return '0 B';
        var k = 1000, // or 1024
            sizes = ['B', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'],
            i = Math.floor(Math.log(bytes) / Math.log(k));

       return (bytes / Math.pow(k, i)).toPrecision(3) + ' ' + sizes[i];
    }


    RowLayout {
        anchors.centerIn: parent
        spacing: 8
        ColumnLayout {
            Label {
                color: "#dd000000"
                text: _fileName
                font.pointSize: 12
                font.bold: true
                styleColor: "#ffffff"
            }

            Label {
                color: "#ddab9d9d"
                text: bytesToSize(_fileSize)
                Layout.topMargin: 2
                Layout.margins: 0
            }
        }

        Image {
            Layout.leftMargin: 8
            source: "./images/icon_file.png"
            sourceSize.width : 36
            sourceSize.height: 36
            fillMode: Image.PreserveAspectFit

        }
    }

//    Button {
//        text: "<font color='#2196F3'>打开</font>"
//        anchors.rightMargin: 20
//        flat: true
//        background: undefined

//        anchors.right: parent.right
//        anchors.bottom: parent.bottom
//    }

    ProgressBar {
        id: fileUploadProgressBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        visible: 0 < _progress && _progress < 1
        value: _progress
        clip: true
    }

}
