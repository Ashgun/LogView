#ifndef LOGVIEWMAINWINDOW_H
#define LOGVIEWMAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "EventsGraphicsScene.h"

#include <vector>

class LogViewMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LogViewMainWindow(const std::vector<std::vector<Event>>& eventLevels, QWidget *parent = nullptr);

signals:

public slots:
    void slot_EventSelected(Event event);

private:
    QGraphicsView* gui_EventsView;
    EventsGraphicsScene* gui_EventsViewScene;
};

#endif // LOGVIEWMAINWINDOW_H
