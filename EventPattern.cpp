#include "EventPattern.h"


EventPattern EventPattern::CreateStringPattern(const EventPattern::PatternString& str)
{
    return EventPattern({ EventPattern::PatternType::String, str });
}

EventPattern EventPattern::CreateRegExpPattern(const EventPattern::PatternString& str)
{
    return EventPattern({ EventPattern::PatternType::RegExp, str });
}
