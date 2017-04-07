#include "worker.h"
#include "ui_worker.h"
#include <QHostAddress>
#include <QTableWidgetItem>

Worker::Worker(WorkerPerson person, QDate ns) :
    ui(new Ui::Worker)
{
    ui->setupUi(this);
    setWindowTitle(person.nickName);

    me = new WorkerPerson(person);
    date = ns;
    ui->dateEdit->setDate(date);

    setInfo();
    setInfoEdit();

    ui->history->setHorizontalHeaderItem(0, new QTableWidgetItem("Date"));
    ui->history->setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));
    ui->history->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->history->horizontalHeader()->setVisible(true);
    ui->history->setRowCount(me->history.size());

    for(int i=0;i<me->history.size();i++)
    {
        ui->history->setItem(i, 0, new QTableWidgetItem(me->history[i].first));//дата
        ui->history->setItem(i, 1, new QTableWidgetItem(me->history[i].second));//значення
        qDebug()<<me->history[i].first<<me->history[i].second;
    }

    ui->history->resizeColumnsToContents();
    ui->history->resizeRowsToContents();

    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 1234);
}

Worker::~Worker()
{
    delete ui;
    delete socket;
    delete me;
}

void Worker::setInfo()
{
    ui->surname->setText(me->surname);
    ui->name->setText(me->name);
    ui->ID->setText(me->ID);
    ui->enterDate->setText(me->enterDate);
    ui->position->setText(me->position);
    ui->salary->setText(me->salary);
    ui->bonus->setText(me->bonus);
}

void Worker::setInfoEdit()
{
    ui->surname_edit->setText(me->surname);
    ui->name_edit->setText(me->name);
    ui->login_edit->setText(me->nickName);
    ui->password_edit->setText(me->password);
}




void Worker::on_saveChange_clicked()
{
    QByteArray ba;
    QDataStream msg(&ba, QIODevice::WriteOnly);
    msg.setVersion(QDataStream::Qt_5_5);
    msg<<saveMyChanges<<me->nickName;//старий нік
    me->nickName=ui->login_edit->text();
    me->password = ui->password_edit->text();
    me->surname = ui->surname_edit->text();
    me->name = ui->name_edit->text();
    socket->write(ba);

    setInfo();
}

void Worker::on_exit_clicked()
{
    emit changeAccount();
}
