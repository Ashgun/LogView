#include "LogViewMainWindow.h"

#include <QHBoxLayout>
#include <QScrollBar>

#include <QDebug>

#include "BaseLinePositionStorage.h"
#include "BasePositionedLinesStorage.h"
#include "CustomItem.h"
#include "EventGraphicsItem.h"
#include "Events.h"
#include "FilesIndexer.h"
#include "IPositionedLinesStorage.h"
#include "RegExpLogLineParser.h"

namespace ViewParams
{

const int BaseVerticalSkip = 15;
const int BaseEventHeight = 15;

const int VerticalSpace = 3;

const int BaseHorizontalSkip = 7;

} // namespace ViewParams

namespace
{

class EventGroupExtractor : public IEventGroupExtractor
{
public:
    explicit EventGroupExtractor(QVector<QPair<QString, QString>> const& headerRegExps, QString const& groupName) :
        m_groupName(groupName)
    {
        QString groupRegExp("");
        m_lineParser.reset(new RegExpLogLineParser(headerRegExps, groupRegExp));
    }

    QString GetGroupFromLine(const PositionedLine& line) const override
    {
        LogLineInfo info = m_lineParser->Parse(line.Line);
        return info.HeaderItems[m_groupName];
    }

private:
    std::unique_ptr<ILogLineParser> m_lineParser;
    QString const m_groupName;
};

std::list<EventGraphicsItem*> GenerateEventViewItems(
        const std::vector<std::vector<Event>>& eventLevels,
        const qreal viewSceneWidth,
        IEventGraphicsItemSelectionCallback& selectionCallback)
{
    std::list<EventGraphicsItem*> eventsToView;

    std::vector<std::vector<std::size_t>> eventGroupIndexes(eventLevels.size());
    std::vector<std::size_t> overlappedGroupsCountForLevel(eventLevels.size(), 0);
    std::size_t maxGroupsCount = 0;

    {
        std::vector<std::vector<std::set<QString>>> crossedEventGroups(eventLevels.size());

        for (std::size_t level = 0; level < eventLevels.size(); ++level)
        {
            crossedEventGroups[level].resize(eventLevels[level].size());
            for (std::size_t i = 0; i < eventLevels[level].size(); ++i)
            {
                for (std::size_t j = 0; j < eventLevels[level].size(); ++j)
                {
                    if (IsEventsOverlapped(eventLevels[level][i], eventLevels[level][j]))
                    {
                        crossedEventGroups[level][i].insert(eventLevels[level][j].Group);
                    }
                }
            }

            eventGroupIndexes[level].resize(eventLevels[level].size());
            for (std::size_t i = 0; i < eventLevels[level].size(); ++i)
            {
                if (overlappedGroupsCountForLevel[level] < crossedEventGroups[level][i].size())
                {
                    overlappedGroupsCountForLevel[level] = crossedEventGroups[level][i].size();

                    if (maxGroupsCount < overlappedGroupsCountForLevel[level])
                    {
                        maxGroupsCount = overlappedGroupsCountForLevel[level];
                    }
                }

                std::size_t index = 0;
                for (const auto& group : crossedEventGroups[level][i])
                {
                    if (eventLevels[level][i].Group == group)
                    {
                        eventGroupIndexes[level][i] = index;
                    }
                    ++index;
                }
            }
        }

        crossedEventGroups.size();
    }

    for (std::size_t level = 0; level < eventLevels.size(); ++level)
    {
        for (std::size_t i = 0; i < eventLevels[level].size(); ++i)
        {
            const std::size_t groupIndex = eventGroupIndexes[level][i];

            const qreal y = eventLevels[level][i].StartLine.Position.Number * ViewParams::BaseEventHeight + ViewParams::BaseVerticalSkip;
            const qreal height =
                    (eventLevels[level][i].EndLine.Position.Number -
                     eventLevels[level][i].StartLine.Position.Number) * ViewParams::BaseEventHeight +
                    ViewParams::BaseEventHeight - ViewParams::VerticalSpace;

            const qreal groupViewWidth = viewSceneWidth / overlappedGroupsCountForLevel[level];
            const qreal shiftX = ViewParams::BaseHorizontalSkip + eventLevels[level][i].Level * ViewParams::BaseHorizontalSkip;
            const qreal x = shiftX + groupViewWidth * groupIndex;
            const qreal width = groupViewWidth - 2 * shiftX;

            eventsToView.push_back(new EventGraphicsItem(
                                       eventLevels[level][i],
                                       x, y, width, height,
                                       selectionCallback));
        }
    }

    return eventsToView;
}

} // namespace

LogViewMainWindow::LogViewMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    qRegisterMetaType<Event>("Event");
    setMinimumSize(1024, 768);

    gui_EventsViewScene = new EventsGraphicsScene(this);

    gui_EventsView = new QGraphicsView();
    gui_EventsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    gui_EventsView->setScene(gui_EventsViewScene);

//    QHBoxLayout* layout = new QHBoxLayout();
//    layout->addWidget(gui_EventsView);
//    setLayout(layout);

//    layout()->addWidget(gui_EventsView);

    setCentralWidget(gui_EventsView);


    CreateActions();
    CreateMenuBar();
    CreateConnetions();
}

LogViewMainWindow::~LogViewMainWindow() = default;

void LogViewMainWindow::LoadLog(const QString& filename)
{
    BaseLinePositionStorage linePositionStorage;
    m_linesStorage = std::make_unique<BasePositionedLinesStorage>();

    EventPatternsHierarchyMatcher lineSelector;
    lineSelector.EventPatterns.AddEventPattern(
        CreateExtendedEventPattern("Service works",
            EventPattern::CreateStringPattern("Logging started"),
            EventPattern::CreateStringPattern("Logging finished"),
            CreateColor(128, 128, 128)));
    lineSelector.EventPatterns.TopLevelNodes.back().AddSubEventPattern(
        CreateSingleEventPattern("Accounts list obtained",
            EventPattern::CreateStringPattern("[AccountRegistry] New accounts list obtained"),
            CreateColor(128, 128, 0)));
    lineSelector.EventPatterns.TopLevelNodes.back().AddSubEventPattern(
                CreateExtendedEventPattern("Tenant backup",
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session started"),
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session completed successfully"),
                    EventPattern::CreateStringPattern("[TenantBackupProcessor] Session completed with errors"),
                    CreateColor(0, 128, 0), CreateColor(128, 0, 0)));
    lineSelector.EventPatterns.TopLevelNodes.back().SubEvents.back().AddSubEventPattern(
                CreateExtendedEventPattern("Mailbox backup",
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was started"),
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was finished"),
                    EventPattern::CreateRegExpPattern("\\[UserBackupProcessor\\] Session #[0-9]+ was failed"),
                    CreateColor(0, 255, 0), CreateColor(255, 0, 0)));

    FilesIndexer indexer(linePositionStorage, *m_linesStorage, lineSelector);
    indexer.AddFileIndexes(filename);

    QVector<QPair<QString, QString>> headerRegExps;
    headerRegExps.push_back(QPair<QString, QString>("DateTime", "\\[([0-9_\\./\\-\\s:]+)\\]\\s*"));
    headerRegExps.push_back(QPair<QString, QString>("LogLevel", "\\[([TILDWEF]){1,1}\\]\\s*"));
    headerRegExps.push_back(QPair<QString, QString>("ThreadId", "\\[([x0-9]+)\\]\\s*"));

    EventGroupExtractor eventGroupExtractor(headerRegExps, "ThreadId");

    m_eventLevels = FindEvents(
        lineSelector.EventPatterns, *m_linesStorage, eventGroupExtractor);

    Invalidate();
}

void LogViewMainWindow::UpdateViewportParams()
{
    if (m_linesStorage == nullptr)
    {
        return;
    }

    std::size_t linesCount = m_linesStorage->Size();
    gui_EventsViewScene->setSceneRect(
                0, 0,
                gui_EventsView->width() - 15,
                (linesCount + 1) * (ViewParams::BaseEventHeight/* + ViewParams::VerticalSpace*/) +
                2 * ViewParams::BaseVerticalSkip);
}

void LogViewMainWindow::LoadLogView()
{
    Invalidate();
}

void LogViewMainWindow::slot_EventSelected(Event event)
{
    qDebug() << "*** Event item clicked:" << event.Name;

    if (event.Type == EventType::Single)
    {
        qDebug() << event.StartLine.Line;
    }
    else
    {
        qDebug() << event.StartLine.Line;
        qDebug() << event.EndLine.Line;
    }
}

void LogViewMainWindow::slot_act_openFileTriggred()
{
    LoadLog("log1.log");
}

void LogViewMainWindow::Invalidate()
{
    UpdateViewportParams();
    Redraw();
}

void LogViewMainWindow::resizeEvent(QResizeEvent* /*event*/)
{
    Invalidate();
}

void LogViewMainWindow::CreateActions()
{
    act_openFile = new QAction("&Open log file");
}

void LogViewMainWindow::CreateMenuBar()
{
    gui_mainMenuBar = new QMenuBar;

    QMenu* fileMenu = new QMenu("&File");
    fileMenu->addAction(act_openFile);

    gui_mainMenuBar->addMenu(fileMenu);

    setMenuBar(gui_mainMenuBar);
}

void LogViewMainWindow::CreateConnetions()
{
    connect(gui_EventsViewScene, SIGNAL(EventSelected(Event)),
            this, SLOT(slot_EventSelected(Event)),
            Qt::QueuedConnection);

    connect(act_openFile, SIGNAL(triggered()),
            this, SLOT(slot_act_openFileTriggred()), Qt::DirectConnection);
}

void LogViewMainWindow::Redraw()
{
    m_eventsToView = GenerateEventViewItems(m_eventLevels, gui_EventsViewScene->width(), *gui_EventsViewScene);
    gui_EventsViewScene->clear();
    for (auto& eventViewItem : m_eventsToView)
    {
        gui_EventsViewScene->addItem(eventViewItem);
    }

    gui_EventsView->horizontalScrollBar()->setValue(gui_EventsView->horizontalScrollBar()->minimum());
    gui_EventsView->verticalScrollBar()->setValue(gui_EventsView->verticalScrollBar()->minimum());
    gui_EventsView->centerOn(0, 0);
}
