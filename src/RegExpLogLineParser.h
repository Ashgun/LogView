#ifndef REGEXPLOGLINEPARSER_H
#define REGEXPLOGLINEPARSER_H

#include "ILogLineParser.h"

#include <QVector>
#include <QRegExp>

class RegExpLogLineParser : public ILogLineParser
{
public:
    RegExpLogLineParser(
            QVector<QPair<QString, QRegExp>> const& headerRegExps, QRegExp const& groupRegExp = QRegExp());
    RegExpLogLineParser(QVector<QPair<QString, QString>> const& headerRegExpsStrs, QString const& groupRegExpStr = QString());
    ~RegExpLogLineParser() override = default;

    LogLineInfo Parse(const QString& line) const override;

private:
    QVector<QPair<QString, QRegExp>> m_headerRegExps;
    QRegExp m_groupRegExp;
};

#endif // REGEXPLOGLINEPARSER_H
