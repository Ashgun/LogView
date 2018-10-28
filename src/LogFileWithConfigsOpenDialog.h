#ifndef LOGFILEWITHCONFIGSOPENDIALOG_H
#define LOGFILEWITHCONFIGSOPENDIALOG_H

#include <QFileDialog>

#include "RecentFilesWidget.h"

class LogFileWithConfigsOpenDialog : public QFileDialog
{
public:
    explicit LogFileWithConfigsOpenDialog(QWidget *parent = nullptr);

    int exec() override;

    QString GetOpenLogFileName();
    QString GetEventPatternConfig();

private:
    QString m_LogFileName;
    QString m_HeaderParsingConfig;
    QString m_EventPatternConfig;

    RecentFilesWidget* gui_RecentEventPatternConfigWidget;
};

#endif // LOGFILEWITHCONFIGSOPENDIALOG_H
