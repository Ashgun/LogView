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
    m_selectionCallback(selectionCallback),
    m_selected(false),
    m_horizontalScale(1.0)
{
    setRect(x * m_horizontalScale, y - ItemViewParams::ySpace, w * m_horizontalScale, h + ItemViewParams::ySpace);
    setZValue(m_event.Level);
}

void EventGraphicsItem::ScaleHorizontally(qreal horizontalScale)
{
    m_horizontalScale = horizontalScale;
    const QRectF currentrect = rect();
    setRect(currentrect.x() * m_horizontalScale, currentrect.y(),
            currentrect.width() * m_horizontalScale, currentrect.height());
    update();
}

int EventGraphicsItem::type() const
{
    return  UserType + 2;
}

void EventGraphicsItem::Select()
{
    m_selected = true;
    update();
}

void EventGraphicsItem::Unselect()
{
    m_selected = false;
    update();
}

const Event&EventGraphicsItem::GetEvent() const
{
    return m_event;
}

void EventGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* /*event*/)
{
    m_selectionCallback.OnEventSelection(this);
}

void EventGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    QColor color = m_event.ViewColor.toQColor();
    const auto currentRect = rect();

//    painter->setPen(QPen(Qt::black, 1));
//    painter->drawLine(-10, currentRect.y(), 5000, currentRect.y());

    if (m_selected)
    {
        QColor invertedColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
        painter->setPen(QPen(invertedColor, 5));
    }
    else
    {
        painter->setPen(QPen(Qt::black, 1));
    }

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
