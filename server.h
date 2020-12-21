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

        /*!
         * \brief Called when a new client acheived to connect
        */
        void NewConnection();

    private:
        QLabel *ServerState; /*!< Pointer to the label displayed to show the server state */
        QPushButton *QuitButton; /*!< Pointer to the quit button t */
        QTcpServer *serveur; /*!< Pointer to the TCP Server */

        QMutex* mapMutex; /*!< mutex to manage shared map */

        QMap<QString, std::pair<QTcpSocket *, QString>>* mapClients; /*!< Shared map between threads to have access to the public key and pseudo */
};

#endif
