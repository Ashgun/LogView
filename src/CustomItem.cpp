#include "CustomItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QPainter>
#include <QtDebug>

CustomItem::CustomItem(qreal x, qreal y, qreal w, qreal h) : QGraphicsRectItem (x, y, w, h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_pressed = false;
    setFlag(ItemIsSelectable);
}

//QRectF CustomItem::boundingRect() const
//{
//    return QRectF(m_x, m_y, m_w, m_h);
//}

int CustomItem::type() const
{
    return UserType + 1;
}

void CustomItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    qDebug() << "Custom item clicked.";
    m_pressed = true;
    update();
    QGraphicsItem::mousePressEvent(event);
}

void CustomItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    m_pressed = false;
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void CustomItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    QRectF rec = boundingRect();
    QBrush brush(Qt::blue);

    if(m_pressed)
    {
        brush.setColor(Qt::red);
    }
    else
    {
        brush.setColor(Qt::green);
    }

    painter->fillRect(rec,brush);
    painter->drawRect(rec);

}
