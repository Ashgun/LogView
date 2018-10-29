#include "RecentFilesWidget.h"

#include "Common.h"

#include <QSettings>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QGroupBox>

namespace
{

namespace MagickNumber
{

constexpr int RecentFileMaxCount = 20;

} // namespace MagickNumber

} // namespace

RecentFilesWidget::RecentFilesWidget(const QString& filesGroupInConfig, const QString& configData, QWidget *parent) :
    QWidget(parent),
    m_filesGroupInConfig(filesGroupInConfig),
    m_configData(configData)
{
    setMinimumSize(200, 200);

    gui_selectedFileEdit = new QLineEdit;
    gui_openFileButton = new QPushButton(tr("Open file"));
    gui_recentFilesListWidget = new QListWidget;

    QHBoxLayout* selectedFileBox = new QHBoxLayout;
    selectedFileBox->addWidget(gui_selectedFileEdit);
    selectedFileBox->addWidget(gui_openFileButton);

    QVBoxLayout* box = new QVBoxLayout;
    box->addWidget(new QLabel(tr("Selected file:")));
    box->addLayout(selectedFileBox);
    box->addWidget(new QLabel(tr("Recent files:")));
    box->addWidget(gui_recentFilesListWidget);

    setLayout(box);

    QGroupBox* recentFilesGroup = new QGroupBox(m_configData);
    recentFilesGroup->setLayout(box);

    QVBoxLayout* recentFilesGroupContainer = new QVBoxLayout();
    recentFilesGroupContainer->addWidget(recentFilesGroup);

    setLayout(recentFilesGroupContainer);

    connect(gui_openFileButton, SIGNAL(clicked(bool)),
            this, SLOT(slot_openFileButton_clicked(bool)),
            Qt::DirectConnection);

    connect(gui_recentFilesListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(slot_recentFilesListWidget_currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            Qt::DirectConnection);

    FillFromConfig();
}

RecentFilesWidget::~RecentFilesWidget()
{
    const QString selectedFilename = gui_selectedFileEdit->text();

    QStringList recentFiles;

    recentFiles << selectedFilename;

    for (int i = 0; i < gui_recentFilesListWidget->count(); ++i)
    {
        if (recentFiles.size() >= MagickNumber::RecentFileMaxCount)
        {
            break;
        }

        if (gui_recentFilesListWidget->item(i)->text() != selectedFilename)
        {
            recentFiles << gui_recentFilesListWidget->item(i)->text();
        }
    }

    QSettings settings(Constants::RecentDataConfig, QSettings::Format::IniFormat);
    settings.setValue(m_filesGroupInConfig, recentFiles);
    settings.sync();
}

QString RecentFilesWidget::GetSelectedFile() const
{
    return gui_selectedFileEdit->text();
}

void RecentFilesWidget::slot_openFileButton_clicked(bool)
{
    const QString caption(tr("Open %1 file").arg(m_configData));

    const QString selectedFilename = QFileDialog::getOpenFileName(
                                 this, caption, QString(), tr("JSON files (*.json)"));

    if (selectedFilename.isEmpty())
    {
        return;
    }

    gui_selectedFileEdit->setText(selectedFilename);

    QStringList recentFiles;

    recentFiles << selectedFilename;

    for (int i = 0; i < gui_recentFilesListWidget->count(); ++i)
    {
        if (recentFiles.size() >= MagickNumber::RecentFileMaxCount)
        {
            break;
        }

        if (gui_recentFilesListWidget->item(i)->text() != selectedFilename)
        {
            recentFiles << gui_recentFilesListWidget->item(i)->text();
        }
    }

    gui_recentFilesListWidget->clear();
    gui_recentFilesListWidget->addItems(recentFiles);
    gui_recentFilesListWidget->setCurrentRow(0);
}

void RecentFilesWidget::slot_recentFilesListWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* /*previous*/)
{
    gui_selectedFileEdit->setText(current->text());
}

void RecentFilesWidget::FillFromConfig()
{
    QSettings settings(Constants::RecentDataConfig, QSettings::Format::IniFormat);

    if (settings.value(m_filesGroupInConfig).isNull())
    {
        return;
    }

    QStringList recentFiles = settings.value(m_filesGroupInConfig).toStringList();

    // Remove not available files
    for (auto& recentFileName : recentFiles)
    {
        QFile file(recentFileName);
        if (!file.exists())
        {
            recentFileName.clear();
        }
    }

    recentFiles.removeAll(QString(""));
    recentFiles.removeDuplicates();

    gui_selectedFileEdit->clear();
    if (!recentFiles.isEmpty())
    {
        gui_selectedFileEdit->setText(recentFiles.front());
    }

    gui_recentFilesListWidget->clear();
    gui_recentFilesListWidget->addItems(recentFiles);
}
