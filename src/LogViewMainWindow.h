#ifndef LOGVIEWMAINWINDOW_H
#define LOGVIEWMAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenuBar>
#include <QAction>

#include "EventsGraphicsScene.h"

#include <vector>

class IPositionedLinesStorage;

class LogViewMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LogViewMainWindow(QWidget *parent = nullptr);

    void LoadLog(const QString& filename);
    void LoadLogView(IPositionedLinesStorage& linesStorage, const std::vector<std::vector<Event>>& eventLevels);

signals:

protected slots:
    void slot_EventSelected(Event event);

    void slot_act_openFileTriggred();


private:
    void CreateActions();
    void CreateMenuBar();
    void CreateConnetions();

private:
    QMenuBar* gui_mainMenuBar;
    QGraphicsView* gui_EventsView;
    EventsGraphicsScene* gui_EventsViewScene;

    QAction* act_openFile;
};

#endif // LOGVIEWMAINWINDOW_H
