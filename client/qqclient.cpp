#include "qqclient.h"
#include <QQmlContext>
#include <QVariant>

QQClient::QQClient(QQmlApplicationEngine *engine, QObject *parent)
{
    //保存前端指针
    this->engine=engine;
    this->root=engine->rootObjects().first();//Qt对象树结构,root是qml全局的根节点，唯一
    //qDebug()<<root->property("width");
    //客户端设置
    quint16 listenPort=9991;
    qDebug()<<QString("客户端监听%1").arg(listenPort);
    //创建套接字
    udpSocket=new QUdpSocket(this);
    //监听
    udpSocket->bind(QHostAddress::Any,listenPort);
    //readyRead响应
    connect(udpSocket,&QUdpSocket::readyRead,
            this,&QQClient::on_udpSocket_readyRead);
    //
    //qml到C++(信号型通讯)
    QObject::connect(root,SIGNAL(loginSignal(int,QString)), //登录
                     this,SLOT(login(int,QString)));
    QObject* registerWindow=root->findChild<QObject*>("registerWindow");
    QObject::connect(registerWindow,SIGNAL(registerSignal(QString,QString)), //注册
                     this,SLOT(registerAccount(QString,QString)));
    QObject* chatScreen=root->findChild<QObject*>("mainWindow")->findChild<QObject*>("chatWindow1")
            ->findChild<QObject*>("chatWindow2")->findChild<QObject*>("chatScreen");
    QObject::connect(chatScreen,SIGNAL(sendMessageSignal(int,QString,int)),
                     this,SLOT(sendChatMessage(int,QString,int)));
    QObject* addFriendWindow=root->findChild<QObject*>("mainWindow") //加好友
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen")
            ->findChild<QObject*>("addFriendWindow");
    QObject::connect(addFriendWindow,SIGNAL(searchSignal(int)),
                     this,SLOT(search(int)));
    QObject::connect(addFriendWindow,SIGNAL(addSignal(int)),
                     this,SLOT(add(int)));
    QObject* createGroupWindow=root->findChild<QObject*>("mainWindow")//建群 createGroup
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen")
            ->findChild<QObject*>("createGroup");
    QObject::connect(createGroupWindow,SIGNAL(createGroupSignal(QVariant)),
                     this,SLOT(createGroup(QVariant)));
    QObject* contactScreen=root->findChild<QObject*>("mainWindow")//通讯录刷新
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen");
    QObject::connect(contactScreen,SIGNAL(requestContactSignal()),
                     this,SLOT(refreshContactFriend()));//人
    QObject::connect(contactScreen,SIGNAL(requestGroupSignal()),
                     this,SLOT(refreshContactGroup()));//群
    //历史聊天刷新
    QObject* historyMessageScreen=root->findChild<QObject*>("mainWindow")->findChild<QObject*>("chatWindow1")
            ->findChild<QObject*>("chatWindow2")->findChild<QObject*>("historyMessageScreen");
    QObject::connect(historyMessageScreen,SIGNAL(refreshChatListSignal(int)),
                     SLOT(messageRequest(int)));
    QObject* infoCheckScreen = chatScreen->findChild<QObject*>("chatListView");//->findChild<QObject*>("chatListItem");
    qDebug() << "personInfoCheck" << infoCheckScreen->findChildren<QObject*>()[0]->findChildren<QObject*>() << endl;
    QObject::connect(infoCheckScreen->findChildren<QObject*>()[1], SIGNAL(personInfSignal(int,bool)),
                     this,SLOT(infoRequest(int,bool)));

    //修改个人信息
    QObject* changePInfo=root->findChild<QObject*>("personalInfPage");
    QObject::connect(changePInfo,
                     SIGNAL(setUsrInfSignal(QString,QString,int,QString,int,QString,QString)),
                     this,
                     SLOT(changePInfoRequest(QString,QString,int,QString,int,QString,QString)));


    /*
    clientId=100002;
    sendChatMessage(100001,"nihao",123);
    sendChatMessage(600001,"你们好",824);
    */


    /*
    //c++调qml函数（例子）
    QVariant res;
    QVariant input="C++到qml";
    QMetaObject::invokeMethod(root,"myFunc",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,input));
    */

    /*
    // 节点测试
    QObject* registerButton=root->findChild<QObject*>("registerWindow")
            ->findChild<QObject*>("registerButton");//注册按钮节点
    qDebug()<<loginButton->property("width"); //要是正常输出属性就说明找对了
    */
}

QQClient::~QQClient()
{

}

//标准收
void QQClient::on_udpSocket_readyRead()
{
    //收到UDP包，提取信息：对方IP，端口，报文
    //准备空间
    char buf[4096]={0};
    QHostAddress ip;
    quint16 port;
    QJsonParseError parseErr;
    QJsonObject recObj;
    QString result;
    //提取
    qint64 len = udpSocket->readDatagram(buf,sizeof(buf),&ip,&port);
    //显示+处理
    if(len>0)
    {
        //这里不做解析，基本不做处理，解析交给parseCommand
        QString content=QString("[%1,%2]：%3")
                        .arg(ip.toString())
                        .arg(port)
                        .arg(buf);
        qDebug()<<content<<'\n';
        //携带全部信息，进行命令解析+分发
        parseCommand(buf,ip,port);//注意只写buf，不要把content写进来
    }
    else
    {
        QString content=QString("收取失败");
        qDebug()<<content<<'\n';
    }

}

//标准发（类型的排列组合）
void QQClient::sendMessage(QString content,QString ip, QString port)
{
    udpSocket->writeDatagram(content.toUtf8().data(),QHostAddress(ip),port.toUInt());
}

void QQClient::sendMessage(QString content,QHostAddress ip, quint16 port)
{
    udpSocket->writeDatagram(content.toUtf8().data(),ip,port);
}

void QQClient::sendMessage(QByteArray content, QHostAddress ip, quint16 port)
{
    udpSocket->writeDatagram(content,ip,port);
}

void QQClient::sendMessage(QByteArray content, QString ip, QString port)
{
    udpSocket->writeDatagram(content,QHostAddress(ip),port.toUInt());
}

//响应解析
void QQClient::parseCommand(QString jsonStr,QHostAddress ip, quint16 port)
{
    //生成json对象
    QJsonParseError error;
    QByteArray jsonByteArray=jsonStr.toUtf8();
    QJsonObject obj=QJsonDocument::fromJson(jsonByteArray,&error).object();
    //解析命令
    QString command=obj["command"].toString();
    //推荐用对应的函数名
    if (command=="registerBack")//注册响应
    {
        registerBack(obj);
    }
    else if(command=="loginBack")//登录响应
    {
        loginBack(obj);
    }
    else if(command=="sendChatMessageBack")//收到消息
    {
        sendChatMessageBack(obj);
    }
    else if(command=="searchBack")//查找响应
    {
        searchBack(obj);
    }
    else if(command=="addBack")//添加响应
    {
        addBack(obj);
    }
    else if(command=="deleteBack")//删除响应
    {
        deleteBack(obj);
    }
    else if(command=="friendBack")//好友列表响应
    {
        friendBack(obj);
    }
    else if(command=="groupBack")//群列表响应
    {
        groupBack(obj);
    }
    else if(command=="messageBack")//历史消息响应
    {
        messageBack(obj);
    }
    else if(command == "personInfoBack"){//查询某人信息
        personInfoBack(obj);
    }
    else if(command == "groupInfoBack"){//查询某群信息
        groupInfoBack(obj);
    }
    else
    {
        //未知命令
        qDebug()<<"未知命令";
    }

}


//请求函数
//注册请求
void QQClient::registerAccount(QString user, QString password)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","register");
    jsonObj.insert("user",user);
    jsonObj.insert("password",password);
    QString content=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(content,this->hostIp,this->hostPort);
}

//登录请求
void QQClient::login(int id, QString password)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","login");
    jsonObj.insert("id",id);
    jsonObj.insert("password",password);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
    //设置临时客户端id
    clientId=id;
    //测试代码

    //add(100002);
    //deleteRequest(100002);
    //friendRequest(100001);
    //groupRequest(100001);
    //messageRequest(100001);

}

//单发请求
void QQClient::sendChatMessage(int targetId, QString content,int time)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","sendChatMessage");
    jsonObj.insert("sendId",clientId);
    jsonObj.insert("targetId",targetId);
    jsonObj.insert("content",content);
    jsonObj.insert("time",time);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}

//查找请求
void QQClient::search(int targetId)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","search");
    jsonObj.insert("targetId",targetId);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}

//添加请求
void QQClient::add(int targetId)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","add");
    jsonObj.insert("sendId",clientId);
    jsonObj.insert("targetId",targetId);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}

void QQClient::createGroup(QVariant var)
{
    QVariantList memberList=var.toList();
    //建群
    QJsonObject obj;
    obj.insert("command","createGroup");
    obj.insert("id",clientId);
    QJsonArray memberArray;
    for(int i=0;i<memberList.length();i++) //加群
    {
       memberArray.append(memberList[i].toInt());
    }
    obj.insert("members",memberArray);
    //发送
    QString diagram=QJsonDocument(obj).toJson();
    sendMessage(diagram,this->hostIp,this->hostPort);
}

//删除请求
void QQClient::deleteRequest(int targetId)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","delete");
    jsonObj.insert("targetId",targetId);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}
//好友列表请求
void QQClient::friendRequest(int id)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","friendRequest");
    jsonObj.insert("id",id);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}
//历史消息请求
void QQClient::messageRequest(int id)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","messageRequest");
    jsonObj.insert("requestId",clientId);
    jsonObj.insert("targetId",id);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}
//群列表请求
void QQClient::groupRequest(int id)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","groupRequest");
    jsonObj.insert("id",id);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}
void QQClient::infoRequest(int id, bool isGroup){
    if(isGroup == true){
        groupInfoRequest(id);
    }
    else{
        personInfoRequest(id);
    }
}


void QQClient::personInfoRequest(int id){
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","personInfoRequest");
    jsonObj.insert("id",id);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}

void QQClient::groupInfoRequest(int id){
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","groupInfoRequest");
    jsonObj.insert("id",id);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}

void QQClient::changePInfoRequest(QString name, QString saying, int id, QString personalHead, int sexn,
                                  QString area, QString edul)
{
    //封装Json
    QJsonObject jsonObj;
    jsonObj.insert("command","changePInfoRequest");
    jsonObj.insert("id",id);
    jsonObj.insert("name", name);
    jsonObj.insert("icon", personalHead);
    jsonObj.insert("gender", sexn);
    jsonObj.insert("area", area);
    jsonObj.insert("edu", edul);
    QString diagram=QJsonDocument(jsonObj).toJson();
    //发送
    sendMessage(diagram,this->hostIp,this->hostPort);
}
//响应函数
//注册响应

void QQClient::registerBack(QJsonObject obj)
{
    //注册
    int id=obj["id"].toInt();
    qDebug()<<"register";
    //调用QML注册函数
    QVariant res; //如果QML函数没有返回值会不会报错？
    QObject* registerWindow=root->findChild<QObject*>("registerWindow");
    QMetaObject::invokeMethod(registerWindow,"registerBack",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,id));
}

//登录响应
void QQClient::loginBack(QJsonObject obj)
{
    //登录
    bool isSuccess=obj["result"].toBool();
    qDebug() << "loginBack::data" << obj;
    //如果登录失败就重置客户端id
    if (!isSuccess)
    {
        clientId=0;
    }
    else//成功，拉数据
    {
        friendRequest(clientId);
        groupRequest(clientId);
        messageRequest(clientId);
        //测试
        /*
        messageRequest(100002);
        messageRequest(600001);
        */
    }
    //调用QML登录函数
    QVariant res; //如果QML函数没有返回值会不会报错？
    QMetaObject::invokeMethod(root,"loginBack",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,isSuccess));

}
//响应
void QQClient::sendChatMessageBack(QJsonObject obj)
{
    //解包
    //groupId,sendId
    int sendId=obj["sendId"].toInt();
    QString content=obj["content"].toString();
    int time=obj["time"].toInt();
    //结合本地数据构建消息包
    QJsonObject jsonObj;
    jsonObj.insert("uid",sendId);//id
    jsonObj.insert("groupId",obj["groupId"].toInt()); //groupId: 0是单发，或者是600001这种，群发
    //利用sendId从本地获取名字，头像(不论是单发还是群聊，都用发送者的)
    for(int i=0;i<friendList.length();i++)
    {
        if(friendList[i]->id==sendId)
        {
            jsonObj.insert("name",friendList[i]->name);
            jsonObj.insert("avatar",friendList[i]->icon);
        }
    }
    jsonObj.insert("time",time);
    jsonObj.insert("message",content);

    jsonObj.insert("type",0);
    //调用QML函数
    QVariant res;
    QObject* chatScreen=root->findChild<QObject*>("mainWindow")->findChild<QObject*>("chatWindow1")
            ->findChild<QObject*>("chatWindow2")->findChild<QObject*>("chatScreen");
    QMetaObject::invokeMethod(chatScreen,"appendData",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,jsonObj)
                              );
}
//查找响应
void QQClient::searchBack(QJsonObject obj)
{
    //调用QML函数
    QVariant res; //如果QML函数没有返回值会不会报错？
    QObject* addFriendWindow=root->findChild<QObject*>("mainWindow") //加好友
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen")
            ->findChild<QObject*>("addFriendWindow");
    QMetaObject::invokeMethod(addFriendWindow,"searchBack",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,obj));
}
//添加响应
void QQClient::addBack(QJsonObject obj)
{
    //解包
    int friendId=obj["friendId"].toInt();
    //调用QML函数
    QVariant res; //如果QML函数没有返回值会不会报错？
    QObject* addFriendWindow=root->findChild<QObject*>("mainWindow") //加好友
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen")
            ->findChild<QObject*>("addFriendWindow");
    QMetaObject::invokeMethod(addFriendWindow,"addBack",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,friendId));
}
//删除响应
void QQClient::deleteBack(QJsonObject obj)
{
    qDebug()<<"deleteBack()"<<obj;
}
//朋友列表响应(同时更新clientInfo)
void QQClient::friendBack(QJsonObject obj)
{
    //解包
    QJsonArray friendJsonList=obj["list"].toArray();
    //清空原有朋友列表
    friendList.clear();
    //自己的信息
    QJsonObject clientObj=friendJsonList[0].toObject();

    QQmlContext* context = engine->rootContext();
    ClientInfo* info = new ClientInfo(clientObj);
    clientName=clientObj["name"].toString();
    clientAvatar=clientObj["icon"].toString();
    context->setContextProperty("ClientInfo", info);

    //遍历添加刷新后朋友列表
    for(int i=1;i<friendJsonList.size();i++)
    {
        //解包
        QJsonObject user=friendJsonList[i].toObject();
        //添加对象
        this->friendList.append(new User(
                                    user["id"].toInt(),
                                    user["name"].toString(),
                                    user["icon"].toString())
                                        );
    }
}
//历史消息响应
void QQClient::messageBack(QJsonObject obj)
{
    //包装QJsonArray<QJsonObject>
    int targetId=obj["targetId"].toInt();
    QJsonArray packageList;//加强消息包数组
    if(targetId/100000==1) //人
    {
        QJsonArray messageList=obj["messagelist"].toArray();
        for(int i =0;i<messageList.size();i++) //逐消息处理
        {
            QJsonObject item=messageList[i].toObject();//初始包
            //groupId,sendId
            int sendId=item["sid"].toInt();
            QString content=item["message"].toString();
            int time=item["datetime"].toInt();
            //结合本地数据构建消息包
            QJsonObject jsonObj;//加强包
            jsonObj.insert("uid",sendId);//id
            jsonObj.insert("groupId",0); //groupId: 0是单发，或者是600001这种，群发
            //利用sendId从本地获取名字，头像(不论是单发还是群聊，都用发送者的)
            if(sendId==clientId)//自己的信息从info抓
            {
                jsonObj.insert("name",clientName);
                jsonObj.insert("avatar",clientAvatar);

            }
            else //朋友信息
            {
                for(int i=0;i<friendList.length();i++)
                {
                    qDebug()<<friendList[i]->id;
                    if(friendList[i]->id==sendId)
                    {

                        jsonObj.insert("name",friendList[i]->name);
                        jsonObj.insert("avatar",friendList[i]->icon);
                    }
                }
            }

            jsonObj.insert("time",time);
            jsonObj.insert("message",content);
            jsonObj.insert("type",0);
            packageList.append(jsonObj);
        }
    }
    else //群
    {
        QJsonArray messageList=obj["grouplist"].toArray();
        for(int i =0;i<messageList.size();i++)
        {
            QJsonObject item=messageList[i].toObject();
            //targetId就是群id mid是发送者
            qDebug()<<item["datetime"].toInt()<<item["message"].toString()<<item["mid"].toInt();
            //groupId,sendId
            int sendId=item["mid"].toInt();
            QString content=item["message"].toString();
            int time=item["datetime"].toInt();
            //结合本地数据构建消息包
            QJsonObject jsonObj;//加强包
            jsonObj.insert("uid",sendId);//id
            jsonObj.insert("groupId",targetId); //groupId: 0是单发，或者是600001这种，群发
            //利用sendId从本地获取名字，头像(不论是单发还是群聊，都用发送者的)
            if(sendId==clientId)//自己的信息从info抓
            {
                jsonObj.insert("name",clientName);
                jsonObj.insert("avatar",clientAvatar);

            }
            else //朋友信息
            {
                for(int i=0;i<friendList.length();i++)
                {
                    qDebug()<<friendList[i]->id;
                    if(friendList[i]->id==sendId)
                    {

                        jsonObj.insert("name",friendList[i]->name);
                        jsonObj.insert("avatar",friendList[i]->icon);
                    }
                }
            }
            jsonObj.insert("time",time);
            jsonObj.insert("message",content);
            jsonObj.insert("type",0);
            packageList.append(jsonObj);
        }
    }
    //调用重置函数
    QObject* chatScreen=root->findChild<QObject*>("mainWindow")->findChild<QObject*>("chatWindow1")
            ->findChild<QObject*>("chatWindow2")->findChild<QObject*>("chatScreen");
    QVariant res;
    QMetaObject::invokeMethod(chatScreen,"setMessages",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,packageList));
}
//群列表响应
void QQClient::groupBack(QJsonObject obj)
{
    //解包
    QJsonArray groupJsonList=obj["groupList"].toArray();
    //清空原有朋友列表
    groupList.clear();
    //遍历添加刷新后朋友列表
    for(int i=0;i<groupJsonList.size();i++)
    {
        //解包
        QJsonObject group=groupJsonList[i].toObject();
        //添加对象
        this->groupList.append(new Group(group["id"].toInt(),
                               group["name"].toString(),
                               group["icon"].toString(),
                               group["intro"].toString(),
                               group["notice"].toString())
                               );
    }
    for(int i=0;i<groupList.length();i++)
    {
        qDebug()<<groupList[i]->name;
    }
}
void QQClient::personInfoBack(QJsonObject obj){
//    qDebug() << "personInfoBack\n" << obj << endl;
    QVariant res;
    QObject* personInfoWindow=root->findChild<QObject*>("mainWindow")
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen");
    QMetaObject::invokeMethod(personInfoWindow,"personalInfBack",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,obj));
}
void QQClient::groupInfoBack(QJsonObject obj){
//    qDebug() << "groupInfoBack\n" << obj << endl;
    QVariant res;
    QObject* groupInfoWindow=root->findChild<QObject*>("mainWindow")
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen");
    QMetaObject::invokeMethod(groupInfoWindow,"groupInfBack",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,obj));
}
//刷新好友列表
void QQClient::refreshContactFriend()
{
    //更新本地朋友信息
    friendRequest(clientId);
    //构造QJsonArray
    QJsonArray jsonArray;
    for(int i=0;i<friendList.length();i++)
    {
        QJsonObject friendObj;
        friendObj.insert("userId",friendList[i]->id);
        friendObj.insert("userName",friendList[i]->name);
        friendObj.insert("avatar",friendList[i]->icon);
        jsonArray.append(friendObj);
    }
    //调用QML刷新函数
    QVariant res; //如果QML函数没有返回值会不会报错？
    QObject* contactScreen=root->findChild<QObject*>("mainWindow") //加好友
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen");
    QMetaObject::invokeMethod(contactScreen,"setContacts",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,jsonArray));
}
//刷新群列表
void QQClient::refreshContactGroup()
{
    //更新本地群信息
    groupRequest(clientId);
    //构造QJsonArray
    QJsonArray jsonArray;
    for(int i=0;i<groupList.length();i++)
    {
        QJsonObject groupObj;
        groupObj.insert("userId",groupList[i]->id);
        groupObj.insert("userName",groupList[i]->name);
        groupObj.insert("avatar",groupList[i]->icon);
        jsonArray.append(groupObj);
    }
    //调用QML刷新函数
    QVariant res; //如果QML函数没有返回值会不会报错？
    QObject* contactScreen=root->findChild<QObject*>("mainWindow") //加好友
            ->findChild<QObject*>("chatWindow1")->findChild<QObject*>("contactScreen");
    QMetaObject::invokeMethod(contactScreen,"setGroups",
                              Q_RETURN_ARG(QVariant,res),
                              Q_ARG(QVariant,jsonArray));
}

