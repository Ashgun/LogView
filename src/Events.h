#pragma once

#include "EventPattern.h"
#include "LineMatcher.h"

#include <QString>

#include <memory>
#include <vector>

class IMatchableEventPattern
{
public:
    QString Name;

public:
    virtual bool IsPatternMatched(QString const& line) const = 0;
    virtual std::unique_ptr<IMatchableEventPattern> Clone() const = 0;
    virtual ~IMatchableEventPattern() = default;

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

public:
    EventPattern const StartPattern;
    EventPattern const EndPattern;
    EventPattern const AltEndPattern;
};

IMatchableEventPatternPtr CreateSingleEvent(QString const& name, EventPattern const& pattern);
IMatchableEventPatternPtr CreateExtendedEvent(QString const& name, EventPattern const& startPattern,
    EventPattern const& endPattern);
IMatchableEventPatternPtr CreateExtendedEvent(QString const& name, EventPattern const& startPattern,
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
    std::vector<IMatchableEventPatternPtr> m_events;
};

struct EventPatternsHierarchyNode
{
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
