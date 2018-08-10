#include "PositionedLine.h"

PositionedLine::PositionedLine() :
    Line(),
    Position(),
    LevelInHierarchy(-1)
{
}

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

PositionedLine&PositionedLine::operator=(const PositionedLine& other)
{
    Line = other.Line;
    Position = other.Position;
    LevelInHierarchy = other.LevelInHierarchy;

    return *this;
}
