#ifndef EVENTSTREEWIDGET_H
#define EVENTSTREEWIDGET_H

#include <QTreeWidget>

class EventsTreeWidget : public QTreeWidget
{
public:
    EventsTreeWidget();

private:
    void dropEvent(QDropEvent* event) override;
};

#endif // EVENTSTREEWIDGET_H
