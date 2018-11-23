#include "LogLineHeaderParsingParamsEditDialog.h"

#include "Common.h"
#include "Events.h"
#include "Utils.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>


LogLineHeaderParsingParamsEditDialog::LogLineHeaderParsingParamsEditDialog(QWidget *parent) : QDialog(parent)
{
    setMinimumSize(800, 800);

    gui_paramsEditWidget = new LogLineHeaderParsingParamsEditWidget();

    QDialogButtonBox* buttons = new QDialogButtonBox(Qt::Orientation::Horizontal);
    buttons->addButton(QDialogButtonBox::Open);
    buttons->addButton(QDialogButtonBox::Save);
//    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);

    QVBoxLayout* topLevelBox = new QVBoxLayout;

    topLevelBox->addWidget(gui_paramsEditWidget);
    topLevelBox->addWidget(buttons);

    setLayout(topLevelBox);

//    connect(buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(slot_accepted()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(slot_rejected()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Open), SIGNAL(clicked()), this, SLOT(slot_open()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(slot_save()), Qt::DirectConnection);
}

void LogLineHeaderParsingParamsEditDialog::slot_accepted()
{
    AcceptState();
    accept();
}

void LogLineHeaderParsingParamsEditDialog::slot_rejected()
{
    reject();
}

void LogLineHeaderParsingParamsEditDialog::slot_open()
{
    const QString caption(tr("Open log line header parsing configuration file"));
    m_openedFileName = QFileDialog::getOpenFileName(
                                 this, caption, QString(), Constants::ConfigFilesFilter);

    const QString logLineParsingConfigJson = LoadFileToQString(m_openedFileName);

    const LogLineHeaderParsingParams params = LogLineHeaderParsingParams::FromJson(logLineParsingConfigJson);

    gui_paramsEditWidget->SetParams(params);
}

void LogLineHeaderParsingParamsEditDialog::slot_save()
{
    AcceptState();

    if (m_openedFileName.isEmpty())
    {
        const QString caption(tr("Save log line parsing configuration file"));
        m_openedFileName = QFileDialog::getSaveFileName(
                                     this, caption, QString(), Constants::ConfigFilesFilter);
    }

    const LogLineHeaderParsingParams params = gui_paramsEditWidget->GetParams();

    const QString eventsParsingConfigJson = LogLineHeaderParsingParams::ToJson(params);

    SaveQStringToFile(eventsParsingConfigJson, m_openedFileName);
}

void LogLineHeaderParsingParamsEditDialog::AcceptState()
{
}
