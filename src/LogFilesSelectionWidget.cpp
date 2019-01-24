#include "LogFilesSelectionWidget.h"

#include "LogFileWithConfigsOpenDialog.h"

#include <QHBoxLayout>

LogFilesSelectionWidget::LogFilesSelectionWidget(QWidget *parent)
    : QWidget(parent)
{
    gui_fileList = new QListWidget();

    gui_openFilesButton = new QPushButton(tr("Open files"));

    QHBoxLayout* box = new QHBoxLayout;

    box->addWidget(gui_fileList);
    box->addWidget(gui_openFilesButton);

    setLayout(box);

    connect(gui_openFilesButton, SIGNAL(clicked(bool)),
            this, SLOT(slot_openFilesButton_clicked(bool)),
            Qt::DirectConnection);
}

QStringList LogFilesSelectionWidget::GetOpenLogFileNames() const
{
    return m_LogFileNames;
}

QString LogFilesSelectionWidget::GetEventPatternConfig() const
{
    return m_EventPatternConfig;
}

void LogFilesSelectionWidget::slot_openFilesButton_clicked(bool)
{
    LogFileWithConfigsOpenDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    m_EventPatternConfig = dialog.GetEventPatternConfig();
    m_LogFileNames = dialog.GetOpenLogFileNames();

    gui_fileList->clear();
    gui_fileList->addItems(m_LogFileNames);

    emit FilesSelected();
}
