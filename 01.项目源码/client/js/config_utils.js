.pragma library
Qt.include("file_utils.js")

const BASE_FILE_URL  = "file://./config/"
let CONFIG_USER_INFO = {}

function test(){
    saveFile(BASE_FILE_URL + "test.txt", "测试一下");
    readFile(BASE_FILE_URL + "test.txt")
};


