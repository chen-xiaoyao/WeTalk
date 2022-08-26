#include "sqlgroupmodel.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtDebug>
#include <QtMath>

static const char* groupTableName = "GROUPINFO";
static const char* memberTableName = "MEMBERINFO";
static const char* groupMessageTableName = "GROUPMESSAGEINFO";
static void createTable(QSqlDatabase db)
{
    if(!db.tables().contains(groupTableName))
    {
        if(db.open())
        {
            QSqlQuery query(db);
            if(!query.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                                   "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "NAME CHAR(15) NOT NULL,"
                                   "ICON TEXT DEFAULT 'https://c-ssl.duitang.com/uploads/blog/201408/15/20140815095903_ttcnF.jpeg',"
                                   "INTRO TEXT DEFAULT '这个群太新了，还没有简介。',"
                                   "NOTICE TEXT)"
                                   ).arg(groupTableName)))
            {
                qDebug() << "群总表创建发生错误";
                qDebug() << query.lastError();
            }
            else
            {
                //根账号
                if(!query.exec("INSERT INTO GROUPINFO(ID, NAME) VALUES"
                               "(600000, 'ADMIN')"))
                {
                    qDebug() << "插入根账号错误";
                    qDebug() << query.lastError();
                }
            }
        }
    }
    if(!db.tables().contains(memberTableName))
    {
        if(db.isOpen())
        {
            QSqlQuery query(db);
            if(!query.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                                   "GID INTEGER REFERENCES GROUPINFO(ID) ON DELETE CASCADE,"
                                   "MID INTEGER NOT NULL REFERENCES USERINFO(ID) ON DELETE CASCADE,"
                                   "RANK INTEGER NOT NULL,"
                                   "PRIMARY KEY (GID, MID))"
                                   ).arg(memberTableName)))
            {
                qDebug() << "成员表创建发生错误";
                qDebug() << query.lastError();
            }
        }
    }
    if(!db.tables().contains(groupMessageTableName))
    {
        if(db.isOpen())
        {
            QSqlQuery query(db);
            if(!query.exec(QString("CREATE TABLE IF NOT EXISTS %1("
                                   "GID INTEGER REFERENCES GROUPINFO(ID) ON DELETE CASCADE,"
                                   "MID INTEGER NOT NULL REFERENCES USERINFO(ID) ON DELETE CASCADE,"
                                   "DATETIME INT NOT NULL,"
                                   "MESSAGE TEXT NOT NULL,"
                                   "TYPE INTEGER NOT NULL,"
                                   "PRIMARY KEY (GID, MID, DATETIME))"
                                   ).arg(groupMessageTableName)))
            {
                qDebug() << "群聊消息表创建发生错误";
                qDebug() << query.lastError();
            }
        }
    }
}
SqlGroupModel::SqlGroupModel(QObject *parent, QSqlDatabase db):
    QSqlTableModel(parent, db)
{
    createTable(this->database());
}

SqlGroupModel::~SqlGroupModel()
{
    database().close();
}

int SqlGroupModel::createGroup(const int& masterID)
{
    setTable(groupTableName);
    QSqlQuery query;
    QJsonObject obj;
    QJsonDocument doc;
    QByteArray bArry;
    int id = 0;
    if(!query.exec(QString("INSERT INTO GROUPINFO(NAME) VALUES("
                           "'%1')").arg(QString::number(masterID)+"s Group")))
    {
        qDebug() << "创建群聊发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "创建群聊成功";
        select();
        QSqlRecord record = QSqlTableModel::record(rowCount()-1);
        id = record.value(0).toInt();
        this->joinGroup(id, masterID, 1);
    }

    return id;
}

QByteArray SqlGroupModel::memberList(const int & gID)
{
    setTable(memberTableName);
    QSqlQuery query;
    QJsonArray jsonAry;
    QByteArray bAry;
    QJsonObject finalObj;
    if(!query.exec(QString("SELECT MID, RANK FROM MEMBERINFO "
                           "WHERE GID=%1").arg(QString::number(gID))))
    {
        qDebug() << "选择群成员错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "选择群成员成功";
        while(query.next())
        {
            QJsonObject obj;
            obj.insert("id", QJsonValue(query.value("MID").toInt()));
            obj.insert("rank", QJsonValue(query.value("RANK").toInt()));
            jsonAry.append(QJsonValue(obj));
        }
        qDebug() << jsonAry;
        finalObj.insert("list", QJsonValue(jsonAry));
    }
    finalObj.insert("command", QJsonValue("memberinfo"));
    bAry = QJsonDocument(finalObj).toJson();
    return bAry;
}

void SqlGroupModel::updateGroupInfo(const int& id, const QString & name, const QString & intro, const QString & notice)
{
    setTable(groupTableName);
    setFilter(QString("ID = %1").arg(id));
    if(!select())
    {
        qDebug() << lastError();
    }
    else
    {
        QSqlRecord record = QSqlTableModel::record(0);
        record.setValue("ID", id);
        record.setValue("NAME", name);
        record.setValue("INTRO", intro);
        record.setValue("NOTICE", notice);
        setRecord(0, record);
        if(!submitAll())
        {
           qDebug() << lastError();
        }
    }
}

void SqlGroupModel::delGroup(const int & gID)
{
    setTable(groupTableName);
    QSqlQuery query;
    if(!query.exec(QString("DELETE FROM GROUPINFO WHERE "
                           "GID=%1").arg(QString::number(gID))))
    {
        qDebug() << "删除群聊发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "删除群聊成功";
    }
}

bool SqlGroupModel::joinGroup(const int & gID, const int & mID, const int & rank)
{
    setTable(memberTableName);
    QSqlQuery query;
    if(!query.exec(QString("INSERT INTO MEMBERINFO(GID, MID, RANK) VALUES("
                           "%1, %2, %3)").arg(QString::number(gID), QString::number(mID), QString::number(rank))))
    {
        qDebug() << "加入群聊发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "加入群聊成功";
    }
}

void SqlGroupModel::quitGroup(const int & gID, const int & mID)
{
    setTable(memberTableName);
    QSqlQuery query;
    if(!query.exec(QString("DELETE FROM MEMBERINFO WHERE "
                           "GID=%1 AND MID=%2").arg(QString::number(gID), QString::number(mID))))
    {
        qDebug() << "退出群聊发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "退出群聊成功";
    }
}

void SqlGroupModel::sendMessage(const int &gID, const int &mID, const int& type, const int &datetime, const QString &message)
{
    setTable(groupMessageTableName);
    QSqlQuery query;
    if(!query.exec(QString("INSERT INTO GROUPMESSAGEINFO VALUES("
                           "%1, %2, %3, '%4', %5)").arg(QString::number(gID), QString::number(mID), QString::number(datetime), message, QString::number(type))))
    {
        qDebug() << "添加群聊天信息发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "添加群聊天信息成功";
    }
}

QByteArray SqlGroupModel::groupList(const int &mID)
{
    QSqlQuery query;
    QByteArray bAry;
    QJsonObject finalObj;
    QJsonArray jsonAry;
    if(!query.exec(QString("SELECT ID, NAME, ICON, INTRO, NOTICE FROM MEMBERINFO JOIN GROUPINFO "
                   "ON MEMBERINFO.GID=GROUPINFO.ID "
                   "WHERE MID=%1").arg(QString::number(mID))))
    {
        qDebug() << "获取群列表发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "获取群列表成功";
        while(query.next())
        {
            QJsonObject obj;
            obj.insert("id", QJsonValue(query.value("ID").toInt()));
            obj.insert("name", QJsonValue(query.value("NAME").toString()));
            obj.insert("icon", QJsonValue(query.value("ICON").toString()));
            obj.insert("intro", QJsonValue(query.value("INTRO").toString()));
            obj.insert("notice", QJsonValue(query.value("NOTICE").toString()));
            jsonAry.append(QJsonValue(obj));
        }
    }
    finalObj.insert("groupList", QJsonValue(jsonAry));
    finalObj.insert("command", QJsonValue("groupBack"));
    bAry = QJsonDocument(finalObj).toJson();
    return bAry;
}

QByteArray SqlGroupModel::messageList(const int &gID)
{
    QSqlQuery query;
    QByteArray bAry;
    QJsonObject finalObj;
    QJsonArray jsonItem;
    if(!query.exec(QString("SELECT * FROM GROUPMESSAGEINFO "
                           "WHERE GID=%1 ORDER BY GID ASC, DATETIME ASC").arg(QString::number(gID))))
    {
        qDebug() << "选择群聊天信息错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "选择群聊天信息成功";
        while(query.next())
        {
            QJsonObject obj;
            obj.insert("mid", QJsonValue(query.value("MID").toInt()));
            obj.insert("datetime", QJsonValue(query.value("DATETIME").toInt()));
            obj.insert("message", QJsonValue(query.value("MESSAGE").toString()));
            jsonItem.append(QJsonValue(obj));
        }

        finalObj.insert("grouplist", QJsonValue(jsonItem));
        finalObj.insert("targetId",gID);
        finalObj.insert("command", QJsonValue("messageBack"));
    }
    bAry = QJsonDocument(finalObj).toJson();
    return bAry;
}

QByteArray SqlGroupModel::groupInfo(const int &gID)
{
    QSqlQuery query;
    QByteArray bAry;
    QJsonObject finalObj;
    if(!query.exec(QString("SELECT * FROM GROUPINFO WHERE "
                           "ID=%1 ").arg(QString::number(gID))))
    {
        qDebug() << "选择群信息发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "选择群信息成功";
        while(query.next())
        {
            QJsonObject obj;
            obj.insert("id", QJsonValue(query.value("ID").toInt()));
            obj.insert("name", QJsonValue(query.value("NAME").toString()));
            obj.insert("icon", QJsonValue(query.value("ICON").toString()));
            obj.insert("intro", QJsonValue(query.value("INTRO").toString()));
            obj.insert("notice", QJsonValue(query.value("NOTICE").toString()));
            finalObj.insert("result", QJsonValue(obj));
        }
        qDebug() << finalObj;
    }
    finalObj.insert("command", QJsonValue("groupInfoBack"));
    bAry = QJsonDocument(finalObj).toJson();
    return bAry;
}
