#include "EventPatternsEditDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDialogButtonBox>

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
    setMinimumSize(800, 600);

    gui_eventsTree = new EventsTreeWidget();
    gui_eventsTree->setHeaderLabels(QStringList() << tr("Event patterns"));

    gui_eventsEdit = new EventPatternEditWidget();

    QDialogButtonBox* buttons = new QDialogButtonBox(
                                    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Orientation::Horizontal);

    QVBoxLayout* topLevelBox = new QVBoxLayout;

    {
        QHBoxLayout* box = new QHBoxLayout;
        box->addWidget(gui_eventsTree);
        box->addWidget(gui_eventsEdit);

        topLevelBox->addLayout(box);
    }

    topLevelBox->addWidget(buttons);

    setLayout(topLevelBox);

    connect(gui_eventsTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(slot_eventsTree_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            Qt::DirectConnection);

    connect(buttons, SIGNAL(accepted()), this, SLOT(slot_accepted()), Qt::DirectConnection);
    connect(buttons, SIGNAL(rejected()), this, SLOT(slot_rejected()), Qt::DirectConnection);
}

void EventPatternsEditDialog::SetEventPatternsHierarchy(const EventPatternsHierarchy& eventPatternsHierarchy)
{
    for (const auto& eventPattern : eventPatternsHierarchy.TopLevelNodes)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << eventPattern.Event->Name);
        m_mapTreeItemsToEventPatterns[item] = eventPattern.Event->Clone();
        gui_eventsTree->addTopLevelItem(item);

        AddSubEventPatternsToTree(item, eventPattern.SubEvents, m_mapTreeItemsToEventPatterns);
    }

    gui_eventsTree->expandAll();
}

EventPatternsHierarchy EventPatternsEditDialog::GetEventPatternsHierarchy() const
{
    EventPatternsHierarchy result;

    for (int i = 0; i < gui_eventsTree->topLevelItemCount(); ++i)
    {
        auto item = gui_eventsTree->topLevelItem(i);
        result.AddEventPattern(m_mapTreeItemsToEventPatterns.at(item)->Clone());

        if (item->childCount() > 0)
        {
            AddTreeItemChildrenToHierarchy(item, m_mapTreeItemsToEventPatterns, result.TopLevelNodes.back());
        }
    }

    return result;
}

void EventPatternsEditDialog::slot_accepted()
{
    QTreeWidgetItem* currentItem = gui_eventsTree->currentItem();
    if (currentItem != nullptr)
    {
        gui_eventsEdit->SetLinePattern(m_mapTreeItemsToEventPatterns[currentItem].get());
    }

    qDebug() << "EventPatternsEditDialog::slot_accepted()";
    accept();
}

void EventPatternsEditDialog::slot_rejected()
{
    qDebug() << "EventPatternsEditDialog::slot_rejected()";
    reject();
}

void EventPatternsEditDialog::slot_eventsTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (previous != nullptr)
    {
        m_mapTreeItemsToEventPatterns[previous] = gui_eventsEdit->GetPattern();
        previous->setText(0, m_mapTreeItemsToEventPatterns[previous]->Name);
    }

    if (current != nullptr)
    {
        gui_eventsEdit->SetLinePattern(m_mapTreeItemsToEventPatterns[current].get());
    }
}
