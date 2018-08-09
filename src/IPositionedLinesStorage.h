#pragma once

#include "EventPattern.h"
#include "LinePosition.h"
#include "PositionedLine.h"

#include <memory>
#include <utility>

class IPositionedLinesStorage
{
public:
    virtual void AddLine(EventPattern::PatternString const& line, LinePosition const& position, int const hierarchyLevel)
    {
        AddLine(PositionedLine(line, position, hierarchyLevel));
    }
    virtual void AddLine(PositionedLine const& positionedLine) = 0;
    virtual std::size_t Size() const = 0;
    virtual PositionedLine const& operator[](std::size_t const index) const = 0;
    virtual PositionedLine& operator[](std::size_t const index) = 0;

    virtual std::unique_ptr<IPositionedLinesStorage> Clone() const = 0;

    virtual ~IPositionedLinesStorage() = default;
};
