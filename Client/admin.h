#ifndef ADMIN_H
#define ADMIN_H

#include <QMainWindow>
#include <QByteArray>
#include "person.h"
#include "addworker.h"
#include <QTcpSocket>
#include <QDate>

namespace Ui {
class Admin;
}

class Admin : public QMainWindow
{
    Q_OBJECT

public:
    explicit Admin(QString, QString, QString, QString, QDate, QWidget *parent = 0);
    ~Admin();
    void addNewWorker(WorkerPerson wp);
    void addNewAdmin(AdminPerson ap);

signals:
    void changeAccount();

public slots:
    void slotAddWorker();
    void slotDeleteWorker();
    void slotAddMoney();
    void readServer();
    void verifyDay();

private slots:
    void setInfo(int);
    void setInfoAdmins(int);
    void on_payOneTimeBonus_clicked();
    void on_saveChange_editWorkers_clicked();
    void on_addAdmin_clicked();
    void on_saveChange_editAdmins_clicked();
    void on_deleteAdmin_clicked();
    void on_saveChange_settings_clicked();
    void on_exit_clicked();

private:
    Ui::Admin *ui;
    QVector<AdminPerson> admins;
    QVector<WorkerPerson> workers;
    QTcpSocket *socket;
    AdminPerson me;
    QDate date;

    enum {authorization, addWorker, addAdmin, deleteWorker, deleteAdmin, takeMoney, addMoney,
          showWorkers, showAdmins, saveChanges, saveMyChanges};
};

#endif // ADMIN_H
