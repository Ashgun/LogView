#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QString>

namespace Constants
{

static const QString AppConfig = "Config.ini";
static const QString LogLinesHeaderParsingConfig = "General/LogLinesHeaderParsingConfig";
static const QString ConfigFilesFilter = QObject::tr("JSON files (*.json)");

static const QString RecentDataConfig = "RecentData.ini";

namespace SpecialEventsLevels
{

static const int GlobalEventsLevel = 100;

} // namespace SpecialEventsLevels

} // namespace Constants

#endif // COMMON_H
