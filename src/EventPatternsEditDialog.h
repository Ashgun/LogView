#ifndef EVENTPATTERNSEDITDIALOG_H
#define EVENTPATTERNSEDITDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QTabWidget>

#include "EventPatternsEditWidget.h"

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

private:
    EventPatternsEditWidget* gui_eventsTreeWidget;
    EventPatternsEditWidget* gui_globalEventsTreeWidget;

    QTabWidget* gui_eventsTreesTab;

    QString m_openedFileName;
};

#endif // EVENTPATTERNSEDITDIALOG_H
