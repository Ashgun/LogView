#pragma once

#include "EventPattern.h"
#include "LineMatcher.h"
#include "PositionedLine.h"

#include <QString>

#include <memory>
#include <vector>

enum EventType
{
    Single,
    Extended
};

class IMatchableEventPattern
{
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

using IMatchableEventPatternPtr = std::unique_ptr<IMatchableEventPattern>;

class SingleEventPattern : public IMatchableEventPattern
{
public:
    explicit SingleEventPattern(QString const& name, EventPattern const& pattern);
    SingleEventPattern(const QString& name, EventPattern::PatternType const& patternType,
        EventPattern::PatternString const& patternString);
    bool IsPatternMatched(const QString& line) const override;
    std::unique_ptr<IMatchableEventPattern> Clone() const override;

    EventType GetType() const override;

public:
    EventPattern const Pattern;
};

class ExtendedEventPattern : public IMatchableEventPattern
{
public:
    ExtendedEventPattern(QString const& name, EventPattern const& startPattern, EventPattern const& endPattern);
    ExtendedEventPattern(QString const& name, EventPattern const& startPattern, EventPattern const& endPattern,
        EventPattern const& altEndPattern);
    bool IsPatternMatched(const QString& line) const override;
    std::unique_ptr<IMatchableEventPattern> Clone() const override;

    EventType GetType() const override;

    bool IsStartPatternMatched(const QString& line) const;
    bool IsEndPatternMatched(const QString& line) const;

public:
    EventPattern const StartPattern;
    EventPattern const EndPattern;
    EventPattern const AltEndPattern;
};

IMatchableEventPatternPtr CreateSingleEventPattern(QString const& name, EventPattern const& pattern);
IMatchableEventPatternPtr CreateExtendedEventPattern(QString const& name, EventPattern const& startPattern,
    EventPattern const& endPattern);
IMatchableEventPatternPtr CreateExtendedEventPattern(QString const& name, EventPattern const& startPattern,
    EventPattern const& endPattern, EventPattern const& altEndPattern);

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
    PositionedLine StartLine;
    PositionedLine EndLine;
    std::vector<Event> NextLevelEvents;

    Event() = default;
};

Event CreateEventFromPattern(IMatchableEventPattern const& pattern);

class IPositionedLinesStorage;

std::vector<std::vector<Event>> FindEvents(EventPatternsHierarchy const& patterns, IPositionedLinesStorage const& lines);
