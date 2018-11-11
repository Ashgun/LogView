#include "Events.h"
#include "IPositionedLinesStorage.h"

#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>

#include <memory>
#include <stdexcept>
#include <sstream>

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

namespace
{

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

namespace
{

Json::Value PatternToJson(const EventPattern& pattern)
{
    Json::Value value;

    value["Data"] = pattern.Pattern.toStdString();
    value["Type"] = pattern.Type == EventPattern::PatternType::String ? "String" : "RegExp";

    return value;
}

Json::Value SingleEventToJson(const IMatchableEventPattern* basePattern)
{
    Json::Value value;

    const SingleEventPattern* pattern = dynamic_cast<const SingleEventPattern*>(basePattern);

    value["PatternName"] = pattern->Name.toStdString();
    value["PatternType"] = "Single";
    value["Color"] = pattern->ViewColor.toColorCode().toStdString();
    value["Pattern"] = PatternToJson(pattern->Pattern);

    return value;
}

Json::Value ExtendEventToJson(const IMatchableEventPattern* basePattern)
{
    Json::Value value;

    const ExtendedEventPattern* pattern = dynamic_cast<const ExtendedEventPattern*>(basePattern);

    value["PatternName"] = pattern->Name.toStdString();
    value["PatternType"] = "Extend";
    value["SuccessColor"] = pattern->SuccessEndColor.toColorCode().toStdString();
    value["AltColor"] = pattern->AltEndColor.toColorCode().toStdString();

    value["StartPattern"] = PatternToJson(pattern->StartPattern);
    value["EndPattern"] = PatternToJson(pattern->EndPattern);
    value["AltEnd"] = PatternToJson(pattern->AltEndPattern);

    return value;
}

Json::Value EventToJson(const IMatchableEventPattern* basePattern)
{
    return basePattern->GetType() == EventType::Single ?
        SingleEventToJson(basePattern) : ExtendEventToJson(basePattern);
}

Json::Value EventPatternsHierarchyNodeToJson(const EventPatternsHierarchyNode& node)
{
    Json::Value root = EventToJson(node.Event.get());

    Json::Value subEvents(Json::ValueType::arrayValue);
    for (const auto& subEvent : node.SubEvents)
    {
        subEvents.append(EventPatternsHierarchyNodeToJson(subEvent));
    }
    root["SubEvents"] = subEvents;

    return root;
}

} // namespace

QString EventPatternsHierarchy::toJson(const EventPatternsHierarchy& patterns)
{
    Json::Value root;

    Json::Value groupInfoArray(Json::ValueType::arrayValue);
    for (const auto& node : patterns.TopLevelNodes)
    {
        Json::Value value = EventToJson(node.Event.get());

        Json::Value subEvents(Json::ValueType::arrayValue);
        for (const auto& subEvent : node.SubEvents)
        {
            subEvents.append(EventPatternsHierarchyNodeToJson(subEvent));
        }
        value["SubEvents"] = subEvents;

        groupInfoArray.append(value);
    }
    root["EventPatternsHierarchyNodes"] = groupInfoArray;

    std::unique_ptr<Json::StreamWriter> writer(Json::StreamWriterBuilder().newStreamWriter());
    std::stringstream ss;
    writer->write(root, &ss);
    return QString::fromStdString(ss.str());
}

namespace
{

EventPattern CreateEventPattern(const QString& data, const QString& type)
{
    if (type.toUpper() == "STRING")
    {
        return EventPattern::CreateStringPattern(data);
    }
    else
    {
        return EventPattern::CreateRegExpPattern(data);
    }
}

void CheckJsonValue(const Json::Value& parent, const std::string& valueKey, const std::string& dataType)
{
    if (parent[valueKey].isNull() || parent[valueKey].empty())
    {
        throw std::invalid_argument(std::string("Invalid JSON data in EventPatternsHierarchy") + dataType + ": " + valueKey);
    }
}

EventPattern ParseLinePattern(const Json::Value& pattern)
{
    CheckJsonValue(pattern, "Type", "EventPatternsHierarchy");
    CheckJsonValue(pattern, "Data", "EventPatternsHierarchy");

    const QString type = QString::fromStdString(pattern["Type"].asString());
    const QString data = QString::fromStdString(pattern["Data"].asString());

    return CreateEventPattern(data, type);
}

void ParsePatternHierarchyNode(EventPatternsHierarchyNode& node, const Json::Value& patternHierarchyNode)
{
    CheckJsonValue(patternHierarchyNode, "PatternName", "EventPatternsHierarchy");
    CheckJsonValue(patternHierarchyNode, "PatternType", "EventPatternsHierarchy");

    const QString patternName = QString::fromStdString(patternHierarchyNode["PatternName"].asString());
    const QString patternTypeString = QString::fromStdString(patternHierarchyNode["PatternType"].asString());
    const EventType patternType = patternTypeString.toUpper() == "SINGLE" ? EventType::Single : EventType::Extended;

    if (patternType == EventType::Single)
    {
        CheckJsonValue(patternHierarchyNode, "Color", "EventPatternsHierarchy");

        const IMatchableEventPattern::Color color =
                IMatchableEventPattern::Color::fromColorCode(QString::fromStdString(patternHierarchyNode["Color"].asString()));

        CheckJsonValue(patternHierarchyNode, "Pattern", "EventPatternsHierarchy");
        node.AddSubEventPattern(
                    CreateSingleEventPattern(
                        patternName,
                        ParseLinePattern(patternHierarchyNode["Pattern"]),
                        color));
    }
    else
    {
        CheckJsonValue(patternHierarchyNode, "SuccessColor", "EventPatternsHierarchy");
        CheckJsonValue(patternHierarchyNode, "AltColor", "EventPatternsHierarchy");

        const IMatchableEventPattern::Color successColor =
                IMatchableEventPattern::Color::fromColorCode(QString::fromStdString(patternHierarchyNode["SuccessColor"].asString()));

        CheckJsonValue(patternHierarchyNode, "StartPattern", "EventPatternsHierarchy");
        CheckJsonValue(patternHierarchyNode, "EndPattern", "EventPatternsHierarchy");

        if (!patternHierarchyNode["AltEnd"].isNull() && !patternHierarchyNode["AltEnd"].empty())
        {
            const IMatchableEventPattern::Color altColor =
                    IMatchableEventPattern::Color::fromColorCode(QString::fromStdString(patternHierarchyNode["AltColor"].asString()));

            node.AddSubEventPattern(
                        CreateExtendedEventPattern(
                            patternName,
                            ParseLinePattern(patternHierarchyNode["StartPattern"]),
                            ParseLinePattern(patternHierarchyNode["EndPattern"]),
                            ParseLinePattern(patternHierarchyNode["AltEnd"]),
                            successColor, altColor)
                        );
        }
        else
        {
            node.AddSubEventPattern(
                        CreateExtendedEventPattern(
                            patternName,
                            ParseLinePattern(patternHierarchyNode["StartPattern"]),
                            ParseLinePattern(patternHierarchyNode["EndPattern"]),
                            successColor)
                        );
        }
    }

    if (!patternHierarchyNode["SubEvents"].isNull() &&
        !patternHierarchyNode["SubEvents"].empty() &&
        patternHierarchyNode["SubEvents"].isArray())
    {
        for (Json::ArrayIndex i = 0; i < patternHierarchyNode["SubEvents"].size(); ++i)
        {
            const Json::Value subEvent = patternHierarchyNode["SubEvents"][i];
            ParsePatternHierarchyNode(node.SubEvents.back(), subEvent);
        }
    }
}

void ParsePatternHierarchyNode(EventPatternsHierarchy& nodes, const Json::Value& patternHierarchyNode)
{
    CheckJsonValue(patternHierarchyNode, "PatternName", "EventPatternsHierarchy");
    CheckJsonValue(patternHierarchyNode, "PatternType", "EventPatternsHierarchy");

    const QString patternName = QString::fromStdString(patternHierarchyNode["PatternName"].asString());
    const QString patternTypeString = QString::fromStdString(patternHierarchyNode["PatternType"].asString());
    const EventType patternType = patternTypeString.toUpper() == "SINGLE" ? EventType::Single : EventType::Extended;

    if (patternType == EventType::Single)
    {
        CheckJsonValue(patternHierarchyNode, "Color", "EventPatternsHierarchy");

        const IMatchableEventPattern::Color color =
                IMatchableEventPattern::Color::fromColorCode(QString::fromStdString(patternHierarchyNode["Color"].asString()));

        CheckJsonValue(patternHierarchyNode, "Pattern", "EventPatternsHierarchy");
        nodes.AddEventPattern(
                    CreateSingleEventPattern(
                        patternName,
                        ParseLinePattern(patternHierarchyNode["Pattern"]),
                        color));
    }
    else
    {
        CheckJsonValue(patternHierarchyNode, "SuccessColor", "EventPatternsHierarchy");
        CheckJsonValue(patternHierarchyNode, "AltColor", "EventPatternsHierarchy");

        const IMatchableEventPattern::Color successColor =
                IMatchableEventPattern::Color::fromColorCode(QString::fromStdString(patternHierarchyNode["SuccessColor"].asString()));

        CheckJsonValue(patternHierarchyNode, "StartPattern", "EventPatternsHierarchy");
        CheckJsonValue(patternHierarchyNode, "EndPattern", "EventPatternsHierarchy");

        if (!patternHierarchyNode["AltEnd"].isNull() && !patternHierarchyNode["AltEnd"].empty())
        {
            const IMatchableEventPattern::Color altColor =
                    IMatchableEventPattern::Color::fromColorCode(QString::fromStdString(patternHierarchyNode["AltColor"].asString()));

            nodes.AddEventPattern(
                        CreateExtendedEventPattern(
                            patternName,
                            ParseLinePattern(patternHierarchyNode["StartPattern"]),
                            ParseLinePattern(patternHierarchyNode["EndPattern"]),
                            ParseLinePattern(patternHierarchyNode["AltEnd"]),
                            successColor, altColor)
                        );
        }
        else
        {
            nodes.AddEventPattern(
                        CreateExtendedEventPattern(
                            patternName,
                            ParseLinePattern(patternHierarchyNode["StartPattern"]),
                            ParseLinePattern(patternHierarchyNode["EndPattern"]),
                            successColor)
                        );
        }
    }

    if (!patternHierarchyNode["SubEvents"].isNull() &&
        !patternHierarchyNode["SubEvents"].empty() &&
        patternHierarchyNode["SubEvents"].isArray())
    {
        for (Json::ArrayIndex i = 0; i < patternHierarchyNode["SubEvents"].size(); ++i)
        {
            const Json::Value subEvent = patternHierarchyNode["SubEvents"][i];
            ParsePatternHierarchyNode(nodes.TopLevelNodes.back(), subEvent);
        }
    }
}

} // namespace

void EventPatternsHierarchy::fromJson(const QString& jsonData, EventPatternsHierarchy& patterns)
{
    std::unique_ptr<Json::CharReader> reader(Json::CharReaderBuilder().newCharReader());

    Json::Value root;

    const std::string data = jsonData.toStdString();
    std::string errs;
    if (!reader->parse(data.data(), data.data() + data.length(), &root, &errs))
    {
        throw std::invalid_argument(std::string("Invalid JSON data in ") + __FUNCTION__ + ": " + errs);
    }

    if (root["EventPatternsHierarchyNodes"].isNull() ||
        root["EventPatternsHierarchyNodes"].empty() ||
        !root["EventPatternsHierarchyNodes"].isArray())
    {
        throw std::invalid_argument("Invalid JSON data in EventPatternsHierarchy: EventPatternsHierarchyNodes");
    }

    patterns.TopLevelNodes.reserve(static_cast<std::size_t>(root["EventPatternsHierarchyNodes"].size()));
    for (Json::ArrayIndex i = 0; i < root["EventPatternsHierarchyNodes"].size(); ++i)
    {
        const Json::Value patternHierarchyNode = root["EventPatternsHierarchyNodes"][i];
        ParsePatternHierarchyNode(patterns, patternHierarchyNode);
    }
}

EventPatternsHierarchyNode::EventPatternsHierarchyNode(const EventPatternsHierarchyNode& other) :
    Event(other.Event != nullptr ? other.Event->Clone() : nullptr),
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

void FindSingleEventInRange(SingleEventPattern const& pattern, IPositionedLinesStorage& lines,
    const int level, std::size_t const firstInd, std::size_t const lastInd, std::vector<LocatedEvent>& events,
                            IEventInfoExtractor const& eventInfoExtractor)
{
    for (std::size_t i = firstInd; i < lastInd; ++i)
    {
        if (pattern.IsPatternMatched(lines[i].Line) && !lines[i].Position.Matched)
        {
            LocatedEvent event;
            event.FoundEvent = CreateEventFromPattern(pattern);
            event.FoundEvent.Level = level;
            event.FoundEvent.StartLine = event.FoundEvent.EndLine = lines[i];
            event.FoundEvent.StartLine.Position.NumberInMatchedLines = event.FoundEvent.EndLine.Position.NumberInMatchedLines = i;
            event.FoundEvent.Group = eventInfoExtractor.GetGroupFromLine(event.FoundEvent.StartLine);
            event.FoundEvent.ViewColor = pattern.ViewColor;
            event.FoundEventStartLocation = event.FoundEventEndLocation = i;

            lines[i].Position.Matched = true;

            events.push_back(event);
        }
    }
}

void FindExtendedEventInRange(ExtendedEventPattern const& pattern, IPositionedLinesStorage& lines,
    const int level, std::size_t const firstInd, std::size_t const lastInd, std::vector<LocatedEvent>& events,
    IEventInfoExtractor const& eventInfoExtractor)
{
    const auto UpdateIncorrectEventIfRequired =
            [&events, level, &lines, &eventInfoExtractor](const std::size_t index)
    {
        if (!events.empty())
        {
            LocatedEvent& event = events.back();

            if (event.FoundEvent.EndLine.LevelInHierarchy == -1 &&
                event.FoundEvent.Level == level)
            {
                event.FoundEvent.EndLine = lines[index - 1];
                event.FoundEvent.EndLine.Position.NumberInMatchedLines = index - 1;

                const QString messageToChange = eventInfoExtractor.GetMessageFromLine(event.FoundEvent.EndLine.Line);
                const int messagePos = event.FoundEvent.EndLine.Line.indexOf(messageToChange);
                event.FoundEvent.EndLine.Line = event.FoundEvent.EndLine.Line.mid(0, messagePos) +
                                                "<<< Event end and alternative end were not found >>>";
            }
        }
    };

    for (std::size_t i = firstInd; i < lastInd; ++i)
    {
        const auto& positionedLineToProc = lines[i];

        if (pattern.IsStartPatternMatched(positionedLineToProc.Line) && !positionedLineToProc.Position.Matched)
        {
            UpdateIncorrectEventIfRequired(i);

            LocatedEvent event;
            event.FoundEvent = CreateEventFromPattern(pattern);
            event.FoundEvent.Level = level;
            event.FoundEvent.StartLine = positionedLineToProc;
            event.FoundEvent.StartLine.Position.NumberInMatchedLines = i;
//            event.FoundEvent.StartLine = event.FoundEvent.EndLine = positionedLineToProc;
//            event.FoundEvent.StartLine.Position.NumberInMatchedLines = event.FoundEvent.EndLine.Position.NumberInMatchedLines = i;
            event.FoundEvent.Group = eventInfoExtractor.GetGroupFromLine(event.FoundEvent.StartLine);
            event.FoundEventStartLocation = i;

            events.push_back(event);

            lines[i].Position.Matched = true;

            continue;
        }

        if (pattern.IsEndOrAltPatternMatched(positionedLineToProc.Line) && !positionedLineToProc.Position.Matched)
        {
            const auto group = eventInfoExtractor.GetGroupFromLine(positionedLineToProc);
            for (std::vector<LocatedEvent>::reverse_iterator iter = events.rbegin(); iter != events.rend(); ++iter)
            {
                if (iter->FoundEvent.Group == group && iter->FoundEvent.Type == EventType::Extended)
                {
                    iter->FoundEvent.EndLine = positionedLineToProc;
                    iter->FoundEvent.EndLine.Position.NumberInMatchedLines = i;
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

            lines[i].Position.Matched = true;
        }
    }

    UpdateIncorrectEventIfRequired(lastInd);
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

void FindEventsInRange(IMatchableEventPattern const& pattern, IPositionedLinesStorage& lines,
    const int level, std::size_t const firstInd, std::size_t const lastInd, std::vector<LocatedEvent>& events,
                       IEventInfoExtractor const& eventInfoExtractor)
{
    if (pattern.GetType() == EventType::Single)
    {
        SingleEventPattern const& singleEventPattern = dynamic_cast<SingleEventPattern const&>(pattern);
        FindSingleEventInRange(singleEventPattern, lines, level, firstInd, lastInd, events, eventInfoExtractor);
    }
    else
    {
        ExtendedEventPattern const& extendedEventPattern = dynamic_cast<ExtendedEventPattern const&>(pattern);
        FindExtendedEventInRange(extendedEventPattern, lines, level, firstInd, lastInd, events, eventInfoExtractor);
    }
}

void GetPattertsOfSelectedLevel(
    std::vector<EventPatternsHierarchyNode> const& patterns, int const requiredLevel, int const currentLevel,
    std::vector<EventPatternsHierarchyNode>& result, IEventInfoExtractor const& eventInfoExtractor)
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
            GetPattertsOfSelectedLevel(patterns[i].SubEvents, requiredLevel, currentLevel + 1, result, eventInfoExtractor);
        }
    }
}

} // namespace

std::vector<std::vector<Event>> FindEvents(const EventPatternsHierarchy& patterns, IPositionedLinesStorage& lines,
                                           IEventInfoExtractor const& eventInfoExtractor)
{
    if (lines.Size() == 0)
    {
        return std::vector<std::vector<Event>>();
    }

    std::vector<std::vector<Event>> result;
    for (int level = 0; level < 10; ++level)
    {
        std::vector<EventPatternsHierarchyNode> eventsOfSelectedLevel;
        GetPattertsOfSelectedLevel(patterns.TopLevelNodes, level, 0, eventsOfSelectedLevel, eventInfoExtractor);

        std::vector<LocatedEvent> locatedEvents;
        for (std::size_t i = 0; i < eventsOfSelectedLevel.size(); ++i)
        {
            FindEventsInRange(*eventsOfSelectedLevel[i].Event, lines, level, 0, lines.Size(), locatedEvents, eventInfoExtractor);
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
            result[event.StartLine.Position.NumberInMatchedLines] = EventHierarchyInfo({ event.Group, eventLevelNum, eventNumInLevel});
            result[event.EndLine.Position.NumberInMatchedLines] = EventHierarchyInfo({ event.Group, eventLevelNum, eventNumInLevel });
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

QColor IMatchableEventPattern::Color::toQColor() const
{
    return QColor(R, G, B);
}

QString IMatchableEventPattern::Color::toColorCode() const
{
    return QString::number(R, 16).toUpper().rightJustified(2, '0') +
           QString::number(G, 16).toUpper().rightJustified(2, '0') +
           QString::number(B, 16).toUpper().rightJustified(2, '0');
}

IMatchableEventPattern::Color IMatchableEventPattern::Color::fromColorCode(const QString& colorCode)
{
    IMatchableEventPattern::Color color;
    color.R = static_cast<quint8>(colorCode.mid(0, 2).toInt(nullptr, 16));
    color.G = static_cast<quint8>(colorCode.mid(2, 2).toInt(nullptr, 16));
    color.B = static_cast<quint8>(colorCode.mid(4, 2).toInt(nullptr, 16));
    return color;
}

IMatchableEventPattern::Color IMatchableEventPattern::Color::fromQColor(const QColor& color)
{
    IMatchableEventPattern::Color result;
    result.R = static_cast<quint8>(color.red());
    result.G = static_cast<quint8>(color.green());
    result.B = static_cast<quint8>(color.blue());
    return result;
}

QString IEventInfoExtractor::GetGroupFromLine(const PositionedLine& line) const
{
    return GetGroupFromLine(line.Line);
}

QString IEventInfoExtractor::GetMessageFromLine(const PositionedLine& line) const
{
    return GetMessageFromLine(line.Line);
}

LogLineHeaderParsingParams LogLineHeaderParsingParams::FromJson(const QString& jsonData)
{
    std::unique_ptr<Json::CharReader> reader(Json::CharReaderBuilder().newCharReader());

    Json::Value root;

    const std::string data = jsonData.toStdString();
    std::string errs;
    if (!reader->parse(data.data(), data.data() + data.length(), &root, &errs))
    {
        throw std::invalid_argument(std::string("Invalid JSON data in ") + __FUNCTION__ + ": " + errs);
    }

    LogLineHeaderParsingParams params;

    if (root["HeaderGroupRegExps"].isNull() || root["HeaderGroupRegExps"].empty() || !root["HeaderGroupRegExps"].isArray())
    {
        throw std::invalid_argument("Invalid JSON data in LogLineHeaderParsingParams: HeaderGroupRegExps");
    }

    if (root["GroupNameForGrouping"].isNull() || root["GroupNameForGrouping"].empty())
    {
        throw std::invalid_argument("Invalid JSON data in LogLineHeaderParsingParams: GroupNameForGrouping");
    }

    if (root["SortingGroup"].isNull() || root["SortingGroup"].empty())
    {
        throw std::invalid_argument("Invalid JSON data in LogLineHeaderParsingParams: SortingGroup");
    }

    params.GroupNameForGrouping = QString::fromStdString(root["GroupNameForGrouping"].asString());
    params.SortingGroup = QString::fromStdString(root["SortingGroup"].asString());

    params.HeaderGroupRegExps.reserve(static_cast<int>(root["HeaderGroupRegExps"].size()));
    for (Json::ArrayIndex i = 0; i < root["HeaderGroupRegExps"].size(); ++i)
    {
        const Json::Value headerParsingInfo = root["HeaderGroupRegExps"][i];

        if (headerParsingInfo["GroupName"].isNull() || headerParsingInfo["GroupName"].empty())
        {
            throw std::invalid_argument("Invalid JSON data in LogLineHeaderParsingParams: GroupName");
        }

        if (headerParsingInfo["GroupRegExpString"].isNull() || headerParsingInfo["GroupRegExpString"].empty())
        {
            throw std::invalid_argument("Invalid JSON data in LogLineHeaderParsingParams: GroupRegExpString");
        }

        const LogLineHeaderParsingParams::GroupNameType group = QString::fromStdString(headerParsingInfo["GroupName"].asString());
        const LogLineHeaderParsingParams::GroupRegExpString regExp = QString::fromStdString(headerParsingInfo["GroupRegExpString"].asString());

        params.HeaderGroupRegExps.push_back(QPair<QString, QString>(group, regExp));
    }

    return params;
}

QString LogLineHeaderParsingParams::ToJson(const LogLineHeaderParsingParams& params)
{
    Json::Value root;

    root["GroupNameForGrouping"] = params.GroupNameForGrouping.toStdString();
    root["SortingGroup"] = params.SortingGroup.toStdString();

    Json::Value groupInfoArray(Json::ValueType::arrayValue);
    for (const auto& groupInfo : params.HeaderGroupRegExps)
    {
        Json::Value value;
        value["GroupName"] = groupInfo.first.toStdString();
        value["GroupRegExpString"] = groupInfo.second.toStdString();

        groupInfoArray.append(value);
    }
    root["HeaderGroupRegExps"] = groupInfoArray;

    std::unique_ptr<Json::StreamWriter> writer(Json::StreamWriterBuilder().newStreamWriter());
    std::stringstream ss;
    writer->write(root, &ss);
    return QString::fromStdString(ss.str());
}
