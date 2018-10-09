#ifndef CUSTOMVIEW_H
#define CUSTOMVIEW_H

#include <QGraphicsView>

class CustomView : public QGraphicsView
{
//    Q_OBJECT
public:
    CustomView();

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // CUSTOMVIEW_H
