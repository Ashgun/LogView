#ifndef EVENTGRAPHICSITEM_H
#define EVENTGRAPHICSITEM_H

#include <QGraphicsRectItem>

#include "Events.h"

#include <functional>

class IEventGraphicsItemSelectionCallback
{
public:
    virtual void OnEventSelection(const Event& event) = 0;

    virtual ~IEventGraphicsItemSelectionCallback() = default;
};

class EventGraphicsItem : public QGraphicsRectItem
{
public:
    explicit EventGraphicsItem(const Event& event, qreal x, qreal y, qreal w, qreal h,
                               IEventGraphicsItemSelectionCallback& selectionCallback);

    int type() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    const Event m_event;
    IEventGraphicsItemSelectionCallback& m_selectionCallback;
};

#endif // EVENTGRAPHICSITEM_H
