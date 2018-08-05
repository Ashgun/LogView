#ifndef ILOGLINEPARSER_H
#define ILOGLINEPARSER_H

#include "LogLineInfo.h"

#include <QString>

class ILogLineParser
{
public:
    virtual ~ILogLineParser() = default;

    virtual LogLineInfo Parse(QString const& line) const = 0;
};

#endif // ILOGLINEPARSER_H
