#ifndef WAKEUPLISTENER_H
#define WAKEUPLISTENER_H

#include <QLocalServer>
#include <QLocalSocket>
#include <QWidget>
#include <QObject>
#include <QMessageBox>



class WakeUpListener : public QObject {
    Q_OBJECT
public:
    WakeUpListener(QWidget *widget, QString appName, QObject *parent = nullptr)
        : QObject(parent),
        server(nullptr),
        widget(widget)
    {

        QLocalSocket socket;
        socket.connectToServer(appName + "Server");
        if (socket.waitForConnected(500)) {
            // Connection succeeded, send a message to the existing instance to bring it to the foreground
            socket.write("BringToFront");
            if(!socket.waitForBytesWritten())
                qDebug() << " Failed to send wakeup message to local app";

            // Wait for the acknowledgment message from the existing instance
            if (socket.waitForReadyRead(500)) {
                // Successfully received acknowledgment, disconnect and return
                if(socket.readAll() == "SUCCESS") {; // Clear any incoming data
                    socket.disconnectFromServer();
                    exit(0);
                } else {
                    QMessageBox::warning(nullptr, "Warning", "Received wrong response from existing instance!");
                }
            } else {
                // Failed to receive acknowledgment, show a warning
                QMessageBox::warning(nullptr, "Warning", "Failed to receive acknowledgment from existing instance.");
            }

            socket.disconnectFromServer();
            return;
        } else {
            server = new QLocalServer(this);
            connect(server, &QLocalServer::newConnection, this, &WakeUpListener::handleConnection);
            server->listen(appName + "Server");
        }

    }

    ~WakeUpListener() {
        if(server) {
            server->disconnect();
            delete server;
        }
    }

private slots:
    void handleConnection() {
        QLocalSocket *socket = server->nextPendingConnection();
        if (!socket)
            return;

        connect(socket, &QLocalSocket::readyRead, this, [socket, this]() {
            QByteArray data = socket->readAll();
            if (data == "BringToFront") {
                widget->raise();
                widget->activateWindow();
                widget->show();
                socket->write("SUCCESS");
            }
            socket->disconnectFromServer();
        });
    }

private:
    QLocalServer *server;
    QWidget *widget;
};

#endif // WAKEUPLISTENER_H
