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

        // Start a thread with each new connection
        void NewConnection();

    private:
        QLabel *ServerState;
        QPushButton *QuitButton;
        QTcpServer *serveur;

        QMutex* mapMutex; // mutex to manage shared map

        // Shared map between threads to have access to the public key and pseudo
        QMap<QString, std::pair<QTcpSocket *, QString>>* mapClients;
};

#endif
