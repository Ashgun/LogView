#ifndef LOGLINEINFO_H
#define LOGLINEINFO_H

#include <QString>
#include <QMap>
#include <QStringList>

struct LogLineInfo
{
    quint64 LineNumber;

    QMap<QString, QString> HeaderItems;

    QStringList Groups;

    QString Message;

    QMap<QString, QString> ValuesInMessage;
};

#endif // LOGLINEINFO_H
