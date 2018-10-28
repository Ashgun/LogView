#include "LogFileWithConfigsOpenDialog.h"

#include <QGridLayout>
#include <QHBoxLayout>

LogFileWithConfigsOpenDialog::LogFileWithConfigsOpenDialog(QWidget* parent) : QFileDialog(parent)
{
    setMinimumSize(1024, 700);
    setOption(QFileDialog::DontUseNativeDialog, true);

    gui_RecentHeaderParsingConfigWidget = new RecentFilesWidget("RecentHeaderParsingConfig", tr("Log line header parsing config"));
    gui_RecentEventPatternConfigWidget = new RecentFilesWidget("RecentEventPatternConfig", tr("Event pattern config"));

    QHBoxLayout* recentFilesWidgetsBox = new QHBoxLayout;
    recentFilesWidgetsBox->addWidget(gui_RecentHeaderParsingConfigWidget);
    recentFilesWidgetsBox->addWidget(gui_RecentEventPatternConfigWidget);

    dynamic_cast<QGridLayout*>(this->layout())->addLayout(
                recentFilesWidgetsBox,
                4, 0,
                1, dynamic_cast<QGridLayout*>(this->layout())->columnCount());
}

int LogFileWithConfigsOpenDialog::exec()
{
    int /*QDialog::DialogCode*/ dialog_code = QFileDialog::exec();

    if (dialog_code != QDialog::Accepted ||
        gui_RecentHeaderParsingConfigWidget->GetSelectedFile().isEmpty() ||
        gui_RecentEventPatternConfigWidget->GetSelectedFile().isEmpty()) {
        return QDialog::Rejected;
    }

    m_LogFileName = selectedFiles().first();
    m_HeaderParsingConfig = gui_RecentHeaderParsingConfigWidget->GetSelectedFile();
    m_EventPatternConfig = gui_RecentEventPatternConfigWidget->GetSelectedFile();

    return QDialog::Accepted;
}

QString LogFileWithConfigsOpenDialog::GetOpenLogFileName()
{
    return m_LogFileName;
}

QString LogFileWithConfigsOpenDialog::GetHeaderParsingConfig()
{
    return m_HeaderParsingConfig;
}

QString LogFileWithConfigsOpenDialog::GetEventPatternConfig()
{
    return m_EventPatternConfig;
}
