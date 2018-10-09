#include "CustomView.h"

#include <QMouseEvent>
#include <QGraphicsScene>

#include <QtDebug>

CustomView::CustomView()
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setAlignment(Qt::AlignCenter);
//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void CustomView::mousePressEvent(QMouseEvent* event)
{
    auto point = mapToScene(event->pos());

    qDebug() << "Custom view clicked:" << scene()->items(point).size();
    QGraphicsView::mousePressEvent(event);
}
