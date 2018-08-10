#pragma once

#include "EventPattern.h"
#include "LinePosition.h"

struct PositionedLine
{
    EventPattern::PatternString Line;
    LinePosition Position;
    int LevelInHierarchy;

    PositionedLine();
    PositionedLine(EventPattern::PatternString const& line, LinePosition const& position, int const levelInHierarchy);
    explicit PositionedLine(PositionedLine const& other);
    bool IsEmpty() const;
    PositionedLine& operator=(PositionedLine const& other);
};
