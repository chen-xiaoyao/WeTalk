#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QThread
{
    Q_OBJECT
public:
    explicit TcpServer(QString filePath,int targetId,QString time,QObject *parent = nullptr);

signals:

private:
    //套接字
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    //文件相关


};

#endif // TCPSERVER_H
