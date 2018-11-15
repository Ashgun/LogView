#ifndef EVENTPATTERNSEDITDIALOG_H
#define EVENTPATTERNSEDITDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QPushButton>

#include "EventPatternEditWidget.h"
#include "EventsTreeEditWidget.h"

#include "Events.h"

#include <map>

class QTreeWidgetItem;

class EventPatternsEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EventPatternsEditDialog(QWidget *parent = nullptr);

    void SetEventPatternsHierarchy(const EventPatternsHierarchy& eventPatternsHierarchy);
    EventPatternsHierarchy GetEventPatternsHierarchy() const;

private:
    void AcceptState();

signals:

protected slots:
    void slot_accepted();
    void slot_rejected();
    void slot_open();
    void slot_save();

    void slot_ItemChanged(EventsTreeEditWidget* treeEditWidget);

private:
    EventPatternEditWidget* gui_eventsEdit;

    std::unique_ptr<FocusCapturingNotifier> m_FocusCapturingNotifier = GetFocusCapturingNotifier();

    EventsTreeEditWidget* gui_eventsTree;
    EventsTreeEditWidget* gui_globalEventsTree;

    EventsTreeEditWidget* m_currentEventsTree;

    QString m_openedFileName;
};

#endif // EVENTPATTERNSEDITDIALOG_H
