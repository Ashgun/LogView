#ifndef ILINESELECTOR_H
#define ILINESELECTOR_H

#include <string>

class ILineSelector
{
public:
    virtual bool LineShouldBeSelected(std::string const& line) const = 0;

    virtual ~ILineSelector() = default;
};

#endif // ILINESELECTOR_H
