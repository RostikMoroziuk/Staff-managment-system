#ifndef ADDWORKER_H
#define ADDWORKER_H

#include <QDialog>
#include "admin.h"
#include "person.h"

namespace Ui {
class AddWorker;
}

class Admin;

class AddWorker : public QDialog
{
    Q_OBJECT

public:
    explicit AddWorker(Admin* adm, bool isAdmin, QWidget *parent = 0);
    ~AddWorker();

private slots:
    void on_pushButton_clicked();

    void on_addWorker_clicked();

private:
    Ui::AddWorker *ui;
    Admin* adm;
    bool isAdmin;
};

#endif // ADDWORKER_H
