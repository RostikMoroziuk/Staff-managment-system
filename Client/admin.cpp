#include "admin.h"
#include "ui_admin.h"
#include <QDataStream>
#include <QHostAddress>
#include <QFile>
#include <QDebug>
#include "mytabbar.h"
#include <QDateEdit>
#include <QDate>
#include <QTimer>

Admin::Admin(QString login, QString pass, QString surname, QString name, QDate nS, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Admin)
{
    ui->setupUi(this);

    ui->login->setText(login);
    ui->password->setText(pass);
    ui->surname_settings->setText(surname);
    ui->name_settings->setText(name);
    me.nickName = login;
    me.password = pass;
    me.surname = surname;
    me.name = name;
    ui->nextSalary->setDate(nS);

    ui->adminName->setText(QString("Admin: ")+surname + QString(" ") + name);
    ui->adminName_editWorkers->setText(QString("Admin: ")+surname + QString(" ") + name);
    ui->adminName_editAdmins->setText(QString("Admin: ")+surname + QString(" ") + name);
    ui->adminName_settings->setText(QString("Admin: ")+surname + QString(" ") + name);

    ui->workers->setModel(ui->workers_editWorkers->model());//обидва комбо бокси мають спільну модель

    setWindowTitle(login);
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 1234);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readServer()));
    connect(ui->addWorker, SIGNAL(clicked(bool)), this, SLOT(slotAddWorker()));
    connect(ui->workers, SIGNAL(currentIndexChanged(int)), this, SLOT(setInfo(int)));
    connect(ui->admins, SIGNAL(currentIndexChanged(int)), this, SLOT(setInfoAdmins(int)));
    connect(ui->paySalary, SIGNAL(clicked(bool)), this, SLOT(slotAddMoney()));
    connect(ui->delWorker, SIGNAL(clicked(bool)), this, SLOT(slotDeleteWorker()));
    connect(ui->workers, SIGNAL(currentIndexChanged(int)), ui->workers_editWorkers, SLOT(setCurrentIndex(int)));
    connect(ui->workers_editWorkers, SIGNAL(currentIndexChanged(int)), ui->workers, SLOT(setCurrentIndex(int)));

    QByteArray ba;
    QDataStream msg(&ba, QIODevice::WriteOnly);
    msg.setVersion(QDataStream::Qt_5_5);
    msg<<showWorkers;
    socket->write(ba);
}

Admin::~Admin()
{
    delete ui;
}

void Admin::addNewWorker(WorkerPerson wp)
{
    workers.push_back(wp);
}

void Admin::addNewAdmin(AdminPerson ap)
{
    admins.push_back(ap);
}

void Admin::slotAddWorker()
{
    AddWorker addWrk(this, false);
    addWrk.show();
    addWrk.exec();

    ui->workers->addItem(workers.last().surname);

    QByteArray ba;
    QDataStream writer(&ba, QIODevice::WriteOnly);
    writer.setVersion(QDataStream::Qt_5_5);
    writer<<addWorker;
    writer<<workers.last().nickName<<workers.last().password<<workers.last().surname<<workers.last().name
                   <<workers.last().ID<<workers.last().enterDate<<workers.last().position<<workers.last().salary<<workers.last().bonus;
    socket->write(ba);
}

void Admin::setInfo(int index)
{
    //administration
    ui->surname->setText(workers[index].surname);
    ui->name->setText(workers[index].name);
    ui->ID->setText(workers[index].ID);
    ui->enterDate->setText(workers[index].enterDate);
    ui->position->setText(workers[index].position);
    ui->salary->setText(workers[index].salary);
    ui->bonus->setText(workers[index].bonus);

    //edit workers
    ui->surname_editWorkers->setText(workers[index].surname);
    ui->name_editWorkers->setText(workers[index].name);
    ui->position_editWorkers->setCurrentText(workers[index].position);
    ui->salary_editWorkers->setText(workers[index].salary);
    ui->bonus_editWorkers->setText(workers[index].bonus);
}

void Admin::setInfoAdmins(int index)
{
    //edit admins
    ui->surname_editAdmins->setText(admins[index].surname);
    ui->name_editAdmins->setText(admins[index].name);
}

void Admin::slotAddMoney()
{
    QByteArray ba;
    QDataStream writer(&ba, QIODevice::WriteOnly);
    writer.setVersion(QDataStream::Qt_5_5);
    writer<<addMoney;
    int money = ui->salary->text().toInt()+ui->salary->text().toInt()*ui->bonus->text().toInt()/100;
    writer<<workers[ui->workers->currentIndex()].nickName;
    writer<<QString::number(money);
    socket->write(ba);
}


void Admin::on_payOneTimeBonus_clicked()
{
    QByteArray ba;
    QDataStream writer(&ba, QIODevice::WriteOnly);
    writer.setVersion(QDataStream::Qt_5_5);
    writer<<addMoney;
    QString money = ui->oneTimeBonus->text();
    writer<<workers[ui->workers->currentIndex()].nickName;
    writer<<money;
    socket->write(ba);
}


void Admin::slotDeleteWorker()
{
    QByteArray ba;
    QDataStream writer(&ba, QIODevice::WriteOnly);
    writer.setVersion(QDataStream::Qt_5_5);
    writer<<deleteWorker<<workers[ui->workers->currentIndex()].nickName;
    socket->write(ba);

    ui->workers->removeItem(ui->workers_editWorkers->currentIndex());
    workers.remove(ui->workers_editWorkers->currentIndex());
}

void Admin::readServer()
{
    QDataStream reader(socket);
    reader.setVersion(QDataStream::Qt_5_5);
    int command;
    reader>>command;
    switch(command)
    {
    case showAdmins:
        admins.clear();
        ui->admins->clear();
        while(!reader.atEnd())
        {
            AdminPerson admin;
            reader>>admin.nickName>>admin.surname>>admin.name;
            admins.push_back(admin);
            ui->admins->addItem(admin.nickName);
        }
        break;
    case showWorkers:
        workers.clear();
        ui->workers->clear();
        while(!reader.atEnd())
        {
            WorkerPerson worker;
            reader>>worker.nickName>>worker.surname>>worker.name
                    >>worker.ID>>worker.enterDate>>worker.position>>worker.salary>>worker.bonus;
            workers.push_back(worker);

            ui->workers->addItem(worker.surname);
        }
        QByteArray ba1;
        QDataStream msg1(&ba1, QIODevice::WriteOnly);
        msg1.setVersion(QDataStream::Qt_5_5);
        msg1<<showAdmins;
        socket->write(ba1);
        break;
    }
}

void Admin::verifyDay()
{
    date = QDate::currentDate();
    if(date==ui->nextSalary->date())
    {
        for(int i=0;i<workers.size();i++)
        {
            ui->workers->setCurrentIndex(i);
            slotAddMoney();
        }
        ui->nextSalary->setDate(date.addMonths(1));
    }
    else//кожні 24 години провіряємо чи не наступив час видачі зарплати
    {
        QTimer::singleShot(1000*60*60*24, this, SLOT(verifyDay()));
    }
}

void Admin::on_saveChange_editWorkers_clicked()
{
    int i = ui->workers_editWorkers->currentIndex();
    workers[i].surname = ui->surname_editWorkers->text();
    workers[i].name = ui->name_editWorkers->text();
    workers[i].position = ui->position_editWorkers->currentText();
    workers[i].salary = ui->salary_editWorkers->text();
    workers[i].bonus = ui->bonus_editWorkers->text();

    setInfo(i);//оновлюємо інформацію

    QByteArray ba;
    QDataStream msg(&ba, QIODevice::WriteOnly);
    msg.setVersion(QDataStream::Qt_5_5);
    msg<<saveChanges<<workers[i].nickName<<workers[i].surname<<workers[i].name<<workers[i].position<<workers[i].salary<<workers[i].bonus;
    socket->write(ba);
}

void Admin::on_addAdmin_clicked()
{
    AddWorker addAdm(this, true);
    addAdm.show();
    addAdm.exec();

    ui->admins->addItem(admins.last().nickName);

    QByteArray ba;
    QDataStream writer(&ba, QIODevice::WriteOnly);
    writer.setVersion(QDataStream::Qt_5_5);
    writer<<addAdmin;
    writer<<admins.last().nickName<<admins.last().password<<admins.last().surname<<admins.last().name;
    socket->write(ba);
}

void Admin::on_saveChange_editAdmins_clicked()
{
    int i = ui->admins->currentIndex();
    admins[i].surname = ui->surname_editAdmins->text();
    admins[i].name = ui->name_editAdmins->text();

    if(admins[i].nickName==ui->login->text())
    {
        ui->surname_settings->setText(ui->surname_editAdmins->text());
        ui->name_settings->setText(ui->name_editAdmins->text());
    }

    setInfoAdmins(i);//оновлюємо інформацію

    QByteArray ba;
    QDataStream msg(&ba, QIODevice::WriteOnly);
    msg.setVersion(QDataStream::Qt_5_5);
    msg<<saveChanges<<admins[i].nickName<<admins[i].surname<<admins[i].name;
    socket->write(ba);
}

void Admin::on_deleteAdmin_clicked()
{
    QByteArray ba;
    QDataStream writer(&ba, QIODevice::WriteOnly);
    writer.setVersion(QDataStream::Qt_5_5);
    writer<<deleteAdmin<<admins[ui->admins->currentIndex()].nickName;
    socket->write(ba);

    ui->admins->removeItem(ui->admins->currentIndex());
    workers.remove(ui->admins->currentIndex());
}


void Admin::on_saveChange_settings_clicked()
{
    for(int i=0;i<admins.size();i++)
    {
        if(admins[i].nickName==me.nickName)
        {
            admins[i].nickName = ui->login->text();
            admins[i].password = ui->password->text();
            admins[i].surname = ui->surname_settings->text();
            admins[i].name = ui->name_settings->text();
            QByteArray ba;
            QDataStream msg(&ba, QIODevice::WriteOnly);
            msg.setVersion(QDataStream::Qt_5_5);
            msg<<saveMyChanges<<me.nickName<<admins[i].nickName<<admins[i].password<<admins[i].surname<<admins[i].name<<ui->nextSalary->date();
            socket->write(ba);
            me.nickName = ui->login->text();
            break;
        }
    }
}

void Admin::on_exit_clicked()
{
    emit changeAccount();
}
