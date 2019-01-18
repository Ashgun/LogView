#ifndef EVENTSGRAPHICSVIEW_H
#define EVENTSGRAPHICSVIEW_H

#include <QGraphicsView>

#include <QVector>

class EventsGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit EventsGraphicsView(QWidget* parent = nullptr);

    void ScrollTo(const int value);
    void ShowReferenceLine(const int verticalPos);

private slots:
    void slot_verticalScroll_valueChanged(int value);

    void mouseMoveEvent(QMouseEvent* event) override;

signals:
    void ViewScrolledTo(int);
    void LineViewed(int);

private:
    QVector<QGraphicsLineItem*> gui_viewedLines;
};

#endif // EVENTSGRAPHICSVIEW_H
