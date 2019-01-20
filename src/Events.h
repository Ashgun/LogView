#pragma once

#include "EventPattern.h"
#include "LineMatcher.h"
#include "PositionedLine.h"

#include <QString>

#include <QColor>
#include <QVector>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>

enum EventType
{
    Single,
    Extended
};

class IMatchableEventPattern
{
public:
    struct Color
    {
        quint8 R = 255;
        quint8 G = 255;
        quint8 B = 255;

        QColor toQColor() const;
        QString toColorCode() const;
        static Color fromColorCode(const QString& colorCode);
        static Color fromQColor(const QColor& color);
    };

public:
    QString Name;

public:
    virtual bool IsPatternMatched(QString const& line) const = 0;
    virtual std::unique_ptr<IMatchableEventPattern> Clone() const = 0;
    virtual ~IMatchableEventPattern() = default;

    virtual EventType GetType() const = 0;

protected:
    LineMatcher m_lineMatcher;

};

IMatchableEventPattern::Color CreateColor(const quint8 R, const quint8 G, const quint8 B);

using IMatchableEventPatternPtr = std::unique_ptr<IMatchableEventPattern>;

class SingleEventPattern : public IMatchableEventPattern
{
public:
    explicit SingleEventPattern(QString const& name, EventPattern const& pattern,
        const IMatchableEventPattern::Color& color);
    SingleEventPattern(const QString& name, EventPattern::PatternType const& patternType,
        EventPattern::PatternString const& patternString, const IMatchableEventPattern::Color& color);
    bool IsPatternMatched(const QString& line) const override;
    std::unique_ptr<IMatchableEventPattern> Clone() const override;

    EventType GetType() const override;

public:
    EventPattern const Pattern;
    Color ViewColor;
};

class ExtendedEventPattern : public IMatchableEventPattern
{
public:
    ExtendedEventPattern(QString const& name, EventPattern const& startPattern, EventPattern const& endPattern,
                         const IMatchableEventPattern::Color& color);
    ExtendedEventPattern(QString const& name, EventPattern const& startPattern, EventPattern const& endPattern,
        EventPattern const& altEndPattern,
        const IMatchableEventPattern::Color& successColor, const IMatchableEventPattern::Color& altColor);
    bool IsPatternMatched(const QString& line) const override;
    std::unique_ptr<IMatchableEventPattern> Clone() const override;

    EventType GetType() const override;

    bool IsStartPatternMatched(const QString& line) const;
    bool IsEndOrAltPatternMatched(const QString& line) const;

    bool IsEndPatternMatched(const QString& line) const;
    bool IsAltPatternMatched(const QString& line) const;

public:
    EventPattern const StartPattern;
    EventPattern const EndPattern;
    EventPattern const AltEndPattern;

    Color SuccessEndColor;
    Color AltEndColor;
};

IMatchableEventPatternPtr CreateSingleEventPattern(QString const& name, EventPattern const& pattern, const IMatchableEventPattern::Color& color);

IMatchableEventPatternPtr CreateExtendedEventPattern(QString const& name, EventPattern const& startPattern,
    EventPattern const& endPattern, const IMatchableEventPattern::Color& color);
IMatchableEventPatternPtr CreateExtendedEventPattern(QString const& name, EventPattern const& startPattern,
    EventPattern const& endPattern, EventPattern const& altEndPattern,
    const IMatchableEventPattern::Color& successColor, const IMatchableEventPattern::Color& altColor);

struct EventPatternsHierarchyNode
{
    EventPatternsHierarchyNode() = default;
    explicit EventPatternsHierarchyNode(EventPatternsHierarchyNode const& other);

    IMatchableEventPatternPtr Event;
    std::vector<EventPatternsHierarchyNode> SubEvents;

    void AddSubEventPattern(IMatchableEventPatternPtr event);
};

struct EventPatternsHierarchy
{
    std::vector<EventPatternsHierarchyNode> TopLevelNodes;
    std::vector<EventPatternsHierarchyNode> GlobalUnexpectedEventPatterns;
    std::vector<EventPatternsHierarchyNode> IgnoredEventPatterns;

    void AddEventPattern(IMatchableEventPatternPtr event);
    void AddGlobalUnexpectedEventPattern(IMatchableEventPatternPtr event);
    void AddIgnoredEventPattern(IMatchableEventPatternPtr event);

private:
    void AddEventToArray(IMatchableEventPatternPtr event, std::vector<EventPatternsHierarchyNode>& array);

public:
    static QString toJson(const EventPatternsHierarchy& patterns);
    static void fromJson(const QString& jsonData, EventPatternsHierarchy& pattern);
};

class EventPatternsHierarchyMatcher
{
public:
    EventPatternsHierarchy EventPatterns;

public:
    bool IsAnyEventMatched(QString const& line) const;
    int GetLevelInHierarchy(QString const& line) const;
};

struct Event
{
    EventType Type;
    QString Name;
    QString Group;
    int Level = -1;
    PositionedLine StartLine;
    PositionedLine EndLine;

    IMatchableEventPattern::Color ViewColor;

    Event() = default;
};

Event CreateEventFromPattern(IMatchableEventPattern const& pattern);

struct LogLineHeaderParsingParams
{
    using GroupNameType = QString;
    using GroupRegExpString = QString;
    using AfterGroupDelimiterType = QString;

    struct GroupData
    {
        GroupNameType Name;
        GroupRegExpString RegExp;
        AfterGroupDelimiterType Delimiter;
    };

    QVector<GroupData> HeaderGroupDatas;
    QString GroupNameForGrouping;
    QString SortingGroup;

    static LogLineHeaderParsingParams FromJson(const QString& jsonData);
    static QString ToJson(const LogLineHeaderParsingParams& params);
};

class IEventInfoExtractor
{
public:
    virtual QString GetGroupFromLine(PositionedLine const& line) const;
    virtual QString GetGroupFromLine(EventPattern::PatternString const& line) const = 0;

    virtual QString GetMessageFromLine(PositionedLine const& line) const;
    virtual QString GetMessageFromLine(EventPattern::PatternString const& line) const = 0;

    virtual ~IEventInfoExtractor() = default;
};

class IPositionedLinesStorage;

std::vector<std::vector<Event>> FindEvents(EventPatternsHierarchy const& patterns, IPositionedLinesStorage& lines,
                                           IEventInfoExtractor const& eventInfoExtractor);


bool EventContainsOther(Event const& container, Event const& contained);
bool IsEventsOverlapped(Event const& l, Event const& r);

// < 0 -- the first event is earlier than the second one
// = 0 -- the first event is overlapped with the second one
// > 0 -- the first event is later than the second one
int CheckEventsOrder(Event const& l, Event const& r);

bool operator<(const Event& lhs, const Event& rhs);

using GroupedEvents = std::set<Event>;

struct EventsHierarchyNode
{
    Event Node;
    GroupedEvents SameGroupChildrenEvents;
    std::map<QString, GroupedEvents> OtherGroupChildrenEvents;
};

struct EventHierarchyInfo
{
    using LineGroupName = QString;
    using EventLevelOfLine = std::size_t;

    LineGroupName Group;
    EventLevelOfLine Level;
    std::size_t EventIndexInHierarchyLevel;
};

using EventHierarchyInfoForLines = std::vector<EventHierarchyInfo>;

EventHierarchyInfoForLines GetHierarchyInfoForLines(IPositionedLinesStorage const& positionedLinesStorage,
                                             std::vector<std::vector<Event>> const& eventLevels);
