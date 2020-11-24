#ifndef HEADER_SERVER
#define HEADER_SERVER

#include <QtWidgets>
#include <QtNetwork>
#include <QMutex>
#include "threadserver.h"


class Server : public QWidget
{
    Q_OBJECT

    public:
        Server();

    private slots:
        void NewConnection();

    private:
        QLabel *ServerState;
        QPushButton *QuitButton;
        QTcpServer *serveur;

        QMutex* mapMutex;
        QMap<QString, std::pair<QTcpSocket *, QString>>* mapClients;
};

#endif
