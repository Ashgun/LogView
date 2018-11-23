#ifndef LOGVIEWMAINWINDOW_H
#define LOGVIEWMAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenuBar>
#include <QAction>
#include <QTreeWidget>
#include <QShortcut>

#include "EventsGraphicsScene.h"

#include <vector>

class EventGraphicsItem;

class IEventInfoExtractor;
class IPositionedLinesStorage;

class LogViewMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LogViewMainWindow(QWidget *parent = nullptr);
    ~LogViewMainWindow() override;

    void LoadLogs(const QStringList& filenames, const QString& eventsParsingConfigJson);
    void LoadLogView();

signals:

protected slots:
    void slot_EventSelectionChanged();
    void slot_act_openFileTriggred();
    void slot_act_copySelectedLinesToClipboard_Triggred();
    void slot_act_editEventPatternsConfig_Triggred();
    void slot_act_editLogLineParsingConfig_Triggred();

    void resizeEvent(QResizeEvent* event) override;

private:
    void CreateActions();
    void CreateMenuBar();
    void CreateConnections();

    void Redraw();
    void UpdateViewportParams();
    void Invalidate();

private:
    QMenuBar* gui_mainMenuBar;
    QGraphicsView* gui_EventsView;
    EventsGraphicsScene* gui_EventsViewScene;

    QTreeWidget* gui_selectedEventView;

    QAction* act_openFile;
    QAction* act_copySelectedLinesToClipboard;
    QShortcut* shortcut_copySelectedLinesToClipboard;
    QAction* act_editEventPatternsConfig;
    QAction* act_editLogLineParsingConfig;

    std::unique_ptr<IPositionedLinesStorage> m_linesStorage;
    std::vector<std::vector<Event>> m_eventLevels;

    QStringList m_loadedFiles;

    std::unique_ptr<IEventInfoExtractor> m_infoExtractor;

    LogLineHeaderParsingParams m_logLineHeaderParsingParams;
};

#endif // LOGVIEWMAINWINDOW_H
