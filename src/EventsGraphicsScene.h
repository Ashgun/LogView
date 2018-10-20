#ifndef EVENTSGRAPHICSSCENE_H
#define EVENTSGRAPHICSSCENE_H

#include <QObject>
#include <QGraphicsScene>

#include "EventGraphicsItem.h"

class EventsGraphicsScene : public QGraphicsScene, public IEventGraphicsItemSelectionCallback
{
    Q_OBJECT
public:
    explicit EventsGraphicsScene(QObject *parent = nullptr);

signals:
    void EventSelectionChanged(const EventGraphicsItem* previous, const EventGraphicsItem* current);

private:
    void OnEventSelection(EventGraphicsItem* eventGraphicsItem) override;

private:
    EventGraphicsItem* m_selectedEventItem;
};



#endif // EVENTSGRAPHICSSCENE_H
