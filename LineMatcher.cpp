#include "LineMatcher.h"

#include <QRegExp>

#include <stdexcept>

LineMatcher::LineMatcher(Qt::CaseSensitivity caseSensitivity) :
    m_caseSensitivity(caseSensitivity)
{
}

bool LineMatcher::IsPatternMatched(const QString& line, const EventPattern& pattern) const
{
    static_assert(EventPattern::PatternType::Count == 2, "EventPattern::PatternType was changed, update switch");

    switch (pattern.Type) {
    case EventPattern::PatternType::String:
        return IsStringMatched(line, pattern.Pattern);
        break;
    case EventPattern::PatternType::RegExp:
        return IsRegExpMatched(line, pattern.Pattern);
        break;
    default:
        break;
    }

    throw std::invalid_argument("[LineMatcher] Unsupported EventPattern::PatternType");
}

bool LineMatcher::IsStringMatched(const QString& line, const QString& pattern) const
{
    return line.contains(pattern, Qt::CaseSensitivity::CaseInsensitive);
}

bool LineMatcher::IsRegExpMatched(const QString& line, const QString& pattern) const
{
    return line.contains(QRegExp(pattern));
}
