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
    void EventSelected(Event event);

private:
    void OnEventSelection(const Event& event) override;
};



#endif // EVENTSGRAPHICSSCENE_H
