#include "EventsGraphicsView.h"

#include <QMouseEvent>
#include <QGraphicsScene>
#include <QScrollBar>
#include <QGraphicsLineItem>

#include <QtDebug>

EventsGraphicsView::EventsGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setMouseTracking(true);

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

void EventsGraphicsView::ShowReferenceLine(const int verticalPos)
{
    if (!gui_viewedLines.isEmpty())
    {
        for (QGraphicsLineItem* item : gui_viewedLines)
        {
            delete item;
        }

        gui_viewedLines.clear();
    }

    gui_viewedLines.push_back(scene()->addLine(0, verticalPos, width(), verticalPos, QPen(QBrush(Qt::darkMagenta), 3)));
    gui_viewedLines.back()->setZValue(1000);

    gui_viewedLines.push_back(scene()->addLine(0, verticalPos, width(), verticalPos, QPen(QBrush(Qt::magenta), 1)));
    gui_viewedLines.back()->setZValue(1001);
}

void EventsGraphicsView::slot_verticalScroll_valueChanged(int value)
{
    emit ViewScrolledTo(value);
}

void EventsGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    const int verticalPos = event->pos().y();

    ShowReferenceLine(verticalPos);

    emit LineViewed(verticalPos);
}
