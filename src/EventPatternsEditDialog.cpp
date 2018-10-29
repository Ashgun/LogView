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

IMatchableEventPatternPtr CreateDefaultEventPattern(const QString& eventPatternName)
{
    return CreateSingleEventPattern(
                eventPatternName,
                EventPattern::CreateStringPattern(""),
                CreateColor(255, 255, 255));
}


} // namespace

EventPatternsEditDialog::EventPatternsEditDialog(QWidget *parent) :
    QDialog(parent)
{
    setMinimumSize(800, 600);

    gui_eventsTree = new EventsTreeWidget();
    gui_eventsTree->setHeaderLabels(QStringList() << tr("Event patterns"));
    gui_addEventPatternButton = new QPushButton(tr("Add event pattern"));
    gui_deleteEventPatternButton = new QPushButton(tr("Delete event pattern"));
    gui_deleteEventPatternButton->setEnabled(false);

    gui_eventsEdit = new EventPatternEditWidget();

    QDialogButtonBox* buttons = new QDialogButtonBox(Qt::Orientation::Horizontal);
    buttons->addButton(QDialogButtonBox::Open);
    buttons->addButton(QDialogButtonBox::Save);
//    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);

    QVBoxLayout* topLevelBox = new QVBoxLayout;

    {
        QHBoxLayout* box = new QHBoxLayout;

        {
            QVBoxLayout* treeEditBox = new QVBoxLayout;
            treeEditBox->addWidget(gui_eventsTree);

            {
                QHBoxLayout* buttonBox = new QHBoxLayout;
                buttonBox->addWidget(gui_addEventPatternButton);
                buttonBox->addWidget(gui_deleteEventPatternButton);
                treeEditBox->addLayout(buttonBox);
            }

            box->addLayout(treeEditBox);
        }

        box->addWidget(gui_eventsEdit);

        topLevelBox->addLayout(box);
    }

    topLevelBox->addWidget(buttons);

    setLayout(topLevelBox);

    connect(gui_eventsTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(slot_eventsTree_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            Qt::DirectConnection);

//    connect(buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(slot_accepted()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(slot_rejected()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Open), SIGNAL(clicked()), SLOT(slot_open()), Qt::DirectConnection);
    connect(buttons->button(QDialogButtonBox::Save), SIGNAL(clicked()), SLOT(slot_save()), Qt::DirectConnection);

    connect(gui_addEventPatternButton, SIGNAL(clicked(bool)),
            this, SLOT(slot_addEventPatternButton_clicked(bool)), Qt::DirectConnection);
    connect(gui_deleteEventPatternButton, SIGNAL(clicked(bool)),
            this, SLOT(slot_deleteEventPatternButton_clicked(bool)), Qt::DirectConnection);
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
    gui_eventsTree->clearSelection();
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

void EventPatternsEditDialog::UpdateItemByEventPatternEdit(QTreeWidgetItem* item)
{
    m_mapTreeItemsToEventPatterns[item] = gui_eventsEdit->GetPattern();
    item->setText(0, m_mapTreeItemsToEventPatterns[item]->Name);
}

void EventPatternsEditDialog::AcceptState()
{
    QTreeWidgetItem* currentItem = gui_eventsTree->currentItem();
    if (currentItem != nullptr)
    {
        UpdateItemByEventPatternEdit(currentItem);
    }
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

void EventPatternsEditDialog::slot_eventsTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    if (previous != nullptr)
    {
        UpdateItemByEventPatternEdit(previous);
    }

    if (current != nullptr)
    {
        gui_eventsEdit->SetLinePattern(m_mapTreeItemsToEventPatterns[current].get());
    }

    gui_deleteEventPatternButton->setEnabled(current != nullptr);
}

void EventPatternsEditDialog::slot_addEventPatternButton_clicked(bool)
{
    auto currentItem = gui_eventsTree->currentItem();

    const QString baseEventPatternName = "New event pattern";
    IMatchableEventPatternPtr eventPattern = CreateDefaultEventPattern(baseEventPatternName);
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

void EventPatternsEditDialog::slot_deleteEventPatternButton_clicked(bool)
{
    auto currentItem = gui_eventsTree->currentItem();

    if (currentItem == nullptr)
    {
        return;
    }

    m_mapTreeItemsToEventPatterns.erase(currentItem);
    delete currentItem;
}
