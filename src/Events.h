#pragma once

#include "EventPattern.h"
#include "LineMatcher.h"

#include <memory>
#include <vector>

class IMatchableEvent
{
public:
    virtual bool IsPatternMatched(QString const& line) const = 0;
    virtual std::unique_ptr<IMatchableEvent> Clone() const = 0;
    virtual ~IMatchableEvent() = default;

protected:
    LineMatcher m_lineMatcher;
};

using IMatchableEventPtr = std::unique_ptr<IMatchableEvent>;

class SingleEvent : public IMatchableEvent
{
public:
    explicit SingleEvent(EventPattern const& pattern);
    SingleEvent(EventPattern::PatternType const& patternType, EventPattern::PatternString const& patternString);
    bool IsPatternMatched(const QString& line) const override;
    std::unique_ptr<IMatchableEvent> Clone() const override;

public:
    EventPattern const Pattern;
};

class ExtendedEvent : public IMatchableEvent
{
public:
    ExtendedEvent(EventPattern const& startPattern, EventPattern const& endPattern);
    ExtendedEvent(EventPattern const& startPattern, EventPattern const& endPattern,
        EventPattern const& altEndPattern);
    bool IsPatternMatched(const QString& line) const override;
    std::unique_ptr<IMatchableEvent> Clone() const override;

public:
    EventPattern const StartPattern;
    EventPattern const EndPattern;
    EventPattern const AltEndPattern;
};

IMatchableEventPtr CreateSingleEvent(EventPattern const& pattern);
IMatchableEventPtr CreateExtendedEvent(EventPattern const& startPattern, EventPattern const& endPattern);
IMatchableEventPtr CreateExtendedEvent(EventPattern const& startPattern, EventPattern const& endPattern,
    EventPattern const& altEndPattern);

class Events
{
public:
    Events();

    void push_back(SingleEvent const& event);
    void push_back(ExtendedEvent const& event);
    std::size_t size() const;
    IMatchableEvent const& operator[](std::size_t const index) const;

private:
    std::vector<IMatchableEventPtr> m_events;
};

struct EventsHierarchyNode
{
    IMatchableEventPtr Event;
    std::vector<EventsHierarchyNode> SubEvents;

    void AddSubEvent(IMatchableEventPtr event);
};

struct EventsHierarchy
{
    std::vector<EventsHierarchyNode> TopLevelNodes;

    void AddEvent(IMatchableEventPtr event);
};

class EventsHierarchyMatcher
{
public:
    EventsHierarchy Events;

public:
    bool IsAnyEventMatched(QString const& line) const;
    int GetLevelInHierarchy(QString const& line) const;
};
