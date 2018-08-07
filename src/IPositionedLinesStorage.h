#pragma once

#include "LinePosition.h"

#include <utility>

using PositionedLine = std::pair<std::string const, LinePosition const>;

class IPositionedLinesStorage
{
public:
    virtual void AddLine(std::string const& line, LinePosition const& position)
    {
        AddLine(PositionedLine(line, position));
    }
    virtual void AddLine(PositionedLine const& positionedLine) = 0;
    virtual std::size_t Size() const = 0;
    virtual PositionedLine const& operator[](std::size_t const index) const = 0;
    virtual PositionedLine& operator[](std::size_t const index) = 0;

    virtual ~IPositionedLinesStorage() = default;
};
