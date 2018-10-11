#include "Events.h"
#include "IPositionedLinesStorage.h"

SingleEventPattern::SingleEventPattern(const QString& name, const EventPattern::PatternType& patternType,
    const EventPattern::PatternString& patternString, const IMatchableEventPattern::Color& color) :
    SingleEventPattern(name, EventPattern({patternType, patternString}), color)
{
}

SingleEventPattern::SingleEventPattern(const QString& name, const EventPattern& pattern, const IMatchableEventPattern::Color& color) :
    Pattern(pattern),
    ViewColor(color)
{
    Name = name;
}

bool SingleEventPattern::IsPatternMatched(const QString& line) const
{
    return m_lineMatcher.IsPatternMatched(line, Pattern);
}

std::unique_ptr<IMatchableEventPattern> SingleEventPattern::Clone() const
{
    return std::make_unique<SingleEventPattern>(Name, Pattern, ViewColor);
}

EventType SingleEventPattern::GetType() const
{
    return EventType::Single;
}

ExtendedEventPattern::ExtendedEventPattern(QString const& name, const EventPattern& startPattern, const EventPattern& endPattern,
                                           const IMatchableEventPattern::Color& color) :
    ExtendedEventPattern(name, startPattern, endPattern, endPattern, color, color)
{
}

ExtendedEventPattern::ExtendedEventPattern(const QString& name, const EventPattern& startPattern, const EventPattern& endPattern,
    const EventPattern& altEndPattern, const IMatchableEventPattern::Color& successColor, const IMatchableEventPattern::Color& altColor) :
    StartPattern(startPattern),
    EndPattern(endPattern),
    AltEndPattern(altEndPattern),
    SuccessEndColor(successColor),
    AltEndColor(altColor)
{
    Name = name;
}

bool ExtendedEventPattern::IsPatternMatched(const QString& line) const
{
    return
        IsStartPatternMatched(line) ||
        IsEndOrAltPatternMatched(line);
}

std::unique_ptr<IMatchableEventPattern> ExtendedEventPattern::Clone() const
{
    return std::make_unique<ExtendedEventPattern>(Name, StartPattern, EndPattern, AltEndPattern, SuccessEndColor, AltEndColor);
}

EventType ExtendedEventPattern::GetType() const
{
    return EventType::Extended;
}

bool ExtendedEventPattern::IsStartPatternMatched(const QString& line) const
{
    return m_lineMatcher.IsPatternMatched(line, StartPattern);
}

bool ExtendedEventPattern::IsEndOrAltPatternMatched(const QString& line) const
{
    return
        m_lineMatcher.IsPatternMatched(line, EndPattern) ||
        m_lineMatcher.IsPatternMatched(line, AltEndPattern);
}

bool ExtendedEventPattern::IsEndPatternMatched(const QString& line) const
{
    return m_lineMatcher.IsPatternMatched(line, EndPattern);
}

bool ExtendedEventPattern::IsAltPatternMatched(const QString& line) const
{
    return m_lineMatcher.IsPatternMatched(line, AltEndPattern);
}

EventPatterns::EventPatterns()
{
}

void EventPatterns::push_back(const SingleEventPattern& event)
{
    m_eventPatterns.push_back(event.Clone());
}

void EventPatterns::push_back(const ExtendedEventPattern& event)
{
    m_eventPatterns.push_back(event.Clone());
}

std::size_t EventPatterns::size() const
{
    return m_eventPatterns.size();
}

const IMatchableEventPattern&EventPatterns::operator[](const std::size_t index) const
{
    return *m_eventPatterns[index];
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

IMatchableEventPatternPtr CreateSingleEventPattern(QString const& name, const EventPattern& pattern, const IMatchableEventPattern::Color& color)
{
    return std::make_unique<SingleEventPattern>(name, pattern, color);
}

IMatchableEventPatternPtr CreateExtendedEventPattern(QString const& name, const EventPattern& startPattern, const EventPattern& endPattern,
                                                     const IMatchableEventPattern::Color& color)
{
    return std::make_unique<ExtendedEventPattern>(name, startPattern, endPattern, color);
}

IMatchableEventPatternPtr CreateExtendedEventPattern(
        QString const& name, const EventPattern& startPattern, const EventPattern& endPattern,
        const EventPattern& altEndPattern,
        const IMatchableEventPattern::Color& successColor, const IMatchableEventPattern::Color& altColor)
{
    return std::make_unique<ExtendedEventPattern>(name, startPattern, endPattern, altEndPattern, successColor, altColor);
}

void EventPatternsHierarchy::AddEventPattern(IMatchableEventPatternPtr event)
{
    TopLevelNodes.push_back(EventPatternsHierarchyNode());
    TopLevelNodes.back().Event = std::move(event);
}

EventPatternsHierarchyNode::EventPatternsHierarchyNode(const EventPatternsHierarchyNode& other) :
    Event(other.Event != nullptr ? std::move(other.Event->Clone()) : 0),
    SubEvents(other.SubEvents)
{
}

void EventPatternsHierarchyNode::AddSubEventPattern(IMatchableEventPatternPtr event)
{
    SubEvents.push_back(EventPatternsHierarchyNode());
    SubEvents.back().Event = std::move(event);
}

namespace
{

struct LocatedEvent
{
    Event FoundEvent;
    std::size_t FoundEventStartLocation;
    std::size_t FoundEventEndLocation;

    LocatedEvent() = default;
};

void FindSingleEventInRange(SingleEventPattern const& pattern, IPositionedLinesStorage const& lines,
    const int level, std::size_t const firstInd, std::size_t const lastInd, std::vector<LocatedEvent>& events,
                            IEventGroupExtractor const& eventGroupExtractor)
{
    for (std::size_t i = firstInd; i < lastInd; ++i)
    {
        if (pattern.IsPatternMatched(lines[i].Line))
        {
            LocatedEvent event;
            event.FoundEvent = CreateEventFromPattern(pattern);
            event.FoundEvent.Level = level;
            event.FoundEvent.StartLine = event.FoundEvent.EndLine = lines[i];
            event.FoundEvent.StartLine.Position.Number = event.FoundEvent.EndLine.Position.Number = i;
            event.FoundEvent.Group = eventGroupExtractor.GetGroupFromLine(event.FoundEvent.StartLine);
            event.FoundEvent.ViewColor = pattern.ViewColor;
            event.FoundEventStartLocation = event.FoundEventEndLocation = i;

            events.push_back(event);
        }
    }
}

void FindExtendedEventInRange(ExtendedEventPattern const& pattern, IPositionedLinesStorage const& lines,
    const int level, std::size_t const firstInd, std::size_t const lastInd, std::vector<LocatedEvent>& events,
                              IEventGroupExtractor const& eventGroupExtractor)
{
    for (std::size_t i = firstInd; i < lastInd; ++i)
    {
        const auto& positionedLineToProc = lines[i];

        if (pattern.IsStartPatternMatched(positionedLineToProc.Line))
        {
            LocatedEvent event;
            event.FoundEvent = CreateEventFromPattern(pattern);
            event.FoundEvent.Level = level;
            event.FoundEvent.StartLine = positionedLineToProc;
            event.FoundEvent.StartLine.Position.Number = i;
            event.FoundEvent.Group = eventGroupExtractor.GetGroupFromLine(event.FoundEvent.StartLine);
            event.FoundEventStartLocation = i;

            events.push_back(event);

            continue;
        }

        if (pattern.IsEndOrAltPatternMatched(positionedLineToProc.Line))
        {
            const auto group = eventGroupExtractor.GetGroupFromLine(positionedLineToProc);
            for (std::vector<LocatedEvent>::reverse_iterator iter = events.rbegin(); iter != events.rend(); ++iter)
            {
                if (iter->FoundEvent.Group == group && iter->FoundEvent.Type == EventType::Extended)
                {
                    iter->FoundEvent.EndLine = positionedLineToProc;
                    iter->FoundEvent.EndLine.Position.Number = i;
                    iter->FoundEventEndLocation = i;

                    if (pattern.IsAltPatternMatched(positionedLineToProc.Line))
                    {
                        iter->FoundEvent.ViewColor = pattern.AltEndColor;
                    }
                    else
                    {
                        iter->FoundEvent.ViewColor = pattern.SuccessEndColor;
                    }

                    break;
                }
            }
        }
    }
}

std::vector<Event> LocatedEventsToEvents(std::vector<LocatedEvent> const& locatedEvents)
{
    std::vector<Event> result;
    result.reserve(locatedEvents.size());

    for (auto const& locatedEvent : locatedEvents)
    {
        result.push_back(locatedEvent.FoundEvent);
    }

    return result;
}

void FindEventsInRange(IMatchableEventPattern const& pattern, IPositionedLinesStorage const& lines,
    const int level, std::size_t const firstInd, std::size_t const lastInd, std::vector<LocatedEvent>& events,
                       IEventGroupExtractor const& eventGroupExtractor)
{
    if (pattern.GetType() == EventType::Single)
    {
        SingleEventPattern const& singleEventPattern = dynamic_cast<SingleEventPattern const&>(pattern);
        FindSingleEventInRange(singleEventPattern, lines, level, firstInd, lastInd, events, eventGroupExtractor);
    }
    else
    {
        ExtendedEventPattern const& extendedEventPattern = dynamic_cast<ExtendedEventPattern const&>(pattern);
        FindExtendedEventInRange(extendedEventPattern, lines, level, firstInd, lastInd, events, eventGroupExtractor);
    }
}

void GetPattertsOfSelectedLevel(
    std::vector<EventPatternsHierarchyNode> const& patterns, int const requiredLevel, int const currentLevel,
    std::vector<EventPatternsHierarchyNode>& result, IEventGroupExtractor const& eventGroupExtractor)
{
    if (requiredLevel == currentLevel)
    {
        for (std::size_t i = 0; i < patterns.size(); ++i)
        {
            result.push_back(patterns[i]);
            result.back().SubEvents.clear();
        }
    }
    else
    {
        for (std::size_t i = 0; i < patterns.size(); ++i)
        {
            GetPattertsOfSelectedLevel(patterns[i].SubEvents, requiredLevel, currentLevel + 1, result, eventGroupExtractor);
        }
    }
}

} // namespace

std::vector<std::vector<Event>> FindEvents(const EventPatternsHierarchy& patterns, const IPositionedLinesStorage& lines,
                                           IEventGroupExtractor const& eventGroupExtractor)
{
    if (lines.Size() == 0)
    {
        return std::vector<std::vector<Event>>();
    }

    std::vector<std::vector<Event>> result;
    for (int level = 0; level < 10; ++level)
    {
        std::vector<EventPatternsHierarchyNode> eventsOfSelectedLevel;
        GetPattertsOfSelectedLevel(patterns.TopLevelNodes, level, 0, eventsOfSelectedLevel, eventGroupExtractor);

        std::vector<LocatedEvent> locatedEvents;
        for (std::size_t i = 0; i < eventsOfSelectedLevel.size(); ++i)
        {
            FindEventsInRange(*eventsOfSelectedLevel[i].Event, lines, level, 0, lines.Size(), locatedEvents, eventGroupExtractor);
        }

        if (level > 0 && locatedEvents.empty())
        {
            break;
        }

        result.push_back(LocatedEventsToEvents(locatedEvents));
    }

    return result;
}

Event CreateEventFromPattern(const IMatchableEventPattern& pattern)
{
    Event event;
    event.Name = pattern.Name;
    event.Type = pattern.GetType();

    return event;
}

namespace
{

bool IsPositionedLineBetweenTwoOthers(PositionedLine const& checkedLine, PositionedLine const& rangeStart,
                                     PositionedLine const& rangeEnd)
{
    if (rangeEnd.Position.Offset < rangeStart.Position.Offset)
    {
        return IsPositionedLineBetweenTwoOthers(checkedLine, rangeEnd, rangeStart);
    }

    return
        checkedLine.Position.Offset >= rangeStart.Position.Offset &&
        checkedLine.Position.Offset <= rangeEnd.Position.Offset;
}

} // namespace

bool IsEventsOverlapped(const Event& l, const Event& r)
{
    return
        IsPositionedLineBetweenTwoOthers(l.StartLine, r.StartLine, r.EndLine) ||
        IsPositionedLineBetweenTwoOthers(l.EndLine, r.StartLine, r.EndLine) ||
        IsPositionedLineBetweenTwoOthers(r.StartLine, l.StartLine, l.EndLine) ||
        IsPositionedLineBetweenTwoOthers(r.EndLine, l.StartLine, l.EndLine);
}

int CheckEventsOrder(const Event& l, const Event& r)
{
    if (IsEventsOverlapped(l, r))
    {
        return 0;
    }

    if (l.EndLine.Position.Offset < r.StartLine.Position.Offset)
    {
        return -1;
    }

    return 1;
}

bool Event::operator==(const Event& other) const
{
    return
        Type == other.Type &&
        Name == other.Name &&
        Group == other.Group &&
        StartLine == other.StartLine &&
        EndLine == other.EndLine;
}

bool operator<(const Event& lhs, const Event& rhs)
{
    return CheckEventsOrder(lhs, rhs) < 0;
}

#include <QDebug>

EventHierarchyInfoForLines GetHierarchyInfoForLines(const IPositionedLinesStorage& positionedLinesStorage, const std::vector<std::vector<Event> >& eventLevels)
{
    EventHierarchyInfoForLines result;
    result.resize(positionedLinesStorage.Size());

    for (std::size_t eventLevelNum = 0; eventLevelNum < eventLevels.size(); ++eventLevelNum)
    {
        auto const& eventLevel = eventLevels[eventLevelNum];

        for (std::size_t eventNumInLevel = 0; eventNumInLevel < eventLevel.size(); ++eventNumInLevel)
        {
            Event const& event = eventLevel[eventNumInLevel];
            result[event.StartLine.Position.Number] = EventHierarchyInfo({ event.Group, eventLevelNum, eventNumInLevel});
            result[event.EndLine.Position.Number] = EventHierarchyInfo({ event.Group, eventLevelNum, eventNumInLevel });
        }
    }

    return result;
}

IMatchableEventPattern::Color CreateColor(const quint8 R, const quint8 G, const quint8 B)
{
    IMatchableEventPattern::Color color;

    color.R = R;
    color.G = G;
    color.B = B;

    return color;
}
