#include "server.h"
#include <QtDebug>
Server::Server()
{
    // init map and mutex
    mapMutex = new QMutex();
    mapClients = new QMap<QString, std::pair<QTcpSocket *, QString>>();

    // create window
    ServerState = new QLabel;
    QuitButton = new QPushButton(tr("Stop Server"));
    connect(QuitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ServerState);
    layout->addWidget(QuitButton);
    setLayout(layout);

    setWindowTitle(tr("Secure Server"));

    // init tcp server
    serveur = new QTcpServer(this);

    if (!serveur->listen(QHostAddress::Any, 50885)) // start it wit all available ip and port 50885
    {
        // if failed
        ServerState->setText(tr("Server failed to start. Reason :<br />") + serveur->errorString());
    }
    else
    {
        // if success
        ServerState->setText(tr("The server has started on the port : <strong>") + QString::number(serveur->serverPort()) + tr("</strong>.<br />Clients are know abled to connect."));
        connect(serveur, SIGNAL(newConnection()), this, SLOT(NewConnection()));
    }
}

void Server::NewConnection()
{
    QTcpSocket *newClient = serveur->nextPendingConnection();

    mapMutex->lock();
        ThreadServer* threadServer = new ThreadServer(mapMutex,mapClients,newClient,this);
    mapMutex->unlock();

    // once a thread is not needed, it will be beleted later
    connect(threadServer, SIGNAL(finished()), threadServer, SLOT(deleteLater()));

    threadServer->start();
}
