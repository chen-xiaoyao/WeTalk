#ifndef SQLACCOUNTMODEL_H
#define SQLACCOUNTMODEL_H

#include <QObject>
#include <QtSql/QSqlTableModel>
#include <QJsonArray>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

class SqlAccountModel : public QSqlTableModel
{
    Q_OBJECT
public:
    SqlAccountModel(QObject *parent, QSqlDatabase db);
    ~SqlAccountModel();

    //添加用户账号信息，如果添加成功返回id, 添加失败返回0, 包装在json下的id字段
    QByteArray addUserAccount(const QString& userName, const QString& userPassword);
    //检查用户账号是否正确，如果正确返回true, 错误返回false, 包装在json下的result字段
    QByteArray checkAccount(const int& userID, const QString& userPassword);
    //获取用户个人信息
    QByteArray userInfo(const int& userID);
    //添加或更新用户头像的url
    void updateIcon(const int& userID, const QString& iconURL);
    //更新用户信息
    void updateUserInfo(const int& userID, const QString& name, const int& gender, const QString& birth, const QString& area, const QString& education, const QString& signature);
    //获取用户所有聊天信息
    QByteArray messageList(const int& ID);
};
#endif // SQLACCOUNTMODEL_H
