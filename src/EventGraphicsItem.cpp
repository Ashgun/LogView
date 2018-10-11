#include "EventGraphicsItem.h"

#include <QPainter>
#include <QtDebug>

namespace ItemViewParams
{

const int xSpace = 5;
const int ySpace = 7;

} // namespace ItemViewParams

EventGraphicsItem::EventGraphicsItem(const Event& event, qreal x, qreal y, qreal w, qreal h,
                                     IEventGraphicsItemSelectionCallback& selectionCallback) :
    QGraphicsRectItem (x, y - ItemViewParams::ySpace, w, h + ItemViewParams::ySpace),
    m_event(event),
    m_selectionCallback(selectionCallback)
{
    setRect(x, y - ItemViewParams::ySpace, w, h + ItemViewParams::ySpace);
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
    QColor color(m_event.ViewColor.R, m_event.ViewColor.G, m_event.ViewColor.B);
    const auto currentRect = rect();

    painter->setPen(QPen(Qt::black, 1));
//    painter->drawLine(-10, currentRect.y(), 5000, currentRect.y());

    painter->setBrush(QBrush(color));
    painter->drawRect(currentRect);

    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(Qt::white));
    const QRectF namePlaceRect(currentRect.x() + ItemViewParams::xSpace, currentRect.y() - ItemViewParams::ySpace,
                               currentRect.width() - 2 * ItemViewParams::xSpace, 2 * ItemViewParams::ySpace);
    painter->drawRect(namePlaceRect);

//    QColor invertedColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
//    painter->setPen(QPen(invertedColor, 1));
    QPointF pos = QPointF(namePlaceRect.x() + ItemViewParams::xSpace, namePlaceRect.y() + 1.5 * ItemViewParams::ySpace);
    painter->drawText(pos, m_event.Name);
}
