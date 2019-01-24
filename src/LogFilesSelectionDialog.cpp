#include "LogFilesSelectionDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

LogFilesSelectionDialog::LogFilesSelectionDialog(QWidget *parent) : QDialog(parent)
{
    setMinimumWidth(800);

    gui_buttonBox = new QDialogButtonBox(Qt::Orientation::Horizontal);
    gui_buttonBox->addButton(QDialogButtonBox::Ok);
    gui_buttonBox->addButton(QDialogButtonBox::Cancel);

    QVBoxLayout* topLevelBox = new QVBoxLayout;

    const int maxLogGroupsCount = 3;
    for (int i = 0; i < maxLogGroupsCount; ++i)
    {
        LogFilesSelectionWidget* logFilesSelectionWidget = new LogFilesSelectionWidget();
        topLevelBox->addWidget(logFilesSelectionWidget);
        gui_LogFilesSelectionWidgets.append(logFilesSelectionWidget);

        connect(logFilesSelectionWidget, SIGNAL(FilesSelected()),
                this, SLOT(slot_LogFilesSelectionWidget_FilesSelected()),
                Qt::DirectConnection);
    }

    topLevelBox->addWidget(gui_buttonBox);

    setLayout(topLevelBox);

    gui_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(gui_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(slot_accepted()), Qt::DirectConnection);
    connect(gui_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(slot_rejected()), Qt::DirectConnection);
}

const QVector<LogFilesSelectionDialog::LogFilesSelectionData> &LogFilesSelectionDialog::GetLogFilesSelectionData() const
{
    return m_obtainedData;
}

void LogFilesSelectionDialog::slot_accepted()
{
    for (const LogFilesSelectionWidget* widget : gui_LogFilesSelectionWidgets)
    {
        if (widget->GetOpenLogFileNames().isEmpty() ||
            widget->GetEventPatternConfig().isEmpty())
        {
            continue;
        }

        m_obtainedData.append(LogFilesSelectionData({widget->GetOpenLogFileNames(), widget->GetEventPatternConfig()}));
    }

    accept();
}

void LogFilesSelectionDialog::slot_rejected()
{
    reject();
}

void LogFilesSelectionDialog::slot_LogFilesSelectionWidget_FilesSelected()
{
    gui_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
