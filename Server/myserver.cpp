#include "myserver.h"
#include <QApplication>
#include <QDataStream>
#include <QMessageBox>
#include <QDebug>
#include <QVariant>
#include <QAction>
#include <QIcon>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QDebug>

MyServer::MyServer(QWidget *parent)
    : QWidget(parent)
{
    systemTray = new QSystemTrayIcon(this);

    QMenu *contextMenu = new QMenu();
    exit = new QAction("Вихід", this);
    contextMenu->addActions(QList<QAction*>()<<exit);
    systemTray->setContextMenu(contextMenu);
    systemTray->setIcon(QIcon(":/icon/93336518_large_7.png"));
    systemTray->show();

    //загружаємо інформацію з файла про користувачів
    QXmlStreamReader xmlReader;
    QXmlStreamAttributes attributes;
    QFile file("info.xml");
    if(file.open(QFile::ReadOnly))
    {
        xmlReader.setDevice(&file);

        while(xmlReader.name()!="admins")
        {
            if(xmlReader.name() == "nextSalary" && !xmlReader.isEndElement())
            {
                attributes = xmlReader.attributes();
                date.setDate(attributes.value("year").toInt(), attributes.value("month").toInt(),
                             attributes.value("day").toInt());
                qDebug()<<date;
            }
            qDebug()<<xmlReader.name();
            xmlReader.readNext();
        }
        while(!(xmlReader.name()=="admins" && xmlReader.isEndElement()))//поки не кінець списку адмінів
        {
            if(xmlReader.name() == "admin" && !xmlReader.isEndElement())
            {
                Admin admin;
                //починаємо парсити кожного адміна
                attributes = xmlReader.attributes();
                admin.nickName = attributes.value("nickName").toString();
                admin.password = attributes.value("password").toString();
                admin.surname = attributes.value("surname").toString();
                admin.name = attributes.value("name").toString();
                admins.insert(admin.nickName, admin);
            }
            xmlReader.readNext();
        }
        //зчитали workers
        while(xmlReader.name()!="workers")
        {
            xmlReader.readNext();
        }
        while(!(xmlReader.name()=="workers" && xmlReader.isEndElement()))//поки не кінець списку працівників
        {
            if(xmlReader.name() == "worker" && !xmlReader.isEndElement())
            {
                Worker worker;
                //починаємо парсити кожного працівника
                attributes = xmlReader.attributes();
                worker.nickName = attributes.value("nickName").toString();
                worker.password = attributes.value("password").toString();
                worker.surname = attributes.value("surname").toString();
                worker.name = attributes.value("name").toString();
                worker.ID = attributes.value("ID").toString();
                worker.enterDate = attributes.value("enterDate").toString();
                worker.position = attributes.value("position").toString();
                worker.salary = attributes.value("salary").toString();
                worker.bonus = attributes.value("bonus").toString();
                while(xmlReader.name()!="histories")
                {
                    xmlReader.readNext();
                }
                while(!(xmlReader.name()=="histories" && xmlReader.isEndElement()))
                {
                    if(xmlReader.name()=="history" && !xmlReader.isEndElement())
                    {
                        attributes = xmlReader.attributes();
                        worker.history.push_back(QPair<QString, QString>(attributes.value("date").toString(), attributes.value("balance").toString()));
                    }
                    xmlReader.readNext();
                }
                workers.insert(worker.nickName, worker);
            }
            if(!xmlReader.atEnd())
                xmlReader.readNext();
        }
        file.close();
    }

    //налаштовуємо серверний сокет на слухання клієнтів
    server = new QTcpServer(this);
    if(!server->listen(QHostAddress::Any, 1234))
    {
        QMessageBox::critical(NULL, "Server error", "Unable to start the server"+server->errorString());
        server->close();
        return;
    }
    connect(server, SIGNAL(newConnection()), this, SLOT(incommingConnection()));
    connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
}


MyServer::~MyServer()
{
    server->close();
    QXmlStreamWriter xmlWriter;
    QFile file("info.xml");
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0, "Error!", "Error opening file");
    }
    else
    {
        xmlWriter.setDevice(&file);
        xmlWriter.setAutoFormatting(true);

        /* Writes a document start with the XML version number.*/
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("nextSalary");
            xmlWriter.writeAttribute("year",QString::number(date.year()));
            xmlWriter.writeAttribute("month",QString::number(date.month()));
            xmlWriter.writeAttribute("day",QString::number(date.day()));
            xmlWriter.writeStartElement("admins");
                for(QMap<QString, Admin>::iterator adm = admins.begin();adm!=admins.end();adm++)
                {
                    xmlWriter.writeStartElement("admin");
                    xmlWriter.writeAttribute("nickName",adm.value().nickName);
                    xmlWriter.writeAttribute("password",adm.value().password);
                    xmlWriter.writeAttribute("surname",adm.value().surname);
                    xmlWriter.writeAttribute("name",adm.value().name);
                    xmlWriter.writeEndElement();
                }
            xmlWriter.writeEndElement();

            xmlWriter.writeStartElement("workers");
            for(QMap<QString, Worker>::iterator wrk = workers.begin();wrk!=workers.end();wrk++)
            {
                xmlWriter.writeStartElement("worker");
                xmlWriter.writeAttribute("nickName",wrk.value().nickName);
                xmlWriter.writeAttribute("password",wrk.value().password);
                xmlWriter.writeAttribute("surname",wrk.value().surname);
                xmlWriter.writeAttribute("name",wrk.value().name);
                xmlWriter.writeAttribute("ID",wrk.value().ID);
                xmlWriter.writeAttribute("enterDate",wrk.value().enterDate);
                xmlWriter.writeAttribute("position",wrk.value().position);
                xmlWriter.writeAttribute("salary",wrk.value().salary);
                xmlWriter.writeAttribute("bonus",wrk.value().bonus);
                    xmlWriter.writeStartElement("histories");
                    for(int j=0;j<wrk.value().history.size();j++)
                    {
                        xmlWriter.writeStartElement("history");
                        xmlWriter.writeAttribute("date",wrk.value().history[j].first);
                        xmlWriter.writeAttribute("balance",wrk.value().history[j].second);
                        xmlWriter.writeEndElement();
                    }
                    xmlWriter.writeEndElement();
                xmlWriter.writeEndElement();
            }
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        file.close();
    }

    delete systemTray;
}


void MyServer::incommingConnection()
{
    //створюємо сокет для керування клієнтом
    QTcpSocket* client = server->nextPendingConnection();
    connect(client, SIGNAL(readyRead()), this, SLOT(listenClient()));//сигнал кидається коли є готові, доступні для читання дані
    connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
}

void MyServer::listenClient()
{
    QTcpSocket *socket = (QTcpSocket*)sender();//sender вертає вказівник на об'єкт, який кинув сигнал
    QDataStream reader(socket);
    int command;
    reader>>command;
    switch(command)
    {
    case authorization:
    {
        QString nickName, password;
        reader>>nickName>>password;
        QMap<QString, Admin>::iterator adm = admins.find(nickName);
        QMap<QString, Worker>::iterator wrk = workers.find(nickName);
        if(adm!=admins.end())//якшо таке ім'я було знайдено
        {
            if(adm.value().password==password)//якшо пароль правильний
            {
                QByteArray ba;
                QDataStream msg(&ba, QIODevice::WriteOnly);
                msg.setVersion(QDataStream::Qt_5_5);
                msg<<authorization<<true;
                msg<<true;//адмін
                msg<<adm.value().surname<<adm.value().name<<date;
                socket->write(ba);
                break;//зробить вихід
            }
        }
        else if(wrk!=workers.end())
        {
            if(wrk.value().password==password)//якшо пароль правильний
            {
                QByteArray ba;
                QDataStream msg(&ba, QIODevice::WriteOnly);
                msg.setVersion(QDataStream::Qt_5_5);
                msg<<authorization<<true;
                msg<<false;//працівник
                msg<<wrk.value().nickName<<wrk.value().password<<wrk.value().surname<<wrk.value().name<<wrk.value().ID
                  <<wrk.value().enterDate<<wrk.value().position<<wrk.value().salary<<wrk.value().bonus<<wrk.value().history<<date;
                socket->write(ba);
                break;//зробить вихід
            }
        }
        //авторизація неуспішна
        else
        {
            QByteArray ba;
            QDataStream msg(&ba, QIODevice::WriteOnly);
            msg.setVersion(QDataStream::Qt_5_5);
            msg<<authorization<<false;
            socket->write(ba);
            break;
        }
    }
    case addWorker:
    {
        //записуємо нового працівника
        Worker worker;
        reader>>worker.nickName>>worker.password>>worker.surname>>worker.name
               >>worker.ID>>worker.enterDate>>worker.position>>worker.salary>>worker.bonus;
        workers.insert(worker.nickName, worker);
        break;
    }
    case addAdmin:
    {
        Admin admin;
        reader>>admin.nickName>>admin.password>>admin.surname>>admin.name;
        admins.insert(admin.nickName, admin);
        break;
    }
    case deleteWorker:
    {
        QString nickName;
        reader>>nickName;
        qDebug()<<nickName;
        qDebug()<<workers.remove(nickName);
        for(QMap<QString, Worker>::iterator it = workers.begin(); it!=workers.end();it++)
            qDebug()<<it.value().nickName;
        break;
    }
    case deleteAdmin:
    {
        QString nickName;
        reader>>nickName;
        admins.remove(nickName);
        break;
    }
    case addMoney:
    {
        QString nickName, money;
        reader>>nickName>>money;
        qDebug()<<nickName<<money;
        QMap<QString, Worker>::iterator it = workers.find(nickName);
        it.value().history.push_back(QPair<QString, QString>(QDateTime::currentDateTime().toString(), money));
        break;
    }
    case showWorkers:
    {
        QByteArray ba;
        QDataStream msg(&ba, QIODevice::WriteOnly);
        msg.setVersion(QDataStream::Qt_5_5);
        msg<<showWorkers;
        for(QMap<QString, Worker>::iterator it = workers.begin();it!=workers.end();it++)
        {
            msg<<it.value().nickName<<it.value().surname<<it.value().name
              <<it.value().ID<<it.value().enterDate<<it.value().position<<it.value().salary<<it.value().bonus;
        }
        socket->write(ba);
        break;
    }
    case showAdmins:
    {
        QByteArray ba;
        QDataStream msg(&ba, QIODevice::WriteOnly);
        msg.setVersion(QDataStream::Qt_5_5);
        msg<<showAdmins;
        for(QMap<QString, Admin>::iterator it = admins.begin();it!=admins.end();it++)
        {
            msg<<it.value().nickName<<it.value().surname<<it.value().name;
        }
        socket->write(ba);
        break;
    }
    case saveChanges://Спільно для адмінів і працівників. Сервер сам визначає кого саме змінюють і в залежності від того знає скільки параметрів було передано
    {
        QString person;
        reader>>person;
        QMap<QString, Worker>::iterator it = workers.find(person);
        if(it!=workers.end())
        {
            reader>>it.value().surname;
            reader>>it.value().name;
            reader>>it.value().position;
            reader>>it.value().salary;
            reader>>it.value().bonus;
        }
        else
        {
             QMap<QString, Admin>::iterator it = admins.find(person);
             if(it!=admins.end())
             {
                 reader>>it.value().surname;
                 reader>>it.value().name;
             }
        }
        break;
    }
    case saveMyChanges:
    {
        QString person;
        reader>>person;
        QMap<QString, Worker>::iterator it = workers.find(person);
        if(it!=workers.end())
        {
            reader>>it.value().nickName;
            reader>>it.value().password;
            reader>>it.value().surname;
            reader>>it.value().name;
            reader>>date;
        }
        else
        {
             QMap<QString, Admin>::iterator it = admins.find(person);
             if(it!=admins.end())
             {
                 reader>>it.value().nickName;
                 reader>>it.value().password;
                 reader>>it.value().surname;
                 reader>>it.value().name;
                 reader>>date;
             }
        }
        break;
    }
    }
}

