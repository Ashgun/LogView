#include <QCoreApplication>

#include <iostream>

#include <QString>
#include <QVector>

#include <QDebug>

namespace tests
{

struct LogStringInfo
{
public:
    typedef QPair<QString, QStringList> GroupWithSubgroups;

public:
    QString DateTime;
    QChar Level;
    long ThreadId;
    QVector<GroupWithSubgroups> Groups;
    QString Message;
};

void ParsingTest00()
{
    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal] Setting handler for signal [PIPE], native signal 0";

    QRegExp re("\\[([0-9_\\./\\-\\s:]+)\\] \\[([LIDWEF]){1,1}\\] \\[([0-9]+)\\]");
    QStringList list;
    int pos = 0;
    while((pos=re.indexIn(line, pos))!=-1){
        list << re.cap(1);
        list << re.cap(2);
        list << re.cap(3);
        pos += re.matchedLength();
        std::cout << pos << "|" << line.mid(pos).toStdString() <<  std::endl;
    }

    qDebug() << "LIST:" << list;
}

void ParsingTest01()
{
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal][Group] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal][Group1][Group2] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal::Group] Setting handler for signal [PIPE], native signal 0";
    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal::Group1::Group2][Group3] Setting handler for signal [PIPE], native signal 0";

    QRegExp reBaseData("\\[([0-9_\\./\\-\\s:]+)\\] \\[([TILDWEF]){1,1}\\] \\[([0-9]+)\\]");

    int pos = 0;
    pos = reBaseData.indexIn(line, pos);

    if (pos < 0)
    {
        return;
    }

    LogStringInfo info;

    info.DateTime = reBaseData.cap(1);
    info.Level = reBaseData.cap(2).at(0);
    QString const threadIdString = reBaseData.cap(3);
    info.ThreadId = threadIdString.toLong();

    pos += reBaseData.matchedLength() + 1;

    QRegExp reGroups("\\[([0-9A-Za-z:]+)\\]");

    QStringList possibleGroups;
    int nextPossibleCapStartPos = pos;

    while((pos = reGroups.indexIn(line, pos)) != -1)
    {
        pos += reGroups.matchedLength();

        if (nextPossibleCapStartPos + reGroups.matchedLength() == pos)
        {
            nextPossibleCapStartPos = pos;
            possibleGroups << reGroups.cap(1);

            info.Message = line.mid(pos + 1);
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < possibleGroups.size(); ++i)
    {
        QStringList groupData = possibleGroups.at(i).split(":", QString::SkipEmptyParts);

        if (!groupData.isEmpty())
        {
            info.Groups.push_back(LogStringInfo::GroupWithSubgroups());

            info.Groups.last().first = groupData.first();

            for (int j = 1; j < groupData.size(); ++j)
            {
                info.Groups.last().second << groupData.at(j);
            }
        }
    }

//    qDebug() << possibleGroups << info.Message;
    qDebug() << info.Groups;
}

void ParsingTest02()
{
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal][Group] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal][Group1][Group2] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal::Group] Setting handler for signal [PIPE], native signal 0";
    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal::Group1::Group2][Group3] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] Setting handler for signal [PIPE], native signal 0";

//    QRegExp reBaseData("(\\[([0-9_\\./\\-\\s:]+)\\])(\s)*(\\[([TILDWEF]){1,1}\\])(\s)*(\\[([0-9]+)\\])(\s)*(\\[([0-9A-Za-z:]+)\\])*");
//    QRegExp reBaseData("\\[([0-9_\\./\\-\\s:]+)\\]\s*");

    QVector<QRegExp> res;
    res.push_back(QRegExp("\\[([0-9_\\./\\-\\s:]+)\\]\\s*"));
    res.push_back(QRegExp("\\[([TILDWEF]){1,1}\\]\\s*"));
    res.push_back(QRegExp("\\[([0-9]+)\\]\\s*"));
    int pos = 0;

    for (int reNum = 0; reNum < res.size(); ++reNum)
    {
        pos = res[reNum].indexIn(line, pos);

        if (pos < 0)
        {
            qDebug() << "Exit" << reNum << pos;
            return;
        }

        pos += res[reNum].matchedLength();

        qDebug() << res[reNum].cap(1);
    }

    qDebug() << line.mid(pos);
    qDebug() << "";

    QRegExp reGroups("\\[([0-9A-Za-z:]+)\\]\\s*");

    int nextPossibleCapStartPos = pos;
    int messagePos = pos;

    while((pos = reGroups.indexIn(line, pos)) != -1)
    {
        pos += reGroups.matchedLength();

        if (nextPossibleCapStartPos + reGroups.matchedLength() == pos)
        {
            nextPossibleCapStartPos = pos;
            messagePos += reGroups.matchedLength();
            qDebug() << reGroups.cap(1);
        }
        else
        {
            break;
        }
    }

    qDebug() << line.mid(messagePos);
}

void ParsingTest03()
{
    QString line1 = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal] Setting handler for signal [PIPE], native signal 0";
    QString line2 = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal] Unsetting handler for signal [PIPE], native signal 0";

    qDebug() << line1.contains("Setting handler", Qt::CaseSensitivity::CaseSensitive);
    qDebug() << line2.contains("Setting handler", Qt::CaseSensitivity::CaseSensitive);

    qDebug() << line1.contains("Unsetting handler", Qt::CaseSensitivity::CaseSensitive);
    qDebug() << line2.contains("Unsetting handler", Qt::CaseSensitivity::CaseSensitive);
}

} // namespace tests

#include "RegExpLogLineParser.h"
#include "RegExpsLogMessageParser.h"

#include <memory>

void ILogLineParser_Test()
{
    QString groupRegExp("");
//    QString groupRegExp("\\[([0-9A-Za-z:]+)\\]\\s*");
    QVector<QPair<QString, QString>> headerRegExps;
    headerRegExps.push_back(QPair<QString, QString>("DateTime", "\\[([0-9_\\./\\-\\s:]+)\\]\\s*"));
    headerRegExps.push_back(QPair<QString, QString>("LogLevel", "\\[([TILDWEF]){1,1}\\]\\s*"));
    headerRegExps.push_back(QPair<QString, QString>("ThreadId", "\\[([x0-9]+)\\]\\s*"));

    std::unique_ptr<ILogLineParser> lineParser(new RegExpLogLineParser(headerRegExps, groupRegExp));

//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal][Group] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal][Group1][Group2] Setting handler for signal [PIPE], native signal 0";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] [Signal::Group] Setting handler for signal [PIPE], native signal 0";
    QString const line = "[2017-11-24 14:29:32.968704] [D] [0x21036] [Signal::Group1::Group2][Group3] Setting handler for signal [PIPE], native signal 1";
//    QString const line = "[2017-11-24 14:29:32.968704] [D] [21036] Setting handler for signal [PIPE], native signal 0";

    LogLineInfo info = lineParser->Parse(line);

    qDebug() << info.HeaderItems << info.Groups << info.Message;

    std::unique_ptr<ILogMessageParser> messageParser(new RegExpsLogMessageParser());
    QVector<ILogMessageParser::ValueParsingInfo> valuesRegExps;
    valuesRegExps.push_back(ILogMessageParser::ValueParsingInfo({ "SignalSource", "\\[([A-Za-z]+)\\]", 1 }));
    valuesRegExps.push_back(ILogMessageParser::ValueParsingInfo({ "SignalNumber", "(native signal\\s+)([0-9])+", 2 }));

    qDebug() << messageParser->Parse(info.Message, valuesRegExps);
}

#include "BaseLinePositionStorage.h"
#include "BasePositionedLinesStorage.h"
#include "Events.h"
#include "FilesIndexer.h"
#include "ILogLineParser.h"

#include <stdexcept>
#include <vector>

using IndexedLogLineInfo = std::pair<std::size_t, LogLineInfo>;
using SplittedIndexes = std::vector<IndexedLogLineInfo>;
using SplittedIndexesMap = std::map<QString, SplittedIndexes>;
SplittedIndexesMap SplitLinesByHeaderField(IPositionedLinesStorage const& source,
    QString const& headerField, ILogLineParser const& logLineParser)
{
    if (headerField.isEmpty())
    {
        throw std::invalid_argument("Header field for splitting should be not empty");
    }

    SplittedIndexesMap result;
    for (std::size_t i = 0; i < source.Size(); ++i)
    {
        LogLineInfo const info = logLineParser.Parse(source[i].Line);
        QString const orderingField = info.HeaderItems[headerField];
        result[orderingField].push_back(IndexedLogLineInfo(i, info));
    }

    return result;
}

class SplittedPositionedLinesStorage : public IPositionedLinesStorage
{
public:
    SplittedPositionedLinesStorage(IPositionedLinesStorage& decoratee, SplittedIndexes const& indexes) :
        m_decoratee(decoratee),
        m_indexes(indexes)
    {
    }

    void AddLine(const PositionedLine& positionedLine) override
    {
        m_decoratee.AddLine(positionedLine);
    }

    std::size_t Size() const override
    {
        return m_indexes.size();
    }

    const PositionedLine&operator [](const std::size_t index) const override
    {
        return m_decoratee[m_indexes[index].first];
    }

    PositionedLine&operator [](const std::size_t index) override
    {
        return m_decoratee[m_indexes[index].first];
    }

    std::unique_ptr<IPositionedLinesStorage> Clone() const override
    {
        return std::make_unique<SplittedPositionedLinesStorage>(m_decoratee, m_indexes);
    }

private:
    IPositionedLinesStorage& m_decoratee;
    SplittedIndexes const& m_indexes;
};

class ThreadIdEventGroupExtractor : public IEventGroupExtractor
{
public:
    ThreadIdEventGroupExtractor()
    {
        QString groupRegExp("");
        QVector<QPair<QString, QString>> headerRegExps;
        headerRegExps.push_back(QPair<QString, QString>("DateTime", "\\[([0-9_\\./\\-\\s:]+)\\]\\s*"));
        headerRegExps.push_back(QPair<QString, QString>("LogLevel", "\\[([TILDWEF]){1,1}\\]\\s*"));
        headerRegExps.push_back(QPair<QString, QString>("ThreadId", "\\[([x0-9]+)\\]\\s*"));

        m_lineParser.reset(new RegExpLogLineParser(headerRegExps, groupRegExp));
    }

    QString GetGroupFromLine(const PositionedLine& line) const
    {
        LogLineInfo info = m_lineParser->Parse(line.Line);
        return info.HeaderItems["ThreadId"];
    }

private:
    std::unique_ptr<ILogLineParser> m_lineParser;
};

void FilesIndexer_Test2()
{
    BaseLinePositionStorage linePositionStorage;
    BasePositionedLinesStorage positionedLinesStorage;

    EventPatternsHierarchyMatcher lineSelector;
    lineSelector.EventPatterns.AddEventPattern(
        CreateExtendedEventPattern("Service works",
            EventPattern::CreateStringPattern("Logging started"),
            EventPattern::CreateStringPattern("Logging finished")));
    lineSelector.EventPatterns.TopLevelNodes.back().AddSubEventPattern(
        CreateSingleEventPattern("Accounts list obtained",
            EventPattern::CreateStringPattern("[AccountRegistry] New accounts list obtained")));
    lineSelector.EventPatterns.TopLevelNodes.back().AddSubEventPattern(
                CreateExtendedEventPattern("Tenant backup",
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session started"),
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session completed successfully"),
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session completed with errors")));
    lineSelector.EventPatterns.TopLevelNodes.back().SubEvents.back().AddSubEventPattern(
                CreateExtendedEventPattern("Mailbox backup",
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was started"),
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was finished"),
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was failed")));

    FilesIndexer indexer(linePositionStorage, positionedLinesStorage, lineSelector);
    indexer.AddFileIndexes("log1.log");

    qDebug() << positionedLinesStorage.Size();

    ThreadIdEventGroupExtractor const threadIdEventGroupExtractor;
    std::vector<std::vector<Event>> eventLevels = FindEvents(lineSelector.EventPatterns, positionedLinesStorage,
                                                             threadIdEventGroupExtractor);

    GroupedSubEventsLinksHierarchy linksHierarchy = LinkEventsToHierarchy(eventLevels);

    qDebug() << eventLevels.size();
}

void FilesIndexer_Test()
{
    BaseLinePositionStorage linePositionStorage;
    BasePositionedLinesStorage positionedLinesStorage;

    EventPatternsHierarchyMatcher lineSelector;
    lineSelector.EventPatterns.AddEventPattern(
        CreateExtendedEventPattern("Service works",
            EventPattern::CreateStringPattern("Logging started"),
            EventPattern::CreateStringPattern("Logging finished")));
    lineSelector.EventPatterns.TopLevelNodes.back().AddSubEventPattern(
        CreateSingleEventPattern("Accounts list obtained",
            EventPattern::CreateStringPattern("[AccountRegistry] New accounts list obtained")));
    lineSelector.EventPatterns.TopLevelNodes.back().AddSubEventPattern(
                CreateExtendedEventPattern("Tenant backup",
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session started"),
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session completed successfully"),
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session completed with errors")));
    lineSelector.EventPatterns.TopLevelNodes.back().SubEvents.back().AddSubEventPattern(
                CreateExtendedEventPattern("Mailbox backup",
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was started"),
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was finished"),
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was failed")));

    FilesIndexer indexer(linePositionStorage, positionedLinesStorage, lineSelector);
    indexer.AddFileIndexes("log1.log");

    LinePosition pos;
    pos = linePositionStorage[0];
    pos = linePositionStorage[1];
    pos = linePositionStorage[2];

    qDebug() << linePositionStorage.Size();
    for (int i = 0; i < 3; ++i)
    {
        LinePosition pos = linePositionStorage[i];

        qDebug() << i << pos.Offset;
    }

    qDebug() << positionedLinesStorage.Size();
//    for (std::size_t i = 0; i < positionedLinesStorage.Size(); ++i)
//    {
//        PositionedLine const& line = positionedLinesStorage[i];

//        qDebug() << i << line.Position.Offset << line.LevelInHierarchy << line.Line;
//    }

    QString groupRegExp("");
    QVector<QPair<QString, QString>> headerRegExps;
    headerRegExps.push_back(QPair<QString, QString>("DateTime", "\\[([0-9_\\./\\-\\s:]+)\\]\\s*"));
    headerRegExps.push_back(QPair<QString, QString>("LogLevel", "\\[([TILDWEF]){1,1}\\]\\s*"));
    headerRegExps.push_back(QPair<QString, QString>("ThreadId", "\\[([x0-9]+)\\]\\s*"));

    std::unique_ptr<ILogLineParser> lineParser(new RegExpLogLineParser(headerRegExps, groupRegExp));
    SplittedIndexesMap const splittedIndexesMap = SplitLinesByHeaderField(positionedLinesStorage, "ThreadId", *lineParser);

    std::vector<std::vector<Event>> allEvents;
    allEvents.reserve(10);
    ThreadIdEventGroupExtractor const threadIdEventGroupExtractor;
    for (auto const& data : splittedIndexesMap)
    {
//        qDebug() << data.first;

        SplittedPositionedLinesStorage splittedPositionedLinesStorage(positionedLinesStorage, data.second);
        std::vector<std::vector<Event>> eventLevels = FindEvents(lineSelector.EventPatterns, splittedPositionedLinesStorage,
                                                                 threadIdEventGroupExtractor);

        if (allEvents.size() < eventLevels.size())
        {
            allEvents.resize(eventLevels.size());
        }

        for (std::size_t level = 0; level < eventLevels.size(); ++level)
        {
            std::copy(eventLevels[level].begin(), eventLevels[level].end(), std::back_inserter(allEvents[level]));
        }

//        qDebug() << eventLevels.size();
//        for (auto const& eventLevel : eventLevels)
//        {
//            qDebug() << "-" << eventLevel.size();

////            for (auto const& event : eventLevel)
////            {
////                qDebug() << event.Name << event.StartLine.Line;
////            }
//        }
    }

    qDebug() << allEvents.size();
    for (auto const& eventLevel : allEvents)
    {
        qDebug() << "-" << eventLevel.size();

//            for (auto const& event : eventLevel)
//            {
//                qDebug() << event.Name << event.StartLine.Line;
//            }
    }

    EventsHierarchy eventsHierarchy;
    AddEventsToEventsHierarchy(allEvents, eventsHierarchy);


}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Start" << std::endl;

//    tests::ParsingTest00();
//    tests::ParsingTest01();
//    tests::ParsingTest02();
//    tests::ParsingTest03();

//    ILogLineParser_Test();
//    FilesIndexer_Test();
    FilesIndexer_Test2();

    std::cout << "Finish" << std::endl;

    return 0;//a.exec();
}
