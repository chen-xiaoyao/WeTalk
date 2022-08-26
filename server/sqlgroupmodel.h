#ifndef SQLGROUPMODEL_H
#define SQLGROUPMODEL_H

#include <QObject>
#include <QtSql/QSqlTableModel>
#include <QJsonArray>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

class SqlGroupModel : public QSqlTableModel
{
    Q_OBJECT
public:
    SqlGroupModel(QObject *parent, QSqlDatabase db);
    ~SqlGroupModel();
    //创建群聊
    int createGroup(const int& masterID);
    //获取群成员信息（id, 级别）
    QByteArray memberList(const int& gID);
    //修改群信息
    void updateGroupInfo(const int& id, const QString& name, const QString& intro, const QString& notice);
    //删除群聊
    void delGroup(const int& gID);
    //加入群聊
    bool joinGroup(const int& gID, const int& mID, const int& rank);
    //退出群聊
    void quitGroup(const int& gID, const int& mID);
    //添加群消息
    void sendMessage(const int& gID, const int& mID, const int& type, const int& datetime, const QString& message);
    //获取用户加入的群信息
    QByteArray groupList(const int& mID);
    //获取特定群的聊天记录
    QByteArray messageList(const int& gID);
    //获取特定群信息
    QByteArray groupInfo(const int& gID);
};

#endif // SQLGROUPMODEL_H
