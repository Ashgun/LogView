#ifndef LOGVIEWMAINWINDOW_H
#define LOGVIEWMAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenuBar>
#include <QAction>
#include <QTreeWidget>

#include "EventsGraphicsScene.h"

#include <vector>

class EventGraphicsItem;

class IEventGroupExtractor;
class IPositionedLinesStorage;

class LogViewMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LogViewMainWindow(QWidget *parent = nullptr);
    ~LogViewMainWindow() override;

    void LoadLog(const QString& filename, const QString& headerParsingConfigJson, const QString& eventsParsingConfigJson);
    void LoadLogView();

signals:

protected slots:
    void slot_EventSelectionChanged(const EventGraphicsItem* previous, const EventGraphicsItem* current);

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

    QTreeWidget* gui_selectedEventView;

    QAction* act_openFile;

    std::unique_ptr<IPositionedLinesStorage> m_linesStorage;
    std::list<EventGraphicsItem*> m_eventsToView;
    std::vector<std::vector<Event>> m_eventLevels;

    QString m_loadedFile;

    std::unique_ptr<IEventGroupExtractor> m_groupExtractor;
};

#endif // LOGVIEWMAINWINDOW_H
