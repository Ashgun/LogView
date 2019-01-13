#ifndef EVENTPATTERNSEDITWIDGET_H
#define EVENTPATTERNSEDITWIDGET_H

#include <QWidget>

#include "EventPatternEditWidget.h"
#include "EventsTreeEditWidget.h"

#include "Events.h"

class EventPatternsEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EventPatternsEditWidget(const EventsTreeEditWidget::PatternAddingPolicy& addingPolicy, QWidget *parent = nullptr);

    void SetEventPatternsHierarchy(const std::vector<EventPatternsHierarchyNode>& eventPatternsHierarchy);
    std::vector<EventPatternsHierarchyNode> GetEventPatternsHierarchy() const;

    void AcceptState();

signals:

public slots:

private:
    EventsTreeEditWidget* gui_eventsTree;
    EventPatternEditWidget* gui_eventsEdit;
};

#endif // EVENTPATTERNSEDITWIDGET_H
