#pragma once

#include "EventPattern.h"
#include "LineMatcher.h"
#include "PositionedLine.h"

#include <QString>

#include <QColor>
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
        quint8 R;
        quint8 G;
        quint8 B;

        QColor toQColor() const;
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

class EventPatterns
{
public:
    EventPatterns();

    void push_back(SingleEventPattern const& event);
    void push_back(ExtendedEventPattern const& event);
    std::size_t size() const;
    IMatchableEventPattern const& operator[](std::size_t const index) const;

private:
    std::vector<IMatchableEventPatternPtr> m_eventPatterns;
};

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

    void AddEventPattern(IMatchableEventPatternPtr event);
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
    bool operator==(Event const& other) const;
};

Event CreateEventFromPattern(IMatchableEventPattern const& pattern);

class IEventGroupExtractor
{
public:
    virtual QString GetGroupFromLine(PositionedLine const& line) const = 0;
    virtual ~IEventGroupExtractor() = default;
};

class IPositionedLinesStorage;

std::vector<std::vector<Event>> FindEvents(EventPatternsHierarchy const& patterns, IPositionedLinesStorage const& lines,
                                           IEventGroupExtractor const& eventGroupExtractor);


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

using EventHierarchyInfoForLines = QVector<EventHierarchyInfo>;

EventHierarchyInfoForLines GetHierarchyInfoForLines(IPositionedLinesStorage const& positionedLinesStorage,
                                             std::vector<std::vector<Event>> const& eventLevels);
