#ifndef EVENTSGRAPHICSVIEW_H
#define EVENTSGRAPHICSVIEW_H

#include <QGraphicsView>

class EventsGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit EventsGraphicsView(QWidget* parent = nullptr);

    void ScrollTo(const int value);

private slots:
    void slot_verticalScroll_valueChanged(int value);

signals:
    void ViewScrolledTo(int);
};

#endif // EVENTSGRAPHICSVIEW_H
