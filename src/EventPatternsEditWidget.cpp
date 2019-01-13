#include "EventPatternsEditWidget.h"

#include <QHBoxLayout>

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

EventPatternsEditWidget::EventPatternsEditWidget(const EventsTreeEditWidget::PatternAddingPolicy& addingPolicy, QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(800, 800);

    gui_eventsEdit = new EventPatternEditWidget();

    gui_eventsTree = new EventsTreeEditWidget(gui_eventsEdit, addingPolicy);

    QHBoxLayout* topLevelBox = new QHBoxLayout;

    topLevelBox->addWidget(gui_eventsTree);
    topLevelBox->addWidget(gui_eventsEdit);

    setLayout(topLevelBox);
}

void EventPatternsEditWidget::SetEventPatternsHierarchy(const std::vector<EventPatternsHierarchyNode> &eventPatternsHierarchy)
{
    for (const auto& eventPattern : eventPatternsHierarchy)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << eventPattern.Event->Name);
        gui_eventsTree->GetItemsMap()[item] = eventPattern.Event->Clone();
        gui_eventsTree->AddTopLevelItem(item);

        AddSubEventPatternsToTree(item, eventPattern.SubEvents, gui_eventsTree->GetItemsMap());
    }

    gui_eventsTree->ExpandAll();
    gui_eventsTree->ClearSelection();
}

std::vector<EventPatternsHierarchyNode> EventPatternsEditWidget::GetEventPatternsHierarchy() const
{
    std::vector<EventPatternsHierarchyNode> result;

    for (int i = 0; i < gui_eventsTree->TopLevelItemCount(); ++i)
    {
        auto item = gui_eventsTree->TopLevelItem(i);

        result.push_back(EventPatternsHierarchyNode());
        result.back().Event = std::move(gui_eventsTree->GetItemsMap().at(item)->Clone());

        if (item->childCount() > 0)
        {
            AddTreeItemChildrenToHierarchy(item, gui_eventsTree->GetItemsMap(), result.back());
        }
    }

    return result;
}

void EventPatternsEditWidget::AcceptState()
{
    gui_eventsTree->AcceptState();
}
