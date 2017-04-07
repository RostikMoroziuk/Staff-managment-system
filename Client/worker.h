#ifndef WORKER_H
#define WORKER_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QVector>
#include <QPair>
#include <person.h>
#include <QDate>
#include <QDebug>

namespace Ui {
class Worker;
}

class Worker : public QMainWindow
{
    Q_OBJECT

public:
    explicit Worker(WorkerPerson, QDate);
    ~Worker();

signals:
    void changeAccount();

private slots:
    void on_saveChange_clicked();

    void on_exit_clicked();

private:
    Ui::Worker *ui;
    QTcpSocket *socket;//тільки надсилати до сервера
    WorkerPerson *me;
    QDate date;

    void setInfo();
    void setInfoEdit();

    enum {authorization, addWorker, addAdmin, deleteWorker, deleteAdmin, takeMoney, addMoney,
          showWorkers, showAdmins, saveChanges, saveMyChanges};
};

#endif // WORKER_H
