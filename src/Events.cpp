#include "Events.h"

SingleEvent::SingleEvent(const EventPattern::PatternType& patternType, const EventPattern::PatternString& patternString) :
    SingleEvent(EventPattern({patternType, patternString}))
{
}

SingleEvent::SingleEvent(const EventPattern& pattern) :
    Pattern(pattern)
{
}

bool SingleEvent::IsPatternMatched(const QString& line) const
{
    return m_lineMatcher.IsPatternMatched(line, Pattern);
}

std::unique_ptr<IMatchableEvent> SingleEvent::Clone() const
{
    return std::make_unique<SingleEvent>(Pattern);
}

ExtendedEvent::ExtendedEvent(const EventPattern& startPattern, const EventPattern& endPattern) :
    ExtendedEvent(startPattern, endPattern, endPattern)
{
}

ExtendedEvent::ExtendedEvent(const EventPattern& startPattern, const EventPattern& endPattern,
    const EventPattern& altEndPattern) :
    StartPattern(startPattern),
    EndPattern(endPattern),
    AltEndPattern(altEndPattern)
{
}

bool ExtendedEvent::IsPatternMatched(const QString& line) const
{
    return
        m_lineMatcher.IsPatternMatched(line, StartPattern) ||
        m_lineMatcher.IsPatternMatched(line, EndPattern) ||
            m_lineMatcher.IsPatternMatched(line, AltEndPattern);
}

std::unique_ptr<IMatchableEvent> ExtendedEvent::Clone() const
{
    return std::make_unique<ExtendedEvent>(StartPattern, EndPattern, AltEndPattern);
}

Events::Events()
{
}

void Events::push_back(const SingleEvent& event)
{
    m_events.push_back(event.Clone());
}

void Events::push_back(const ExtendedEvent& event)
{
    m_events.push_back(event.Clone());
}

std::size_t Events::size() const
{
    return m_events.size();
}

const IMatchableEvent&Events::operator[](const std::size_t index) const
{
    return *m_events[index];
}

bool IsAnyEventMatched(const Events& events, const QString& line)
{
    for (std::size_t i = 0; i < events.size(); ++i)
    {
        if (events[i].IsPatternMatched(line))
        {
            return true;
        }
    }

    return false;
}

namespace
{

bool IsAnyEventMatchedImpl(const EventsHierarchyNode& eventsNode, const QString& line)
{
    if (eventsNode.Event->IsPatternMatched(line))
    {
        return true;
    }

    for (std::size_t i = 0; i < eventsNode.SubEvents.size(); ++i)
    {
        if (IsAnyEventMatchedImpl(eventsNode.SubEvents[i], line))
        {
            return true;
        }
    }

    return false;
}

int GetLevelInHierarchyImpl(const EventsHierarchyNode& eventsNode, const QString& line, int currentLevel)
{
    if (eventsNode.Event->IsPatternMatched(line))
    {
        return currentLevel;
    }

    for (std::size_t i = 0; i < eventsNode.SubEvents.size(); ++i)
    {
        int const level = GetLevelInHierarchyImpl(eventsNode.SubEvents[i], line, currentLevel + 1);
        if (level >= 0)
        {
            return level;
        }
    }

    return -1;
}

} // namespace

bool EventsHierarchyMatcher::IsAnyEventMatched(const QString& line) const
{
    for (std::size_t i = 0; i < Events.TopLevelNodes.size(); ++i)
    {
        if (IsAnyEventMatchedImpl(Events.TopLevelNodes[i], line))
        {
            return true;
        }
    }

    return false;
}

int EventsHierarchyMatcher::GetLevelInHierarchy(const QString& line) const
{
    for (std::size_t i = 0; i < Events.TopLevelNodes.size(); ++i)
    {
        int const level = GetLevelInHierarchyImpl(Events.TopLevelNodes[i], line, 0);
        if (level >= 0)
        {
            return level;
        }
    }

    return -1;
}

IMatchableEventPtr CreateSingleEvent(const EventPattern& pattern)
{
    return std::make_unique<SingleEvent>(pattern);
}

IMatchableEventPtr CreateExtendedEvent(const EventPattern& startPattern, const EventPattern& endPattern)
{
    return std::make_unique<ExtendedEvent>(startPattern, endPattern);
}

IMatchableEventPtr CreateExtendedEvent(const EventPattern& startPattern, const EventPattern& endPattern,
                                       const EventPattern& altEndPattern)
{
    return std::make_unique<ExtendedEvent>(startPattern, endPattern, altEndPattern);
}

void EventsHierarchy::AddEvent(IMatchableEventPtr event)
{
    TopLevelNodes.push_back(EventsHierarchyNode());
    TopLevelNodes.back().Event = std::move(event);
}

void EventsHierarchyNode::AddSubEvent(IMatchableEventPtr event)
{
    SubEvents.push_back(EventsHierarchyNode());
    SubEvents.back().Event = std::move(event);
}
