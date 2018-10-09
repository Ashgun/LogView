#include "EventGraphicsItem.h"

#include <QPainter>
#include <QtDebug>

EventGraphicsItem::EventGraphicsItem(const Event& event, qreal x, qreal y, qreal w, qreal h,
                                     IEventGraphicsItemSelectionCallback& selectionCallback) :
    QGraphicsRectItem (x, y, w, h),
    m_event(event),
    m_selectionCallback(selectionCallback)
{
    setRect(x, y, w, h);
    setZValue(m_event.Level);
}

int EventGraphicsItem::type() const
{
    return  UserType + 2;
}

void EventGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* /*event*/)
{
//    qDebug() << "Event item clicked:" << m_event.Name;
//    emit EventSelected(m_event);
    m_selectionCallback.OnEventSelection(m_event);
}

void EventGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    const int xSpace = 5;
    const int ySpace = 7;

    QColor color(Qt::green);
    const auto currentRect = rect();

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(color));
    painter->drawRect(currentRect);

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(Qt::white));
    const QRectF namePlaceRect(currentRect.x() + xSpace, currentRect.y() - ySpace,
                               currentRect.width() - 2 * xSpace, 2 * ySpace);
    painter->drawRect(namePlaceRect);

//    QColor invertedColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
//    painter->setPen(QPen(invertedColor, 1));
    QPointF pos = QPointF(namePlaceRect.x() + xSpace, namePlaceRect.y() + 1.5 * ySpace);
    painter->drawText(pos, m_event.Name);
}
