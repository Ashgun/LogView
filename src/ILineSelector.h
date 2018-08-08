#ifndef ILINESELECTOR_H
#define ILINESELECTOR_H

#include "EventPattern.h"

#include <string>

class ILineSelector
{
public:
    virtual bool LineShouldBeSelected(EventPattern::PatternString const& line) const = 0;

    virtual ~ILineSelector() = default;
};

#endif // ILINESELECTOR_H
