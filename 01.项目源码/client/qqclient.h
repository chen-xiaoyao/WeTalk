#ifndef QQCLIENT_H
#define QQCLIENT_H
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QHostAddress>
#include <QUdpSocket>
#include <QQmlApplicationEngine>
#include <QDateTime>
#include <iostream>

class ClientInfo: public QObject//储存当前客户端用户
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)

public:
    ClientInfo(){}
    ClientInfo(QJsonObject infoObj)
    {
        parse(infoObj);
    }

    int mId=0;
    QString mName="默认昵称";
    QString mAvatar="https://www.com8.cn/wp-content/uploads/2020/08/20200823052248-5f41fd28d49e4.jpg";

    int id(){return mId;};
    void setId(int id){if(id!=mId){
            mId = id;
            emit idChanged();
        }}

    QString name(){return mName;}
    void setName(QString name){if(name != mName){
            mName = name;
            emit nameChanged();
        }
    }

    QString avatar(){return mAvatar;}
    void setAvatar(QString avatar){if(avatar != mAvatar){
            mAvatar = avatar;
            emit avatarChanged();
        }
    }

    void parse(QJsonObject infoObj)
    {
        mId=infoObj["id"].toInt();
        mName=infoObj["name"].toString();
        mAvatar=infoObj["icon"].toString();
        qDebug()<<"ClientInfo"<<mId<<mName<<mAvatar;
    }

signals:
    void idChanged();
    void nameChanged();
    void avatarChanged();
};

class User //储存用户信息
{
public:
    User(int id,QString name,QString icon)
    {
        this->id=id;
        this->name=name;
        this->icon=icon;
        qDebug()<<"构建User:"<<id<<name;
    }

    int id;
    QString name;
    QString icon;
};

class Group //储存群组信息
{
public:
    Group(int id,QString name,QString icon,QString intro,QString notice)
    {
        this->id=id;
        this->name=name;
        this->icon=icon;
        this->intro=intro;
        this->notice=notice;
        qDebug()<<"构建Group:"<<id<<name;
    }
    int id;
    QString name;
    QString icon;
    QString intro;
    QString notice;
};


class QQClient:public QObject
{
    Q_OBJECT
public:
    //初始化的时候把QMLengine塞进去
    QQClient(QQmlApplicationEngine* engine,QObject *parent = nullptr);
    ~QQClient();
    Q_INVOKABLE void test(int i);//用于注册后调用，这个函数链接不到，所以使用注册方法实现。
    Q_INVOKABLE void changePInfoRequest(QString,QString,int,QString,int,QString,QString);
    Q_INVOKABLE void changeGInfoRequest(QString groupName,QString groupNotice,QString groupSummary,int groupId);


private slots:
    //请求函数：QML到C++的槽函数
    void registerAccount(QString user,QString password);//注册
    void login(int id,QString password);//登录
    void sendChatMessage(int targetId,QString content,int time);//发消息
    void search(int targetId);//查找群/人
    void add(int targetId);//加群/人
    void createGroup(QVariant memberList);
    void deleteRequest(int targetId);//删群/人
    void friendRequest(int id);//请求好友列表（从服务端拉取数据的系列函数）
    void messageRequest(int id);//请求历史聊天记录(实际只需要id，但是为了适应signal用QVariant)
    void groupRequest(int id);//请求群列表
    void infoRequest(int id, bool isGroup);//请求某人或某群的详细数据
    void personInfoRequest(int id);//请求某人的详细数据
    void groupInfoRequest(int id);//请求某群的详细数据
    //其他函数
    void refreshContactFriend();//刷新联系人
    void refreshContactGroup();//刷新群聊通讯录


private:
    //配置信息
    QUdpSocket* udpSocket;//服务器ip和端口
    QHostAddress hostIp=QHostAddress("81.68.194.103");
    quint16 hostPort=8080;
    //临时数据
    int clientId=0;//储存当前id
    ClientInfo * clientInfo;//储存当前账户所有信息
    QString clientName;
    QString clientAvatar;
    QList<User*> friendList;//储存好友列表
    QList<Group*> groupList;//储存群组列表
    //前端通信engine
    QQmlApplicationEngine* engine=NULL;
    QObject* root=NULL;
    //基本通信方法
    void on_udpSocket_readyRead();
    void sendMessage(QString content,QString ip,QString port);//不同参数类型的的发送信息
    void sendMessage(QString content,QHostAddress ip,quint16 port);
    void sendMessage(QByteArray content,QHostAddress ip,quint16 port);
    void sendMessage(QByteArray content,QString ip,QString port);
    //数据包通信解析
    void parseCommand(QString jsonStr,QHostAddress ip,quint16 port);
    //响应函数：C++到QML的函数
    void registerBack(QJsonObject obj);
    void loginBack(QJsonObject obj);
    void sendChatMessageBack(QJsonObject obj);
    void searchBack(QJsonObject obj);
    void addBack(QJsonObject obj);
    void createGroupBack(QJsonObject obj);
    void deleteBack(QJsonObject obj);
    void friendBack(QJsonObject obj);//请求好友列表（从服务端拉取数据的系列函数）
    void messageBack(QJsonObject obj);//请求历史聊天记录
    void groupBack(QJsonObject obj);//请求群列表
    void personInfoBack(QJsonObject obj);//某人信息
    void groupInfoBack(QJsonObject obj);//某群信息
    //其他函数

};



#endif // QQCLIENT_H
