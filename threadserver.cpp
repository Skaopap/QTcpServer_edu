#include "threadserver.h"

ThreadServer::ThreadServer(QMutex* mapMutex,QMap<QString, std::pair<QTcpSocket *, QString>>* mapCients, QTcpSocket *psocket, QObject *parent) :
    QThread(parent)
{
    if(verbose)
        qDebug() << "ThreadServer";

    sharedMapMutex = mapMutex;
    sharedMapClients = mapCients;
    socket = psocket;
    MessageSize=0;
}

void ThreadServer::run()
{
    if(verbose)
        qDebug() << "ThreadServer::run";

    // thread starts here
    qDebug() << " Thread started";

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    exec();
}

void ThreadServer::readyRead()
{
    if(verbose)
        qDebug() << "ThreadServer::readyRead";

    //Recieve paquet from client

    // get msg
    QDataStream in(socket);

    if (MessageSize == 0) // if msg sized unknown, try to get it
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16)) // msg not recieved entierly
             return;

        in >> MessageSize; // store the size
    }

    // Check if the whole msg has arrived, if not : wait
    if (socket->bytesAvailable() < MessageSize)
        return;


    // Go
    QString message;
    in >> message;

    qDebug() << "Server msg recu : "<< message;
    //qDebug() << *sharedMapClients;

    sharedMapMutex->lock();

    // Register Pseudo
    if(message.indexOf("P:") == 0 && message.indexOf("PuBKeY") != -1 )
    {
        int indexOfPK = message.lastIndexOf(" PuBKeY");
        QString pseudo = message.mid(2).left(indexOfPK-2);
        SocketPseudo = pseudo;

        if(!sharedMapClients->contains(pseudo) )
        {
            QString pubkey = message.mid(indexOfPK + 7);

            (*sharedMapClients)[pseudo] = std::make_pair(socket,pubkey);
            sharedMapMutex->unlock();
            SendToAll(QString("Welcome to : " + pseudo + "\n" + ClientsConnected()));
        }
        else
        {
            sharedMapMutex->unlock();
            SendTo(pseudo+ " already used, choose another one", socket);
        }

    }
    // Asking for clients PubKey
    else if(message.indexOf("CoNNeCTTo") == 0)
    {
        //qDebug() << sharedMapClients;
        QString pubkey;
        if(!sharedMapClients->contains(message.mid(9)))
        {
            pubkey = "";
        }
        else
            pubkey = (*sharedMapClients)[message.mid(9)].second;

        // APK for answer public key
        sharedMapMutex->unlock();
        SendTo(QString("APK")+pubkey,socket);
    }
    // PubKey changed
    /*else if(message.indexOf("PpUbChAnGeD:") == 0)
    {
        int indexOfPK = message.lastIndexOf(" PubKey");
        QString pseudo = message.mid(12).left(indexOfPK-12);
        if(pseudo == SocketPseudo)
        {
             (*sharedMapClients)[pseudo].second = message.mid(indexOfPK + 7);
        }
        sharedMapMutex->unlock();
    }*/
    // Send crypted message
    else if (message.indexOf("PsEUdO:") == 0)
    {

        int indexOfMSG = message.lastIndexOf(" MSG:");

        QString pseudo = message.mid(7); // .left(indexOfMSG);
        pseudo = pseudo.left(pseudo.lastIndexOf(" MSG:"));

        QString msg;

        if(!sharedMapClients->contains(pseudo))
        {
            msg= pseudo + " is disconected ";
            sharedMapMutex->unlock();
            SendTo(msg, socket);
        }
        else
        {
            msg = "CrYpTEd:" + message.mid(indexOfMSG + 5);
            sharedMapMutex->unlock();
            SendTo(msg, (*sharedMapClients)[pseudo].first );
        }
    }
    // General chat, clear msg
    else
    {
        sharedMapMutex->unlock();
        SendToAll(message);
    }

    MessageSize = 0;
}

void ThreadServer::disconnected()
{
    if(verbose)
        qDebug() << "ThreadServer::disconnected";

    // remove from map
    sharedMapMutex->lock();
    QMapIterator<QString, std::pair<QTcpSocket *, QString>> i(*sharedMapClients);
    QString key = "";
    while (i.hasNext() && key == "")
    {
        i.next();
        if(i.value().first == socket)
            key = i.key();
    }
    sharedMapClients->remove(key);

    sharedMapMutex->unlock();

    // send connection infos to all
    SendToAll(tr("<em>Client deconnexion..</em> \n") + ClientsConnected() );

    socket->deleteLater();

    exit(0);
}

void ThreadServer::SendToAll(const QString &message)
{
    if(verbose)
        qDebug() << "ThreadServer::SendToAll";

    // Préparation du paquet
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    // write paquet
    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    sharedMapMutex->lock();
    //send paquet to all
    QMapIterator<QString, std::pair<QTcpSocket *, QString>> i(*sharedMapClients);
    while (i.hasNext())
    {
        i.next();
        i.value().first->write(paquet);
    }
    sharedMapMutex->unlock();

}

void ThreadServer::SendTo(const QString &message, QTcpSocket* client)
{
    if(verbose)
        qDebug() << "ThreadServer::SendTo";

    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0;
    out << message;
    out.device()->seek(0);
    out << (quint16) (paquet.size() - sizeof(quint16));

    client->write(paquet);
}

QString ThreadServer::ClientsConnected()
{
    if(verbose)
        qDebug() << "ThreadServer::ClientsConnected";

    QString msg = "Now connected : ";
    sharedMapMutex->lock();
    // Create msg
    QMapIterator<QString, std::pair<QTcpSocket *, QString>> i(*sharedMapClients);
    while (i.hasNext())
    {
        i.next();
        msg += i.key() + " / ";
    }
    sharedMapMutex->unlock();

    return msg;
}
