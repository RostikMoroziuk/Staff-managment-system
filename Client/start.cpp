#include "ui_start.h"
#include "start.h"
#include <QByteArray>
#include <QMessageBox>
#include <QPair>
#include <QDate>

Start::Start(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Start)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 1234);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readAnswer()));
    connect(ui->enter, SIGNAL(clicked(bool)), this, SLOT(send()));
}

Start::~Start()
{
    socket->close();
    delete ui;
}

void Start::readAnswer()
{
    QDataStream reader(socket);
    reader.setVersion(QDataStream::Qt_5_5);
    int command;
    reader>>command;
    switch(command)
    {
    case authorization:
        bool ok, isAdmin;
        reader>>ok>>isAdmin;
        if(ok)
        {
            if(isAdmin)
            {
                this->hide();
                QString surname, name;
                QDate nextSalary;
                reader>>surname>>name>>nextSalary;
                adm = new Admin(ui->login->text(), ui->password->text(), surname, name, nextSalary);
                adm->show();

                connect(adm, SIGNAL(changeAccount()), this, SLOT(show()));
                connect(adm, SIGNAL(changeAccount()), adm, SLOT(deleteLater()));
                connect(this, SIGNAL(destroyed(QObject*)), adm, SLOT(deleteLater()));
            }
            else
            {
                this->hide();
                WorkerPerson workerPerson;
                QDate date;

                reader>>workerPerson.nickName>>workerPerson.password>>workerPerson.surname>>workerPerson.name>>workerPerson.ID >>workerPerson.enterDate
                        >>workerPerson.position>>workerPerson.salary>>workerPerson.bonus>>workerPerson.history>>date;
                worker = new Worker(workerPerson, date);
                worker->show();

                connect(worker, SIGNAL(changeAccount()), worker, SLOT(deleteLater()));
                connect(worker, SIGNAL(changeAccount()), this, SLOT(show()));
                connect(this, SIGNAL(destroyed(QObject*)), adm, SLOT(deleteLater()));
            }
        }
        else
        {
            QMessageBox::critical(NULL,"Error", "Incorrectly login or password");
        }
        break;
    }
}

void Start::send()
{
    QByteArray ba;
    QDataStream writer(&ba, QIODevice::WriteOnly);
    writer.setVersion(QDataStream::Qt_5_5);
    writer<<authorization<<ui->login->text()<<ui->password->text();
    socket->write(ba);
}


