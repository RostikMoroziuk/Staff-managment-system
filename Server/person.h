#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QPair>
#include <QDateTime>

struct Admin
{
    QString nickName, password, surname, name;
};

struct Worker
{
    QString nickName, password, surname, name, ID, enterDate, position, salary, bonus;
    QVector<QPair<QString, QString> > history;
};

#endif // PERSON_H
