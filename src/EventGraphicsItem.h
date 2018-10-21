#ifndef EVENTGRAPHICSITEM_H
#define EVENTGRAPHICSITEM_H

#include <QGraphicsRectItem>

#include "Events.h"

#include <functional>

class EventGraphicsItem;

class IEventGraphicsItemSelectionCallback
{
public:
    virtual void OnEventSelection(EventGraphicsItem* eventGraphicsItem) = 0;

    virtual ~IEventGraphicsItemSelectionCallback() = default;
};

class EventGraphicsItem : public QGraphicsRectItem
{
public:
    explicit EventGraphicsItem(const Event& event, qreal x, qreal y, qreal w, qreal h,
                               IEventGraphicsItemSelectionCallback& selectionCallback);

    void ScaleHorizontally(qreal horizontalScale);

    int type() const override;

    void Select();
    void Unselect();

    const Event& GetEvent() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
//    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    const Event m_event;
    IEventGraphicsItemSelectionCallback& m_selectionCallback;

    bool m_selected;
    qreal m_horizontalScale;
};

#endif // EVENTGRAPHICSITEM_H
