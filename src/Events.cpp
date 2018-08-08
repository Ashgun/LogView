#include "Events.h"

SingleEventPattern::SingleEventPattern(const EventPattern::PatternType& patternType, const EventPattern::PatternString& patternString) :
    SingleEventPattern(EventPattern({patternType, patternString}))
{
}

SingleEventPattern::SingleEventPattern(const EventPattern& pattern) :
    Pattern(pattern)
{
}

bool SingleEventPattern::IsPatternMatched(const QString& line) const
{
    return m_lineMatcher.IsPatternMatched(line, Pattern);
}

std::unique_ptr<IMatchableEventPattern> SingleEventPattern::Clone() const
{
    return std::make_unique<SingleEventPattern>(Pattern);
}

ExtendedEventPattern::ExtendedEventPattern(const EventPattern& startPattern, const EventPattern& endPattern) :
    ExtendedEventPattern(startPattern, endPattern, endPattern)
{
}

ExtendedEventPattern::ExtendedEventPattern(const EventPattern& startPattern, const EventPattern& endPattern,
    const EventPattern& altEndPattern) :
    StartPattern(startPattern),
    EndPattern(endPattern),
    AltEndPattern(altEndPattern)
{
}

bool ExtendedEventPattern::IsPatternMatched(const QString& line) const
{
    return
        m_lineMatcher.IsPatternMatched(line, StartPattern) ||
        m_lineMatcher.IsPatternMatched(line, EndPattern) ||
            m_lineMatcher.IsPatternMatched(line, AltEndPattern);
}

std::unique_ptr<IMatchableEventPattern> ExtendedEventPattern::Clone() const
{
    return std::make_unique<ExtendedEventPattern>(StartPattern, EndPattern, AltEndPattern);
}

EventPatterns::EventPatterns()
{
}

void EventPatterns::push_back(const SingleEventPattern& event)
{
    m_events.push_back(event.Clone());
}

void EventPatterns::push_back(const ExtendedEventPattern& event)
{
    m_events.push_back(event.Clone());
}

std::size_t EventPatterns::size() const
{
    return m_events.size();
}

const IMatchableEventPattern&EventPatterns::operator[](const std::size_t index) const
{
    return *m_events[index];
}

bool IsAnyEventMatched(const EventPatterns& events, const QString& line)
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

bool IsAnyEventMatchedImpl(const EventPatternsHierarchyNode& eventsNode, const QString& line)
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

int GetLevelInHierarchyImpl(const EventPatternsHierarchyNode& eventsNode, const QString& line, int currentLevel)
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

bool EventPatternsHierarchyMatcher::IsAnyEventMatched(const QString& line) const
{
    for (std::size_t i = 0; i < EventPatterns.TopLevelNodes.size(); ++i)
    {
        if (IsAnyEventMatchedImpl(EventPatterns.TopLevelNodes[i], line))
        {
            return true;
        }
    }

    return false;
}

int EventPatternsHierarchyMatcher::GetLevelInHierarchy(const QString& line) const
{
    for (std::size_t i = 0; i < EventPatterns.TopLevelNodes.size(); ++i)
    {
        int const level = GetLevelInHierarchyImpl(EventPatterns.TopLevelNodes[i], line, 0);
        if (level >= 0)
        {
            return level;
        }
    }

    return -1;
}

IMatchableEventPatternPtr CreateSingleEvent(const EventPattern& pattern)
{
    return std::make_unique<SingleEventPattern>(pattern);
}

IMatchableEventPatternPtr CreateExtendedEvent(const EventPattern& startPattern, const EventPattern& endPattern)
{
    return std::make_unique<ExtendedEventPattern>(startPattern, endPattern);
}

IMatchableEventPatternPtr CreateExtendedEvent(const EventPattern& startPattern, const EventPattern& endPattern,
                                       const EventPattern& altEndPattern)
{
    return std::make_unique<ExtendedEventPattern>(startPattern, endPattern, altEndPattern);
}

void EventPatternsHierarchy::AddEventPattern(IMatchableEventPatternPtr event)
{
    TopLevelNodes.push_back(EventPatternsHierarchyNode());
    TopLevelNodes.back().Event = std::move(event);
}

void EventPatternsHierarchyNode::AddSubEventPattern(IMatchableEventPatternPtr event)
{
    SubEvents.push_back(EventPatternsHierarchyNode());
    SubEvents.back().Event = std::move(event);
}
