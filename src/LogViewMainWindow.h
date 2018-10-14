#ifndef LOGVIEWMAINWINDOW_H
#define LOGVIEWMAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenuBar>
#include <QAction>

#include "EventsGraphicsScene.h"

#include <vector>

class EventGraphicsItem;

class IPositionedLinesStorage;

class LogViewMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LogViewMainWindow(QWidget *parent = nullptr);
    ~LogViewMainWindow() override;

    void LoadLog(const QString& filename);
    void LoadLogView();

signals:

protected slots:
    void slot_EventSelected(Event event);

    void slot_act_openFileTriggred();

    void resizeEvent(QResizeEvent* event);


private:
    void CreateActions();
    void CreateMenuBar();
    void CreateConnetions();

    void Redraw();
    void UpdateViewportParams();
    void Invalidate();

private:
    QMenuBar* gui_mainMenuBar;
    QGraphicsView* gui_EventsView;
    EventsGraphicsScene* gui_EventsViewScene;

    QAction* act_openFile;

    std::unique_ptr<IPositionedLinesStorage> m_linesStorage;
    std::list<EventGraphicsItem*> m_eventsToView;
    std::vector<std::vector<Event>> m_eventLevels;
};

#endif // LOGVIEWMAINWINDOW_H
