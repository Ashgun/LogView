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

namespace
{

void AddSubEventPatternsToTree(
        QTreeWidgetItem* parent,
        const std::vector<EventPatternsHierarchyNode>& subEventPatterns,
        std::map<QTreeWidgetItem*, IMatchableEventPatternPtr>& mapTreeItemsToEventPatterns)
{
    for (const auto& eventPatternNode : subEventPatterns)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(parent, QStringList() << eventPatternNode.Event->Name);
        mapTreeItemsToEventPatterns[item] = eventPatternNode.Event->Clone();

        AddSubEventPatternsToTree(item, eventPatternNode.SubEvents, mapTreeItemsToEventPatterns);
    }
}

void AddTreeItemChildrenToHierarchy(
        QTreeWidgetItem* parent,
        const std::map<QTreeWidgetItem*, IMatchableEventPatternPtr>& mapTreeItemsToEventPatterns,
        EventPatternsHierarchyNode& target)
{
    for (int i = 0; i < parent->childCount(); ++i)
    {
        auto item = parent->child(i);
        target.AddSubEventPattern(mapTreeItemsToEventPatterns.at(item)->Clone());

        if (item->childCount() > 0)
        {
            AddTreeItemChildrenToHierarchy(item, mapTreeItemsToEventPatterns, target.SubEvents.back());
        }
    }
}

} // namespace

EventPatternsEditDialog::EventPatternsEditDialog(QWidget *parent) :
    QDialog(parent)
{
    setMinimumSize(800, 800);

    gui_eventsEdit = new EventPatternEditWidget();

    gui_eventsTree = new EventsTreeEditWidget(
                         gui_eventsEdit, m_FocusCapturingNotifier.get(), EventsTreeEditWidget::PatternAddingPolicy::AddToTree);
    gui_globalEventsTree = new EventsTreeEditWidget(
                         gui_eventsEdit, m_FocusCapturingNotifier.get(), EventsTreeEditWidget::PatternAddingPolicy::AddToTopLevelOnly);

    m_FocusCapturingNotifier->RegisterObserver(gui_eventsTree);
    m_FocusCapturingNotifier->RegisterObserver(gui_globalEventsTree);

    QDialogButtonBox* buttons = new QDialogButtonBox(Qt::Orientation::Horizontal);
    buttons->addButton(QDialogButtonBox::Open);
    buttons->addButton(QDialogButtonBox::Save);
//    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);

    QVBoxLayout* topLevelBox = new QVBoxLayout;

    {
        QHBoxLayout* box = new QHBoxLayout;

        {
            QVBoxLayout* treesEditBox = new QVBoxLayout;
            treesEditBox->addWidget(gui_eventsTree);
            treesEditBox->addWidget(gui_globalEventsTree);

            box->addLayout(treesEditBox);
        }

        box->addWidget(gui_eventsEdit);

        topLevelBox->addLayout(box);
    }

    topLevelBox->addWidget(buttons);

    setLayout(topLevelBox);

//    connect(buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(slot_accepted()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(slot_rejected()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Open), SIGNAL(clicked()), SLOT(slot_open()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Save), SIGNAL(clicked()), SLOT(slot_save()), Qt::DirectConnection);

    connect(gui_eventsTree, SIGNAL(ItemChanged(EventsTreeEditWidget*)),
            this, SLOT(slot_ItemChanged(EventsTreeEditWidget*)), Qt::DirectConnection);
    connect(gui_globalEventsTree, SIGNAL(ItemChanged(EventsTreeEditWidget*)),
            this, SLOT(slot_ItemChanged(EventsTreeEditWidget*)), Qt::DirectConnection);

    m_currentEventsTree = nullptr;
}

void EventPatternsEditDialog::SetEventPatternsHierarchy(const EventPatternsHierarchy& eventPatternsHierarchy)
{
    for (const auto& eventPattern : eventPatternsHierarchy.TopLevelNodes)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << eventPattern.Event->Name);
        gui_eventsTree->GetItemsMap()[item] = eventPattern.Event->Clone();
        gui_eventsTree->AddTopLevelItem(item);

        AddSubEventPatternsToTree(item, eventPattern.SubEvents, gui_eventsTree->GetItemsMap());
    }

    gui_eventsTree->ExpandAll();
    gui_eventsTree->ClearSelection();

    for (const auto& eventPattern : eventPatternsHierarchy.GlobalUnexpectedEventPatterns)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << eventPattern.Event->Name);
        gui_globalEventsTree->GetItemsMap()[item] = eventPattern.Event->Clone();
        gui_globalEventsTree->AddTopLevelItem(item);

        AddSubEventPatternsToTree(item, eventPattern.SubEvents, gui_globalEventsTree->GetItemsMap());
    }

    gui_globalEventsTree->ExpandAll();
    gui_globalEventsTree->ClearSelection();
}

EventPatternsHierarchy EventPatternsEditDialog::GetEventPatternsHierarchy() const
{
    EventPatternsHierarchy result;

    for (int i = 0; i < gui_eventsTree->TopLevelItemCount(); ++i)
    {
        auto item = gui_eventsTree->TopLevelItem(i);
        result.AddEventPattern(gui_eventsTree->GetItemsMap().at(item)->Clone());

        if (item->childCount() > 0)
        {
            AddTreeItemChildrenToHierarchy(item, gui_eventsTree->GetItemsMap(), result.TopLevelNodes.back());
        }
    }

    for (int i = 0; i < gui_globalEventsTree->TopLevelItemCount(); ++i)
    {
        auto item = gui_globalEventsTree->TopLevelItem(i);
        result.AddGlobalUnexpectedEventPattern(gui_globalEventsTree->GetItemsMap().at(item)->Clone());

        if (item->childCount() > 0)
        {
            AddTreeItemChildrenToHierarchy(item, gui_globalEventsTree->GetItemsMap(), result.GlobalUnexpectedEventPatterns.back());
        }
    }

    return result;
}

void EventPatternsEditDialog::AcceptState()
{
    gui_eventsTree->AcceptState();
    gui_globalEventsTree->AcceptState();
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

void EventPatternsEditDialog::slot_ItemChanged(EventsTreeEditWidget* /*treeEditWidget*/)
{
}

