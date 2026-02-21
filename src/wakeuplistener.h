#ifndef WAKEUPLISTENER_H
#define WAKEUPLISTENER_H

/**
 * @file wakeuplistener.h
 * @brief Erkennung mehrerer Instanzen und Weiterleitung an die laufende Instanz.
 */

#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QPointer>
#include <QWidget>
#include <QObject>

/**
 * @brief Verhindert mehrfaches Öffnen der Anwendung und bringt das Hauptfenster
 *        der bereits laufenden Instanz in den Vordergrund.
 *
 * WakeUpListener verwendet QLocalServer/-Socket als IPC-Mechanismus:
 * - Die **erste** Instanz startet einen lokalen Server und wartet auf Verbindungen.
 * - Jede weitere Instanz verbindet sich, sendet "BringToFront" und beendet sich.
 *
 * Das Protokoll:
 * 1. Neue Instanz verbindet sich zum Server der laufenden Instanz.
 * 2. Neue Instanz sendet @c "BringToFront".
 * 3. Server bringt das Fenster in den Vordergrund, sendet @c "SUCCESS" und
 *    emittiert das Signal bringToFront().
 * 4. Neue Instanz sieht @c isSecondInstance() == true und soll beendet werden.
 *
 * @note Das Singleton-Muster wird durch QLocalServer’s exklusiven Bind-Mechanismus
 *       erzwungen — nicht durch globale Variablen. Die Klasse ist damit
 *       wiederverwendbar und testbar.
 *
 * @par Verwendung (in main()):
 * @code
 * int main(int argc, char *argv[]) {
 *     QApplication app(argc, argv);
 *     app.setOrganizationName("MeineOrg");
 *     app.setApplicationName("MeineApp");
 *
 *     MainWindow w;
 *     WakeUpListener wul(&w, app.applicationName());
 *     if (wul.isSecondInstance())
 *         return 0;  // Bestehende Instanz wurde benachrichtigt, diese beenden
 *
 *     w.show();
 *     return app.exec();
 * }
 * @endcode
 */
class WakeUpListener : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Gibt true zurück wenn diese Instanz eine Zweitinstanz ist.
     *
     * Wert ist direkt nach dem Konstruktor gültig. Ist er true, sollte
     * die Anwendung umgehend mit @c return 0 aus main() beendet werden,
     * da die bereits laufende Instanz bereits benachrichtigt wurde.
     */
    bool isSecondInstance() const { return m_isSecondInstance; }

    /**
     * @brief Konstruktor — prüft sofort ob eine Instanz läuft.
     * @param widget   Hauptfenster, das in den Vordergrund gebracht wird.
     * @param appName  Eindeutiger Anwendungsname (wird als Socket-Name verwendet).
     * @param parent   Optionaler QObject-Elternteil.
     */
    WakeUpListener(QWidget *widget, const QString &appName, QObject *parent = nullptr)
        : QObject(parent),
        server(nullptr),
        widget(widget),
        m_isSecondInstance(false)
    {
        // Kurzer Verbindungsversuch zum Server der laufenden Instanz.
        // Timeout 500 ms reicht für lokale Sockets auf demselben Rechner.
        QLocalSocket socket;
        socket.connectToServer(appName + "Server");
        if (socket.waitForConnected(500)) {
            // Verbindung zur laufenden Instanz hergestellt — Aufweckbefehl senden
            socket.write("BringToFront");
            if(!socket.waitForBytesWritten())
                qDebug() << "WakeUpListener: Failed to send wakeup message";

            if (socket.waitForReadyRead(500)) {
                if(socket.readAll() == "SUCCESS") {
                    socket.disconnectFromServer();
                    m_isSecondInstance = true; // Aufrufer soll die Anwendung beenden
                    return;
                } else {
                    qWarning() << "WakeUpListener: Received wrong response from existing instance";
                }
            } else {
                qWarning() << "WakeUpListener: Failed to receive acknowledgment from existing instance";
            }

            socket.disconnectFromServer();
            return;
        } else {
            // Keine laufende Instanz gefunden — dieses Exemplar wird zur Erstinstanz
            server = new QLocalServer(this);
            connect(server, &QLocalServer::newConnection, this, &WakeUpListener::handleConnection);
            if (!server->listen(appName + "Server")) {
                qWarning() << "WakeUpListener: Failed to start local server:" << server->errorString();
            }
        }
    }

    ~WakeUpListener() {
        if(server) {
            server->close();
        }
    }

signals:
    /**
     * @brief Wird emittiert wenn eine Zweitinstanz diese Instanz aufgeweckt hat.
     *
     * Kann verwendet werden um zusätzliche Aktionen beim Aufwecken durchzuführen
     * (z.B. Splash-Screen schließen, Fenster maximieren).
     */
    void bringToFront();

private slots:
    /**
     * @brief Behandelt eine eingehende IPC-Verbindung.
     *
     * Liest "BringToFront", bringt widget in den Vordergrund, antwortet
     * mit "SUCCESS" und emittiert bringToFront().
     */
    void handleConnection() {
        QLocalSocket *socket = server->nextPendingConnection();
        if (!socket)
            return;

        connect(socket, &QLocalSocket::readyRead, this, [socket, this]() {
            QByteArray data = socket->readAll();
            if (data == "BringToFront") {
                if(widget) {
                    widget->raise();
                    widget->activateWindow();
                    widget->show();
                }
                socket->write("SUCCESS");
                socket->flush();
                emit bringToFront();
            }
            socket->disconnectFromServer();
            socket->deleteLater();
        });
    }

private:
    QLocalServer     *server;            ///< Lokaler IPC-Server (Erstinstanz)
    QPointer<QWidget> widget;            ///< Hauptfenster (QPointer: keine dangling-pointer-Gefahr)
    bool              m_isSecondInstance;///< true wenn eine laufende Instanz gefunden wurde
};

#endif // WAKEUPLISTENER_H
