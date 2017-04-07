#ifndef MYSERVER_H
#define MYSERVER_H 

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QByteArray>
#include <QList>
#include <QMap>
#include "person.h"
#include <QFile>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QXmlStreamWriter>
#include <QVector>
#include <QDate>

class MyServer : public QWidget
{
    Q_OBJECT

public:
    MyServer(QWidget *parent = 0);
    ~MyServer();

private:
    QTcpServer *server;
    QMap<QString, Admin> admins;
    QMap<QString, Worker> workers;
    QSystemTrayIcon* systemTray;
    QAction *exit;
    QDate date;

    enum {authorization, addWorker, addAdmin, deleteWorker, deleteAdmin, takeMoney, addMoney,
          showWorkers, showAdmins, saveChanges, saveMyChanges};

public slots:
    void incommingConnection();
    void listenClient();
};

#endif // MYSERVER_H
