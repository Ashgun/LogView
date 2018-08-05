#include "RegExpsLogMessageParser.h"

#include <QRegExp>

RegExpsLogMessageParser::RegExpsLogMessageParser()
{

}

QMap<QString, QString> RegExpsLogMessageParser::Parse(
        const QString& message,
        const QVector<ValueParsingInfo>& valueParsingInfo) const
{
    QMap<QString, QString> result;

    for (ValueParsingInfo const& valueRegExp : valueParsingInfo)
    {
        QRegExp varRegEx(valueRegExp.ValueRegEx);

        int pos = 0;

        pos = varRegEx.indexIn(message, pos);

        if (pos == -1)
        {
            result[valueRegExp.ValueName] = "";
            continue;
        }

        result[valueRegExp.ValueName] = varRegEx.cap(valueRegExp.GroupNumber);
    }

    return result;
}
