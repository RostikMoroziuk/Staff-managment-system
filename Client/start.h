#ifndef START_H
#define START_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QDataStream>
#include "admin.h"
#include "worker.h"
#include <QDebug>

namespace Ui {
class Start;
}

class Start : public QDialog
{
    Q_OBJECT

public:
    explicit Start(QWidget *parent = 0);
    ~Start();

public slots:
    void readAnswer();
    void send();

private:
    Ui::Start *ui;
    QTcpSocket *socket;
    Admin* adm;
    Worker *worker;

    enum {authorization, addWorker, addAdmin, deleteWorker, deleteAdmin, takeMoney, addMoney,
          showWorkers, showAdmins, saveChanges, saveMyChanges};
};

#endif // START_H
