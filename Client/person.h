#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QPair>
#include <QDateTime>

struct AdminPerson
{
    QString nickName, password, surname, name;
};

struct WorkerPerson
{
    QString nickName, password, surname, name, ID, enterDate, position, salary, bonus;
    QVector<QPair<QString, QString> > history;
};

#endif // PERSON_H
