#include "PositionedLine.h"

PositionedLine::PositionedLine(const EventPattern::PatternString& line, const LinePosition& position,
    const int levelInHierarchy) :
    Line(line),
    Position(position),
    LevelInHierarchy(levelInHierarchy)
{
}

PositionedLine::PositionedLine(const PositionedLine& other) :
    Line(other.Line),
    Position(other.Position),
    LevelInHierarchy(other.LevelInHierarchy)
{
}

bool PositionedLine::IsEmpty() const
{
    return Line.isEmpty();
}
