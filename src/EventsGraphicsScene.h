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

    void DrawEventItems(const std::vector<std::vector<Event>>& eventLevels);
    void UpdateViewportParams(const std::size_t linesCount, const int baseViewPortWidth);

signals:
    void EventSelectionChanged(const EventGraphicsItem* previous, const EventGraphicsItem* current);

private:
    void OnEventSelection(EventGraphicsItem* eventGraphicsItem) override;

private:
    EventGraphicsItem* m_selectedEventItem;

    std::list<EventGraphicsItem*> m_eventsToView;
    qreal m_previosGraphicsSceneWidth = 1.0;
};



#endif // EVENTSGRAPHICSSCENE_H
