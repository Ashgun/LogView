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
#include "EventsGraphicsView.h"

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

    void LoadLogs(const QStringList& filenames, const int fileGroupsCount, const QString& eventsParsingConfigJson);
    void LoadLogView();

signals:

protected slots:
    void slot_EventSelectionChanged();
    void slot_ViewScrolledTo(int value);
    void slot_LineViewed(int hPos);
    void slot_act_openFileTriggred();
    void slot_act_openMultipleFilesTriggred();
    void slot_act_reloadFilesTriggred();
    void slot_act_closeFileTriggred();
    void slot_act_copySelectedLinesToClipboard_Triggred();
    void slot_act_editEventPatternsConfig_Triggred();
    void slot_act_editLogLineParsingConfig_Triggred();

    void slot_act_closeAppTriggred();

    void resizeEvent(QResizeEvent* event) override;

private:
    void CreateActions();
    void CreateMenuBar();
    void CreateConnections();

    void Redraw();
    void UpdateViewportParams();
    void Invalidate();

    void OpenFiles(const QVector<QStringList>& fileLists, const QStringList& configFiles);
    void CloseFiles();

    void AddView(const int fileGroupsCount);


private:
    QMenuBar* gui_mainMenuBar;
    QWidget* gui_viewsWidget;

    std::vector<EventsGraphicsView*> gui_EventsViews;
    std::vector<EventsGraphicsScene*> gui_EventsViewScenes;

    QTreeWidget* gui_selectedEventView;

    QAction* act_exit;

    QAction* act_openFile;
    QAction* act_openMultipleFiles;
    QAction* act_closeFile;
    QAction* act_reloadFiles;
    QAction* act_copySelectedLinesToClipboard;
    QShortcut* shortcut_copySelectedLinesToClipboard;
    QAction* act_editEventPatternsConfig;
    QAction* act_editLogLineParsingConfig;

    std::vector<std::unique_ptr<IPositionedLinesStorage>> m_linesStorages;
    std::vector<std::vector<std::vector<Event>>> m_eventLevels;

    std::vector<QStringList> m_loadedFiles;

    std::unique_ptr<IEventInfoExtractor> m_infoExtractor;

    LogLineHeaderParsingParams m_logLineHeaderParsingParams;

    QVector<QStringList> m_actualFileLists;
    QStringList m_actualConfigFiles;
};

#endif // LOGVIEWMAINWINDOW_H
