#include "LogFileWithConfigsOpenDialog.h"

#include <QGridLayout>

LogFileWithConfigsOpenDialog::LogFileWithConfigsOpenDialog(QWidget* parent) : QFileDialog(parent)
{
    setMinimumSize(1024, 700);
    setOption(QFileDialog::DontUseNativeDialog, true);
    setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    setWindowTitle(tr("Open log files sequence"));
    setNameFilter(tr("Log files (*.log)"));
    setFileMode(QFileDialog::ExistingFiles);

    gui_RecentEventPatternConfigWidget = new RecentFilesWidget("RecentEventPatternConfig", tr("Event pattern config"));

    dynamic_cast<QGridLayout*>(this->layout())->addWidget(
                gui_RecentEventPatternConfigWidget,
                4, 0,
                1, dynamic_cast<QGridLayout*>(this->layout())->columnCount());
}

int LogFileWithConfigsOpenDialog::exec()
{
    int /*QDialog::DialogCode*/ dialog_code = QFileDialog::exec();

    if (dialog_code != QDialog::Accepted ||
        gui_RecentEventPatternConfigWidget->GetSelectedFile().isEmpty())
    {
        return QDialog::Rejected;
    }

    m_LogFileNames = selectedFiles();
    m_EventPatternConfig = gui_RecentEventPatternConfigWidget->GetSelectedFile();

    return QDialog::Accepted;
}

QStringList LogFileWithConfigsOpenDialog::GetOpenLogFileNames()
{
    return m_LogFileNames;
}

QString LogFileWithConfigsOpenDialog::GetEventPatternConfig()
{
    return m_EventPatternConfig;
}
