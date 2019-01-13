#include "EventsTreeEditWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

#include <vector>

namespace
{

IMatchableEventPatternPtr CreateDefaultEventPattern(const QString& eventPatternName)
{
    return CreateSingleEventPattern(
                eventPatternName,
                EventPattern::CreateStringPattern(""),
                CreateColor(200, 200, 200));
}

} // namespace

EventsTreeEditWidget::EventsTreeEditWidget(
        EventPatternEditWidget* eventsEdit,
        const PatternAddingPolicy patternAddingPolicy,
        QWidget *parent) :
    QWidget(parent),
    m_patternAddingPolicy(patternAddingPolicy),
    gui_eventsEdit(eventsEdit)
{
    setMinimumWidth(375);

    gui_eventsTree = new EventsTreeWidget();
    gui_eventsTree->setHeaderLabels(QStringList() << tr("Event patterns"));
    gui_addEventPatternButton = new QPushButton(tr("Add event pattern"));
    gui_deleteEventPatternButton = new QPushButton(tr("Delete event pattern"));
    gui_deleteEventPatternButton->setEnabled(false);

    QVBoxLayout* treeEditBox = new QVBoxLayout;
    treeEditBox->addWidget(gui_eventsTree);

    {
        QHBoxLayout* buttonBox = new QHBoxLayout;
        buttonBox->addWidget(gui_addEventPatternButton);
        buttonBox->addWidget(gui_deleteEventPatternButton);
        treeEditBox->addLayout(buttonBox);
    }

    setLayout(treeEditBox);

    connect(gui_eventsTree, SIGNAL(clicked()),
            this, SLOT(slot_eventsTree_clicked()),
            Qt::DirectConnection);
    connect(gui_eventsTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(slot_eventsTree_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            Qt::DirectConnection);

    connect(gui_addEventPatternButton, SIGNAL(clicked(bool)),
            this, SLOT(slot_addEventPatternButton_clicked(bool)), Qt::DirectConnection);
    connect(gui_deleteEventPatternButton, SIGNAL(clicked(bool)),
            this, SLOT(slot_deleteEventPatternButton_clicked(bool)), Qt::DirectConnection);
}

std::map<QTreeWidgetItem*, IMatchableEventPatternPtr>& EventsTreeEditWidget::GetItemsMap()
{
    return m_mapTreeItemsToEventPatterns;
}

void EventsTreeEditWidget::AddTopLevelItem(QTreeWidgetItem* item)
{
    gui_eventsTree->addTopLevelItem(item);
}

void EventsTreeEditWidget::ExpandAll()
{
    gui_eventsTree->expandAll();
}

void EventsTreeEditWidget::ClearSelection()
{
    gui_eventsTree->clearSelection();
}

int EventsTreeEditWidget::TopLevelItemCount() const
{
    return gui_eventsTree->topLevelItemCount();
}

QTreeWidgetItem*EventsTreeEditWidget::TopLevelItem(const int index)
{
    return gui_eventsTree->topLevelItem(index);
}

void EventsTreeEditWidget::UpdateItemByEventPatternEdit(QTreeWidgetItem* item)
{
    m_mapTreeItemsToEventPatterns[item] = gui_eventsEdit->GetPattern();
    item->setText(0, m_mapTreeItemsToEventPatterns[item]->Name);
}

void EventsTreeEditWidget::UpdateEventPatternEditIfPossible(QTreeWidgetItem* item)
{
    if (item != nullptr)
    {
        gui_eventsEdit->SetLinePattern(m_mapTreeItemsToEventPatterns[item].get());
    }

    gui_deleteEventPatternButton->setEnabled(item != nullptr);
}

void EventsTreeEditWidget::slot_eventsTree_clicked()
{
    QTreeWidgetItem *current = gui_eventsTree->currentItem();
    if (current != nullptr)
    {
        UpdateEventPatternEditIfPossible(current);
    }
}

void EventsTreeEditWidget::slot_eventsTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (!m_FocusLost)
    {
        if (previous != nullptr)
        {
            UpdateItemByEventPatternEdit(previous);
        }
    }

    UpdateEventPatternEditIfPossible(current);

    emit ItemChanged(this);
}

void EventsTreeEditWidget::slot_addEventPatternButton_clicked(bool)
{
    const QString baseEventPatternName = "New event pattern";
    IMatchableEventPatternPtr eventPattern = CreateDefaultEventPattern(baseEventPatternName);

    QTreeWidgetItem* currentItem =
            m_patternAddingPolicy == PatternAddingPolicy::AddToTree ?
                gui_eventsTree->currentItem() : nullptr;
    QTreeWidgetItem* item = nullptr;
    if (currentItem == nullptr)
    {
        item = new QTreeWidgetItem(QStringList() << eventPattern->Name);
        gui_eventsTree->addTopLevelItem(item);
    }
    else
    {
        item = new QTreeWidgetItem(currentItem, QStringList() << eventPattern->Name);
    }

    m_mapTreeItemsToEventPatterns[item] = std::move(eventPattern);
    gui_eventsTree->expandAll();
    gui_eventsTree->setCurrentItem(item);
}

void EventsTreeEditWidget::AcceptState()
{
    QTreeWidgetItem* currentItem = gui_eventsTree->currentItem();
    if (currentItem != nullptr && !m_FocusLost)
    {
        UpdateItemByEventPatternEdit(currentItem);
    }
}

void EventsTreeEditWidget::slot_deleteEventPatternButton_clicked(bool)
{
    auto currentItem = gui_eventsTree->currentItem();

    if (currentItem == nullptr)
    {
        return;
    }

    m_mapTreeItemsToEventPatterns.erase(currentItem);
    delete currentItem;
}
