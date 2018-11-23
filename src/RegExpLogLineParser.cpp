#include "RegExpLogLineParser.h"

RegExpLogLineParser::RegExpLogLineParser(QVector<QPair<QString, QRegExp>> const& headerRegExps, QRegExp const& groupRegExp) :
    m_headerRegExps(headerRegExps),
    m_groupRegExp(groupRegExp)
{
}

RegExpLogLineParser::RegExpLogLineParser(
        const QVector<LogLineHeaderParsingParams::GroupData>& headerRegExpsStrs, const QString& groupRegExpStr)
{
    for (LogLineHeaderParsingParams::GroupData const& rePair : headerRegExpsStrs)
    {
        m_headerRegExps.push_back(QPair<QString, QRegExp>(rePair.Name, QRegExp(rePair.RegExp + rePair.Delimiter)));
    }

    m_groupRegExp = QRegExp(groupRegExpStr);
}

LogLineInfo RegExpLogLineParser::Parse(const QString& line) const
{
    LogLineInfo resultInfo;

    int pos = 0;

    for (int reNum = 0; reNum < m_headerRegExps.size(); ++reNum)
    {
        pos = m_headerRegExps[reNum].second.indexIn(line, pos);

        if (pos < 0)
        {
            return LogLineInfo();
        }

        pos += m_headerRegExps[reNum].second.matchedLength();

        if (!m_headerRegExps[reNum].first.isEmpty())
        {
            resultInfo.HeaderItems[m_headerRegExps[reNum].first] = m_headerRegExps[reNum].second.cap(1);
        }
    }

    int nextPossibleCapStartPos = pos;
    int messagePos = pos;

    if (!m_groupRegExp.isEmpty())
    {
        while((pos = m_groupRegExp.indexIn(line, pos)) != -1)
        {
            pos += m_groupRegExp.matchedLength();

            if (nextPossibleCapStartPos + m_groupRegExp.matchedLength() == pos)
            {
                nextPossibleCapStartPos = pos;
                messagePos += m_groupRegExp.matchedLength();
                resultInfo.Groups << m_groupRegExp.cap(1);
            }
            else
            {
                break;
            }
        }
    }

    resultInfo.Message = line.mid(messagePos);

    return resultInfo;
}
