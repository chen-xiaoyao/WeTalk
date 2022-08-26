#include "sqlaccountmodel.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtDebug>

static const char *accountTableName = "USERINFO";
static const char* friendMessageTableName = "FRIENDMESSAGEINFO";

static void createTable(QSqlDatabase db)
{
    if(db.tables().contains(accountTableName))
    {
        return;
    }
    if(db.open())
    {
        QSqlQuery query(db);
        if(!query.exec("CREATE TABLE IF NOT EXISTS USERINFO ("
                       "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "NAME CHAR(15) NOT NULL, "
                       "PASSWORD CHAR(15) NOT NULL,"
                       "ICON TEXT DEFAULT 'https://c-ssl.duitang.com/uploads/blog/201408/15/20140815095903_ttcnF.jpeg',"
                       "GENDER BOOLEAN,"
                       "BIRTH TEXT,"
                       "AREA CHAR(20),"
                       "EDUCATION CHAR(20),"
                       "SIGNATURE TEXT DEFAULT '这个人太懒了，没有个性签名')"))
        {
            qDebug() << "表创建发生错误";
            qDebug() << query.lastError();
        }
        else
        {
            //根账号
            if(!query.exec("INSERT INTO USERINFO(ID, NAME, PASSWORD) VALUES"
                           "(100000, 'ADMIN', 'ADMIN')"))
            {
                qDebug() << "插入根账号错误";
                qDebug() << query.lastError();
            }
        }
    }
}

SqlAccountModel::SqlAccountModel(QObject *parent, QSqlDatabase db):
    QSqlTableModel(parent, db)
{
    createTable(this->database());
    setEditStrategy(QSqlTableModel::OnFieldChange);
}

SqlAccountModel::~SqlAccountModel()
{
    database().close();
}

QByteArray SqlAccountModel::addUserAccount(const QString& userName, const QString& userPassword)
{
    setTable(accountTableName);
    QJsonObject obj;
    QJsonDocument doc;
    QByteArray bArry;
    QSqlQuery query;
    int id=0;
    if(!query.exec(QString("INSERT INTO USERINFO(NAME, PASSWORD) VALUES("
                           "'%1', '%2')").arg(userName, userPassword)))
    {
        qDebug() << "插入账号信息发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "插入账号信息成功";
        select();
        QSqlRecord record = QSqlTableModel::record(rowCount()-1);
        id = record.value(0).toInt();
        if(!query.exec(QString("INSERT INTO FRIENDINFO(USERID, FRIENDID) VALUES("
                               "%1, %2)").arg(QString::number(id), QString::number(id))))
        {
            qDebug() << "插入好友信息发生错误";
            qDebug() << query.lastError();
        }
    }
    obj.insert("command","registerBack");
    obj.insert("id", QJsonValue(id));
    doc = QJsonDocument(obj);
    bArry = doc.toJson();
    return bArry;
}

QByteArray SqlAccountModel::checkAccount(const int& userID, const QString &userPassword)
{
    bool rel;
    QString password;
    int id;
    QJsonObject obj;
    QJsonDocument doc;
    QByteArray bArry;

    setTable(accountTableName);
    setFilter(QString("ID = %1").arg(userID));
    select();

    if(rowCount()==0)
    {
        qDebug() << "该用户不存在";
        rel = false;
    }
    else
    {
        QSqlRecord record = QSqlTableModel::record(0);
        id = record.value("ID").toInt();
        password = record.value("PASSWORD").toString();
        if(id==userID && password==userPassword)
        {
            rel = true;
        }
        else rel = false;
    }
    obj.insert("command","loginBack");
    obj.insert("result", QJsonValue(rel));
    doc = QJsonDocument(obj);
    bArry = doc.toJson();
    return bArry;
}

QByteArray SqlAccountModel::userInfo(const int &userID)
{
    QSqlQuery query;
    QByteArray bAry;
    QJsonObject finalObj;
    if(!query.exec(QString("SELECT * FROM USERINFO WHERE "
                           "ID=%1 ").arg(QString::number(userID))))
    {
        qDebug() << "选择个人信息发生错误";
        qDebug() << query.lastError();
    }
    else
    {
        qDebug() << "选择个人信息成功";
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
            finalObj.insert("result", QJsonValue(obj));
        }
        qDebug() << finalObj;
    }
    finalObj.insert("command", QJsonValue("userinfoback"));
    bAry = QJsonDocument(finalObj).toJson();
    return bAry;
}

void SqlAccountModel::updateIcon(const int& userID, const QString &iconURL)
{
    setTable(accountTableName);
    setFilter(QString("ID = %1").arg(userID));
    if(!select())
    {
        qDebug() << lastError();
    }
    else
    {
        QSqlRecord record = QSqlTableModel::record(0);
        record.setValue("ICON", iconURL);
        setRecord(0, record);
        if(!submitAll())
        {
           qDebug() << lastError();
        }
    }
}

void SqlAccountModel::updateUserInfo(const int &userID, const QString &name, const int &gender, const QString &birth, const QString &area, const QString &education, const QString &signature)
{
    setTable(accountTableName);
    setFilter(QString("ID = %1").arg(userID));
    if(!select())
    {
        qDebug() << lastError();
    }
    else
    {
        QSqlRecord record = QSqlTableModel::record(0);
        record.setValue("NAME", name);
        record.setValue("GENDER", gender);
        record.setValue("BIRTH", birth);
        record.setValue("AREA", area);
        record.setValue("EDUCATION", education);
        record.setValue("SIGNATURE", signature);
        setRecord(0, record);
        if(!submitAll())
        {
           qDebug() << lastError();
        }
    }
}

QByteArray SqlAccountModel::messageList(const int &ID)
{
    setTable(friendMessageTableName);
    QSqlQuery query;
    QSqlQuery query1;
    QJsonArray jsonItem;
    QJsonObject jsonTotalF;
    QJsonObject jsonTotalG;
    QByteArray bAry;
    QSet<int> set;
    QJsonObject finalObj;
    int lastID=0;
    if(!query.exec(QString("SELECT * FROM FRIENDMESSAGEINFO "
                           "WHERE SENDID=%1 OR RECEIVEID=%2 "
                           "ORDER BY ID ASC, DATETIME ASC").arg(QString::number(ID), QString::number(ID))))
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
            if(lastID==0 || lastID==query.value("ID").toInt())
            {

                lastID = query.value("ID").toInt();
                set.insert(query.value("SENDID").toInt());
                set.insert(query.value("RECEIVEID").toInt());
                obj.insert("sid", QJsonValue(query.value("SENDID").toInt()));
                obj.insert("rid", QJsonValue(query.value("RECEIVEID").toInt()));
                obj.insert("datetime", QJsonValue(query.value("DATETIME").toInt()));
                obj.insert("message", QJsonValue(query.value("MESSAGE").toString()));
                jsonItem.append(QJsonValue(obj));
            }
            else
            {
                set.remove(ID);
                jsonTotalF.insert(QString::number(*(set.begin())), QJsonValue(jsonItem));
                lastID = query.value("ID").toInt();
                set.clear();
                set.insert(query.value("SENDID").toInt());
                set.insert(query.value("RECEIVEID").toInt());
                while(!jsonItem.empty()) jsonItem.removeLast();
                obj.insert("sid", QJsonValue(query.value("SENDID").toInt()));
                obj.insert("rid", QJsonValue(query.value("RECEIVEID").toInt()));
                obj.insert("datetime", QJsonValue(query.value("DATETIME").toInt()));
                obj.insert("message", QJsonValue(query.value("MESSAGE").toString()));
                jsonItem.append(QJsonValue(obj));
            }
        }
        set.remove(ID);
        jsonTotalF.insert(QString::number(*(set.begin())), QJsonValue(jsonItem));
        finalObj.insert("friendlist", QJsonValue(jsonTotalF));
    }
    lastID = 0;
    while(!jsonItem.empty()) jsonItem.removeLast();
    if(!query.exec(QString("SELECT GROUPMESSAGEINFO.GID AS GID, GROUPMESSAGEINFO.MID AS MID, GROUPMESSAGEINFO.DATETIME AS DATETIME, GROUPMESSAGEINFO.MESSAGE AS MESSAGE "
                           "FROM MEMBERINFO join GROUPMESSAGEINFO ON MEMBERINFO.GID=GROUPMESSAGEINFO.GID "
                           "WHERE MEMBERINFO.MID=%1 ORDER BY GID ASC, DATETIME ASC").arg(QString::number(ID))))
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
            if(lastID==0 || lastID==query.value("GID"))
            {
                lastID = query.value("GID").toInt();
                obj.insert("mid", QJsonValue(query.value("MID").toInt()));
                obj.insert("datetime", QJsonValue(query.value("DATETIME").toInt()));
                obj.insert("message", QJsonValue(query.value("MESSAGE").toString()));
                jsonItem.append(QJsonValue(obj));
            }
            else
            {
                jsonTotalG.insert(QString::number(lastID), QJsonValue(jsonItem));
                lastID = query.value("GID").toInt();
                while(!jsonItem.empty()) jsonItem.removeLast();
                obj.insert("mid", QJsonValue(query.value("MID").toInt()));
                obj.insert("datetime", QJsonValue(query.value("DATETIME").toInt()));
                obj.insert("message", QJsonValue(query.value("MESSAGE").toString()));
                jsonItem.append(QJsonValue(obj));
            }
        }
        jsonTotalG.insert(QString::number(lastID), QJsonValue(jsonItem));
        finalObj.insert("grouplist", QJsonValue(jsonTotalG));
    }
    finalObj.insert("command", QJsonValue("messageBack"));
    qDebug() << finalObj;
    bAry = QJsonDocument(finalObj).toJson();
    return bAry;
}























