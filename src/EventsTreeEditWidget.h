#ifndef EVENTSTREEEDIDWIDGET_H
#define EVENTSTREEEDIDWIDGET_H

#include <QWidget>
#include <QPushButton>

#include "EventsTreeWidget.h"

#include "EventPatternEditWidget.h"
#include "Events.h"

#include <map>
#include <memory>

class QTreeWidgetItem;

class EventsTreeEditWidget : public QWidget
{
    Q_OBJECT

public:
    enum PatternAddingPolicy
    {
        AddToTree,
        AddToTopLevelOnly
    };

public:
    explicit EventsTreeEditWidget(
            EventPatternEditWidget* eventsEdit,
            const PatternAddingPolicy patternAddingPolicy,
            QWidget *parent = nullptr);

    std::map<QTreeWidgetItem*, IMatchableEventPatternPtr>& GetItemsMap();

    void AddTopLevelItem(QTreeWidgetItem* item);
    void ExpandAll();
    void ClearSelection();

    int TopLevelItemCount() const;
    QTreeWidgetItem* TopLevelItem(const int index);

    void AcceptState();

private:
    void UpdateItemByEventPatternEdit(QTreeWidgetItem* item);
    void UpdateEventPatternEditIfPossible(QTreeWidgetItem* item);

signals:
    void ItemChanged(EventsTreeEditWidget* treeEditWidget);

public slots:
    void slot_eventsTree_clicked();
    void slot_eventsTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void slot_addEventPatternButton_clicked(bool);
    void slot_addTopLevelEventPatternButton_clicked(bool);
    void slot_deleteEventPatternButton_clicked(bool);

private:
    EventsTreeWidget* gui_eventsTree;
    const PatternAddingPolicy m_patternAddingPolicy;

    EventPatternEditWidget* gui_eventsEdit;

    QPushButton* gui_addEventPatternButton;
    QPushButton* gui_addTopLevelEventPatternButton;
    QPushButton* gui_deleteEventPatternButton;

    std::map<QTreeWidgetItem*, IMatchableEventPatternPtr> m_mapTreeItemsToEventPatterns;

    bool m_FocusLost = false;
    bool m_savedOnFocusLoose = false;
};

#endif // EVENTSTREEEDIDWIDGET_H
