#include "EventsGraphicsView.h"

#include <QMouseEvent>
#include <QGraphicsScene>
#include <QScrollBar>

#include <QtDebug>

EventsGraphicsView::EventsGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setAlignment(Qt::AlignCenter);
//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(slot_verticalScroll_valueChanged(int)),
            Qt::DirectConnection);
}

void EventsGraphicsView::ScrollTo(const int value)
{
    bool oldState = blockSignals(true);
    verticalScrollBar()->setValue(value);
    blockSignals(oldState);
}

void EventsGraphicsView::slot_verticalScroll_valueChanged(int value)
{
    emit ViewScrolledTo(value);
}
