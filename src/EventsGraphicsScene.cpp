#include "EventsGraphicsScene.h"

EventsGraphicsScene::EventsGraphicsScene(QObject* parent) :
    QGraphicsScene(parent)
{
}

void EventsGraphicsScene::OnEventSelection(const Event& event)
{
    emit EventSelected(event);
}
