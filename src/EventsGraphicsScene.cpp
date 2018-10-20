#include "EventsGraphicsScene.h"

EventsGraphicsScene::EventsGraphicsScene(QObject* parent) :
    QGraphicsScene(parent),
    m_selectedEventItem(nullptr)
{
}

void EventsGraphicsScene::OnEventSelection(EventGraphicsItem* eventGraphicsItem)
{
    EventGraphicsItem* prev = m_selectedEventItem;
    m_selectedEventItem = eventGraphicsItem;

    if (prev != nullptr)
    {
        prev->Unselect();
    }

    m_selectedEventItem->Select();

    emit EventSelectionChanged(prev, m_selectedEventItem);
}
