#include "addworker.h"
#include "ui_addworker.h"

AddWorker::AddWorker(Admin* admin, bool isAdmin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddWorker)
{
    ui->setupUi(this);
    this->isAdmin = isAdmin;
    adm = admin;

    if(isAdmin)//якщо адміністратор то змінюємо структуру
    {
        ui->groupBox_2->setEnabled(false);
        ui->enterDate->setEnabled(false);
        ui->position->setEnabled(false);
        ui->label_4->setEnabled(false);
        ui->label_5->setEnabled(false);
        ui->ID->setEnabled(false);
        ui->addWorker->setText(QString("Add admin"));
    }
    else
        ui->enterDate->setText(QDate::currentDate().toString());
}

AddWorker::~AddWorker()
{
    delete ui;
}

void AddWorker::on_addWorker_clicked()
{
    if(isAdmin)
    {
        AdminPerson admin;
        admin.nickName = ui->nickName->text();
        admin.password = ui->password->text();
        admin.surname = ui->surname->text();
        admin.name = ui->name->text();

        adm->addNewAdmin(admin);
        close();
    }
    else
    {
        WorkerPerson worker;
        worker.nickName = ui->nickName->text();
        worker.password = ui->password->text();
        worker.surname = ui->surname->text();
        worker.name = ui->name->text();
        worker.ID = ui->ID->text();
        worker.enterDate = ui->enterDate->text();
        worker.position = ui->position->currentText();
        worker.salary = ui->salary->text();
        worker.bonus = ui->bonus->text();
        adm->addNewWorker(worker);
        close();
    }
}
