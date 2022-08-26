#ifndef QQSERVER_H
#define QQSERVER_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument> //默认toJson是utf-8
#include <QDebug>
#include <iostream>
#include "sqlaccountmodel.h"
#include "sqlfriendmodel.h"
#include "sqlgroupmodel.h"
#include "onlinelist.h"
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class QQServer; }
QT_END_NAMESPACE

class User //在线成员的节点
{
public:
    User(QHostAddress ip,quint16 port,int id) //把名字和IP PORT绑定。
    {
        this->id=id;
        this->ip=ip;
        this->port=port;
    }
    ~User();

    int id;
    QHostAddress ip;
    quint16 port;
};

class Group //一个群数据
{
public:
    Group(int gId,SqlGroupModel* gpModel)
    {
        QJsonObject obj=QJsonDocument::fromJson(gpModel->memberList(gId)).object();
        QJsonArray list=obj["list"].toArray();
        for(int i=0;i<list.size();i++)
        {
            memberList.append(list[i].toObject()["id"].toInt());
        }
        for(int i=0;i<memberList.length();i++)
        {
            qDebug()<<memberList[i];
        }
    }

    ~Group()//清空数据
    {
        memberList.clear();
    }

    QList<int> memberList;
};

class QQServer : public QMainWindow
{
    Q_OBJECT

public:
    QQServer(QWidget *parent = nullptr);
    ~QQServer();


private slots:
    //通用收信息函数
    void onUdpSocketReadyRead();

private:
    Ui::QQServer *ui;
    //Socket
    QUdpSocket* udpSocket=NULL;
    //在线用户和群链表
    QList<User*> onlineUser;
    QList<Group*> groupList;
    QJsonObject getTargetIpPort(int targetId);
    //数据库操作
    SqlAccountModel *atModel;
    SqlFriendModel *fdModel;
    SqlGroupModel * gpModel;
    //数据包通信解析
    void parseCommand(QString jsonStr,QHostAddress ip,quint16 port);
    //不同参数类型的的发送信息
    void sendMessage(QString content,QString ip,QString port);
    void sendMessage(QString content,QHostAddress ip,quint16 port);
    void sendMessage(QByteArray content,QHostAddress ip,quint16 port);
    void sendMessage(QByteArray content,QString ip,QString port);
    //对客户端请求响应
    void registerRespond(QJsonObject obj,QHostAddress ip,quint16 port);//注册
    void loginRespond(QJsonObject obj,QHostAddress ip,quint16 port);//登录
    void sendChatMessageRespond(QJsonObject obj,QHostAddress ip,quint16 port);//发消息（单发+群发）
    void searchRespond(QJsonObject obj,QHostAddress ip,quint16 port);//查找（好友或群）
    void addRespond(QJsonObject obj,QHostAddress ip,quint16 port);//添加（好友或群）
    void createGroupRespond(QJsonObject obj,QHostAddress ip,quint16 port);//创建群聊
    void deleteRespond(QJsonObject obj,QHostAddress ip,quint16 port);//删除（好友或群）
    void friendRespond(QJsonObject obj,QHostAddress ip,quint16 port);//好友列表
    void groupRespond(QJsonObject obj,QHostAddress ip,quint16 port);//群列表
    void messageRespond(QJsonObject obj,QHostAddress ip,quint16 port);//历史消息
    void getPersonalInfoRespond(QJsonObject obj, QHostAddress ip, quint16 port);//获取个人信息
    void getGroupInfoRespond(QJsonObject obj, QHostAddress ip, quint16 port);//获取群信息
    void changePInfo(QJsonObject obj, QHostAddress ip, quint16 port);//更改用户信息
    void changeGInfo(QJsonObject obj, QHostAddress ip, quint16 port);//更改群聊信息
    //测试代码
    void test(SqlAccountModel* atModel,SqlFriendModel* fdModel,SqlGroupModel* gpModel);

};
#endif // QQSERVER_H
