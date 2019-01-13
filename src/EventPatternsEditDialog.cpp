#include "EventPatternsEditDialog.h"

#include "Common.h"
#include "Events.h"
#include "Utils.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPushButton>

#include <QDebug>

EventPatternsEditDialog::EventPatternsEditDialog(QWidget *parent) :
    QDialog(parent)
{
    setMinimumSize(800, 800);

    gui_eventsTreeWidget = new EventPatternsEditWidget(EventsTreeEditWidget::PatternAddingPolicy::AddToTree);
    gui_globalEventsTreeWidget = new EventPatternsEditWidget(EventsTreeEditWidget::PatternAddingPolicy::AddToTopLevelOnly);
    gui_ignoredEventsTreeWidget = new EventPatternsEditWidget(EventsTreeEditWidget::PatternAddingPolicy::AddToTopLevelOnly);

    gui_eventsTreesTab = new QTabWidget;
    gui_eventsTreesTab->addTab(gui_eventsTreeWidget, tr("Events to search"));
    gui_eventsTreesTab->addTab(gui_globalEventsTreeWidget, tr("Global events"));
    gui_eventsTreesTab->addTab(gui_ignoredEventsTreeWidget, tr("Ignored events"));

    QDialogButtonBox* buttons = new QDialogButtonBox(Qt::Orientation::Horizontal);
    buttons->addButton(QDialogButtonBox::Open);
    buttons->addButton(QDialogButtonBox::Save);
//    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);

    QVBoxLayout* topLevelBox = new QVBoxLayout;

    topLevelBox->addWidget(gui_eventsTreesTab);
    topLevelBox->addWidget(buttons);

    setLayout(topLevelBox);

//    connect(buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(slot_accepted()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(slot_rejected()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Open), SIGNAL(clicked()), SLOT(slot_open()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Save), SIGNAL(clicked()), SLOT(slot_save()), Qt::DirectConnection);
}

void EventPatternsEditDialog::SetEventPatternsHierarchy(const EventPatternsHierarchy& eventPatternsHierarchy)
{
    gui_eventsTreeWidget->SetEventPatternsHierarchy(eventPatternsHierarchy.TopLevelNodes);
    gui_globalEventsTreeWidget->SetEventPatternsHierarchy(eventPatternsHierarchy.GlobalUnexpectedEventPatterns);
    gui_ignoredEventsTreeWidget->SetEventPatternsHierarchy(eventPatternsHierarchy.IgnoredEventPatterns);
}

EventPatternsHierarchy EventPatternsEditDialog::GetEventPatternsHierarchy() const
{
    EventPatternsHierarchy result;

    result.TopLevelNodes = gui_eventsTreeWidget->GetEventPatternsHierarchy();
    result.GlobalUnexpectedEventPatterns = gui_globalEventsTreeWidget->GetEventPatternsHierarchy();
    result.IgnoredEventPatterns = gui_ignoredEventsTreeWidget->GetEventPatternsHierarchy();

    return result;
}

void EventPatternsEditDialog::AcceptState()
{
    gui_eventsTreeWidget->AcceptState();
    gui_globalEventsTreeWidget->AcceptState();
}

void EventPatternsEditDialog::slot_accepted()
{
    AcceptState();

//    qDebug() << "EventPatternsEditDialog::slot_accepted()";
    accept();
}

void EventPatternsEditDialog::slot_rejected()
{
//    qDebug() << "EventPatternsEditDialog::slot_rejected()";
    reject();
}

void EventPatternsEditDialog::slot_open()
{
    const QString caption(tr("Open event pattern configuration file"));

    m_openedFileName = QFileDialog::getOpenFileName(
                                 this, caption, QString(), Constants::ConfigFilesFilter);

    if (m_openedFileName.isEmpty())
    {
        return;
    }

    const QString eventsParsingConfigJson = LoadFileToQString(m_openedFileName);

    EventPatternsHierarchy patterns;
    EventPatternsHierarchy::fromJson(eventsParsingConfigJson, patterns);

    SetEventPatternsHierarchy(patterns);
}

void EventPatternsEditDialog::slot_save()
{
    AcceptState();

    if (m_openedFileName.isEmpty())
    {
        const QString caption(tr("Save event pattern configuration file"));
        m_openedFileName = QFileDialog::getSaveFileName(
                                     this, caption, QString(), Constants::ConfigFilesFilter);
    }

    EventPatternsHierarchy patterns = GetEventPatternsHierarchy();

    const QString eventsParsingConfigJson = EventPatternsHierarchy::toJson(patterns);

    SaveQStringToFile(eventsParsingConfigJson, m_openedFileName);
}
