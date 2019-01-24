#ifndef LOGFILESSELECTIONDIALOG_H
#define LOGFILESSELECTIONDIALOG_H

#include <QDialog>
#include <QVector>
#include <QDialogButtonBox>

#include "LogFilesSelectionWidget.h"

class LogFilesSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    struct LogFilesSelectionData
    {
        QStringList LogFileNames;
        QString EventPatternConfig;
    };

public:
    explicit LogFilesSelectionDialog(QWidget *parent = nullptr);

    const QVector<LogFilesSelectionData>& GetLogFilesSelectionData() const;

signals:

private slots:
    void slot_accepted();
    void slot_rejected();

    void slot_LogFilesSelectionWidget_FilesSelected();

private:
    QVector<LogFilesSelectionWidget*> gui_LogFilesSelectionWidgets;
    QDialogButtonBox* gui_buttonBox;

    QVector<LogFilesSelectionData> m_obtainedData;
};

#endif // LOGFILESSELECTIONDIALOG_H
