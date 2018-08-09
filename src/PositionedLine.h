#pragma once

#include "EventPattern.h"
#include "LinePosition.h"

struct PositionedLine
{
    EventPattern::PatternString const Line;
    LinePosition const Position;
    int const LevelInHierarchy;

    PositionedLine(EventPattern::PatternString const& line, LinePosition const& position, int const levelInHierarchy);
    explicit PositionedLine(PositionedLine const& other);
    bool IsEmpty() const;
};
