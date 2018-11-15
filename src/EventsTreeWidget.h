#ifndef EVENTSTREEWIDGET_H
#define EVENTSTREEWIDGET_H

#include <QTreeWidget>

class EventsTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    EventsTreeWidget();

    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void clicked();

private:
    void dropEvent(QDropEvent* event) override;
};

#endif // EVENTSTREEWIDGET_H
