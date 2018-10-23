#ifndef EVENTPATTERNSEDITDIALOG_H
#define EVENTPATTERNSEDITDIALOG_H

#include <QWidget>
#include <QDialog>

#include "EventPatternEditWidget.h"
#include "EventsTreeWidget.h"

#include "Events.h"

#include <map>

class QTreeWidgetItem;

class EventPatternsEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EventPatternsEditDialog(QWidget *parent = nullptr);

    void SetEventPatternsHierarchy(const EventPatternsHierarchy& eventPatternsHierarchy);

signals:

protected slots:
    void slot_accepted();
    void slot_rejected();
    void slot_eventsTree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    EventsTreeWidget* gui_eventsTree;
    EventPatternEditWidget* gui_eventsEdit;

    std::map<QTreeWidgetItem*, IMatchableEventPatternPtr> m_mapTreeItemsToEventPatterns;
};

#endif // EVENTPATTERNSEDITDIALOG_H
