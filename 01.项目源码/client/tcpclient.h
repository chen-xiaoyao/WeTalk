#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QThread>

class TcpClient : public QThread
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);

signals:

};

#endif // TCPCLIENT_H
