#ifndef THREADSERVER_H
#define THREADSERVER_H

#include <QThread>
#include <QtNetwork>
#include <QMutex>

class ThreadServer : public QThread
{
    Q_OBJECT

public:
    explicit ThreadServer(QMutex* mapMutex,QMap<QString, std::pair<QTcpSocket *, QString>>* mapCients, QTcpSocket *socket, QObject *parent = 0);

    void run() override;

signals :
    void error(QTcpSocket::SocketError socketError);

public slots:
    void readyRead();
    void disconnected();

private slots:
    void SendToAll(const QString &message);
    void SendTo(const QString &message, QTcpSocket*);
    QString ClientsConnected();

private:
    QTcpSocket *socket;

    QMutex* sharedMapMutex;
    QMap<QString, std::pair<QTcpSocket *, QString>>* sharedMapClients; // Pseudo , <socket , pubkey>

    quint16 MessageSize;

    const bool verbose = false;

};

#endif // THREADSERVER_H
