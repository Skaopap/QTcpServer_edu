#ifndef THREADSERVER_H
#define THREADSERVER_H

#include <QThread>
#include <QtNetwork>
#include <QMutex>

//!  Server Thread.
/*!
  One thread deals with one client.
*/
class ThreadServer : public QThread
{
    Q_OBJECT

public:
    explicit ThreadServer(QMutex* mapMutex,QMap<QString, std::pair<QTcpSocket *, QString>>* mapCients, QTcpSocket *socket, QObject *parent = 0);

    /*!
     * \brief Override QThread function, run the thread
    */
    void run() override;

signals:

    /*!
     * \brief Send the recieving data to the server to display it
    */
    void SendMessageToDisplay(QString);

public slots:

    /*!
     * \brief Called when a client is sending data
    */
    void readyRead();

    /*!
     * \brief Called when the client is disconnected
    */
    void disconnected();

private slots:

    /*!
     * \brief Send message to every connected clients
     * \param message The message to send
    */
    void SendToAll(const QString &message);

    /*!
     * \brief Send message to a specific clients
     * \param message The message to send
    */
    void SendTo(const QString &message, QTcpSocket*);

    /*!
     * \brief Get the connected clients
     * \return A message with the pseudo of all connected clients
    */
    QString ClientsConnected();

private:
    QTcpSocket *socket; /*!< Pointer to the client TCP socket */
    QString SocketPseudo; /*!< Pointer to the Client Pseudo */

    QMutex* sharedMapMutex; /*!< The shared mutex to managed the map */
    QMap<QString, std::pair<QTcpSocket *, QString>>* sharedMapClients;  /*!< The shared map : Pseudo , <socket , pubkey> */ //

    quint16 MessageSize; /*!< Current size of the receiving message */

    const bool verbose = false; /*!< Verbose Mode*/

};

#endif // THREADSERVER_H
