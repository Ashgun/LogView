#ifndef LINEMATCHER_H
#define LINEMATCHER_H

#include "EventPattern.h"

#include <QString>

class LineMatcher
{
public:
    LineMatcher(Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitivity::CaseInsensitive);

    bool IsPatternMatched(QString const& line, EventPattern const& pattern) const;

private:
    bool IsStringMatched(QString const& line, QString const& pattern) const;
    bool IsRegExpMatched(QString const& line, QString const& pattern) const;

private:
    Qt::CaseSensitivity const m_caseSensitivity;
};

#endif // LINEMATCHER_H
