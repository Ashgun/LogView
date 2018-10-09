#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QGraphicsRectItem>

class CustomItem : public QGraphicsRectItem
{
//    Q_OBJECT
public:
    CustomItem(qreal x, qreal y, qreal w, qreal h);

//    QRectF boundingRect() const;
    int type() const override;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    qreal m_x;
    qreal m_y;
    qreal m_w;
    qreal m_h;
    bool m_pressed;
};

#endif // CUSTOMITEM_H
