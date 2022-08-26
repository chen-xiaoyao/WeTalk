#include "sqlfriendmodel.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtDebug>
#include <QtMath>

static const char* friendTableName = "FRIENDINFO";
static const char* friendMessageTableName = "FRIENDMESSAGEINFO";
static void createTable(QSqlDatabase db)
{
    if(!db.tables().contains(friendTableName))
    {
        if(db.open())
        {
            QSqlQuery query(db);
            if(!query.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                                   "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "USERID INTEGER NOT NULL, "
                                   "FRIENDID INTEGER NOT NULL CHECK (USERID<=FRIENDID),"
                                   "UNIQUE (USERID, FRIENDID),"
                                   "FOREIGN KEY (USERID) REFERENCES USERINFO(ID) ON DELETE CASCADE,"
                                   "FOREIGN KEY (FRIENDID) REFERENCES USERINFO(ID) ON DELETE CASCADE)"
                                   ).arg(friendTableName)))
            {
                qDebug() << "表创建发生错误";
                qDebug() << query.lastError();
            }
        }
    }
    if(!db.tables().contains(friendMessageTableName))
    {
        if(db.open())
        {
            QSqlQuery query(db);
            if(!query.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                                   "ID INTEGER REFERENCES FRIENDINFO(ID),"
                                   "SENDID INTEGER NOT NULL, "
                                   "RECEIVEID INTEGER NOT NULL,"
                                   "DATETIME INT NOT NULL,"
                                   "MESSAGE TEXT NOT NULL,"
                                   "TYPE INTEGER NOT NULL,"
                                   "PRIMARY KEY (SENDID, RECEIVEID, DATETIME),"
                                   "FOREIGN KEY (SENDID) REFERENCES USERINFO(ID),"
                                   "FOREIGN KEY (RECEIVEID) REFERENCES USERINFO(ID))"
                                   ).arg(friendMessageTableName)))
            {
                qDebug() << "好友聊天表创建发生错误";
                qDebug() << query.lastError();
            }
        }
    }
}

SqlFriendModel::SqlFriendModel(QObject *parent, QSqlDatabase db):
    QSqlTableModel(parent, db)
{
    createTable(this->database());
}

SqlFriendModel::~SqlFriendModel()
{
    database().close();
}

bool SqlFriendModel::addFriend(const int &aID, const int &bID)
{
    setTable(friendTableName);
    QSqlQuery query;
    int id1, id2;
    id1 = qMin(aID, bID);
    id2 = qMax(aID, bID);
    if(!query.exec(QString("INSERT INTO FRIENDINFO(USERID, FRIENDID) VALUES("
                           "%1, %2)").arg(QString::number(id1), QString::number(id2))))
    {
        qDebug() << "添加好友发生错误";
        qDebug() << query.lastError();
        return false;
    }
    else
    {
        qDebug() << "添加好友成功";
        return true;
    }
}

void SqlFriendModel::delFriend(const int &aID, const int &bID)
{
    setTable(friendTableName);
    QSqlQuery query;
    int id1, id2;
    id1 = qMin(aID, bID);
    id2 = qMax(aID, bID);
    if(!query.exec(QString("DELETE FROM FRIENDINFO WHERE "
                           "USERID=%1 AND FRIENDID=%2").arg(QString::number(id1), QString::number(id2))))
    {
        qDebug() << "删除好友发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "删除好友成功";
    }
}

QByteArray SqlFriendModel::friendList(const int &ID)
{
    setTable(friendTableName);
    QSqlQuery query;
    QSqlQuery query1;
    QJsonArray jsonAry;
    QByteArray bAry;
    QJsonObject finalObj;
    if(!query1.exec(QString("SELECT * FROM USERINFO WHERE "
                           "ID=%1").arg(QString::number(ID))))
    {
        qDebug() << "选择好友发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        query1.next();
        QJsonObject obj;
        obj.insert("id", QJsonValue(query1.value("ID").toInt()));
        obj.insert("name", QJsonValue(query1.value("NAME").toString()));
        obj.insert("icon", QJsonValue(query1.value("ICON").toString()));
        obj.insert("gender", QJsonValue(query1.value("GENDER").toBool()));
        obj.insert("birth", QJsonValue(query1.value("BIRTH").toString()));
        obj.insert("area", QJsonValue(query1.value("AREA").toString()));
        obj.insert("education", QJsonValue(query1.value("EDUCATION").toString()));
        obj.insert("signature", QJsonValue(query1.value("SIGNATURE").toString()));
        jsonAry.append(QJsonValue(obj));
    }
    if(!query.exec(QString("SELECT * FROM (SELECT USERID AS ID FROM FRIENDINFO WHERE "
                           "FRIENDID=%1 AND USERID !=%1 UNION "
                           "SELECT FRIENDID AS ID FROM FRIENDINFO WHERE "
                           "USERID=%1 AND FRIENDID!=%1) AS A JOIN USERINFO AS B  "
                           "ON A.ID=B.ID").arg(QString::number(ID))))
    {
        qDebug() << "选择好友发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "选择好友成功";
        while(query.next())
        {
            QJsonObject obj;
            obj.insert("id", QJsonValue(query.value("ID").toInt()));
            obj.insert("name", QJsonValue(query.value("NAME").toString()));
            obj.insert("icon", QJsonValue(query.value("ICON").toString()));
            obj.insert("gender", QJsonValue(query.value("GENDER").toBool()));
            obj.insert("birth", QJsonValue(query.value("BIRTH").toString()));
            obj.insert("area", QJsonValue(query.value("AREA").toString()));
            obj.insert("education", QJsonValue(query.value("EDUCATION").toString()));
            obj.insert("signature", QJsonValue(query.value("SIGNATURE").toString()));
            jsonAry.append(QJsonValue(obj));
        }
        qDebug() << jsonAry;
        finalObj.insert("list", QJsonValue(jsonAry));
    }
    finalObj.insert("command", QJsonValue("friendBack"));
    bAry = QJsonDocument(finalObj).toJson();
    return bAry;
}

void SqlFriendModel::sendMessage(const int &sendID, const int &receiveID, const int& type, const int &datetime, const QString &message)
{
    QSqlQuery query;
    int id;
    if(!query.exec(QString("SELECT ID FROM FRIENDINFO WHERE "
                           "(USERID=%1 AND FRIENDID=%2) OR (USERID=%2 AND FRIENDID=%1)").arg(QString::number(sendID), QString::number(receiveID))))
    {
        qDebug() << "选择好友对编号发生错误";
        qDebug() << query.lastError();
    }
    query.next();
    id = query.value("ID").toInt();
    if(!query.exec(QString("INSERT INTO FRIENDMESSAGEINFO VALUES("
                           "%1, %2, %3, %4, '%5', %6)").arg(QString::number(id),QString::number(sendID), QString::number(receiveID), QString::number(datetime), message,QString::number(type))))
    {
        qDebug() << "添加好友聊天信息发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "添加好友聊天信息成功";
    }
}

QByteArray SqlFriendModel::messageList(const int &aID, const int &bID)
{
    QSqlQuery query;
    QJsonArray jsonItem;
    QByteArray bAry;
    QJsonObject finalObj;
    if(!query.exec(QString("SELECT * FROM FRIENDMESSAGEINFO "
                           "WHERE (SENDID=%1 AND RECEIVEID=%2) "
                           "OR (SENDID=%2 AND RECEIVEID=%1) "
                           "ORDER BY ID ASC, DATETIME ASC").arg(QString::number(aID), QString::number(bID))))
    {
        qDebug() << "选择好友聊天信息错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "选择好友聊天信息成功";
        while(query.next())
        {
            QJsonObject obj;
            obj.insert("sid", QJsonValue(query.value("SENDID").toInt()));
            obj.insert("rid", QJsonValue(query.value("RECEIVEID").toInt()));
            obj.insert("datetime", QJsonValue(query.value("DATETIME").toInt()));
            obj.insert("message", QJsonValue(query.value("MESSAGE").toString()));
            jsonItem.append(QJsonValue(obj));
        }
        finalObj.insert("command","messageBack");
        finalObj.insert("targetId",bID);
        finalObj.insert("messagelist", QJsonValue(jsonItem));
    }
    bAry = QJsonDocument(finalObj).toJson();
    return bAry;
}
