#ifndef REGEXPSLOGMESSAGEPARSER_H
#define REGEXPSLOGMESSAGEPARSER_H

#include "ILogMessageParser.h"

class RegExpsLogMessageParser : public ILogMessageParser
{
public:
    RegExpsLogMessageParser();
    ~RegExpsLogMessageParser() override = default;

    // ILogMessageParser interface
public:
    QMap<QString, QString> Parse(const QString& message, const QVector<ValueParsingInfo>& valueParsingInfo) const override;
};

#endif // REGEXPSLOGMESSAGEPARSER_H
