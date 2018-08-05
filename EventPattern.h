#ifndef EVENTPATTERN_H
#define EVENTPATTERN_H

#include <QString>

class EventPattern
{
public:
    typedef enum tag_PatternType
    {
        String = 0,
        RegExp,

        Count
    } PatternType;

    typedef QString PatternString;

public:
    static EventPattern CreateStringPattern(PatternString const& str);
    static EventPattern CreateRegExpPattern(PatternString const& str);

public:
    PatternType Type;
    PatternString Pattern;
};

#endif // EVENTPATTERN_H
