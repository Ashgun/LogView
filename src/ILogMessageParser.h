#ifndef ILOGMESSAGEPARSER_H
#define ILOGMESSAGEPARSER_H

#include <QMap>
#include <QVector>
#include <QString>

class ILogMessageParser
{
public:
    struct ValueParsingInfo
    {
        QString ValueName;
        QString ValueRegEx;
        quint16 GroupNumber;
    };

public:
    virtual ~ILogMessageParser() = default;

    virtual QMap<QString, QString> Parse(QString const& message, QVector<ValueParsingInfo> const& valueParsingInfo) const = 0;
};

#endif // ILOGMESSAGEPARSER_H
