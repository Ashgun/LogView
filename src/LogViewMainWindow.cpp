#include "LogViewMainWindow.h"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QSplitter>

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
    explicit EventGroupExtractor(LogLineHeaderParsingParams const& logLineHeaderParsingParams) :
        m_groupName(logLineHeaderParsingParams.GroupNameForGrouping)
    {
        QString groupRegExp("");
        m_lineParser.reset(new RegExpLogLineParser(logLineHeaderParsingParams.HeaderGroupRegExps, groupRegExp));
    }

    QString GetGroupFromLine(const EventPattern::PatternString& line) const override
    {
        LogLineInfo info = m_lineParser->Parse(line);
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
    if (eventLevels.empty())
    {
        return std::list<EventGraphicsItem*>();
    }

    std::list<EventGraphicsItem*> eventsToView;

    std::vector<std::vector<std::size_t>> eventGroupIndexes(eventLevels.size());
    std::vector<std::size_t> overlappedGroupsCountForLevel(eventLevels.size(), 0);
    std::size_t maxGroupsCount = 0;

    {
        std::vector<std::vector<std::set<QString>>> crossedEventGroups(eventLevels.size());

        for (std::size_t level = 0; level < 2 && level < eventLevels.size(); ++level)
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

        for (std::size_t level = 2; level < eventLevels.size(); ++level)
        {
            eventGroupIndexes[level].resize(eventLevels[level].size());
            crossedEventGroups[level].resize(eventLevels[level].size());
            overlappedGroupsCountForLevel[level] = overlappedGroupsCountForLevel[1];
            for (std::size_t i = 0; i < eventLevels[level].size(); ++i)
            {
                for (std::size_t j = 0; j < eventLevels[1].size(); ++j)
                {
                    if (IsEventsOverlapped(eventLevels[level][i], eventLevels[1][j]) &&
                        eventLevels[level][i].Group == eventLevels[1][j].Group)
                    {
                        eventGroupIndexes[level][i] = eventGroupIndexes[1][j];
                    }
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

            const qreal y = eventLevels[level][i].StartLine.Position.NumberInMatchedLines * ViewParams::BaseEventHeight + ViewParams::BaseVerticalSkip;
            const qreal height =
                    (eventLevels[level][i].EndLine.Position.NumberInMatchedLines -
                     eventLevels[level][i].StartLine.Position.NumberInMatchedLines) * ViewParams::BaseEventHeight +
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

QStringList LoadFileBlockToStrings(const QString& filename, const qint64 from, const qint64 to)
{
    QStringList resultList;

    QFile binfile(filename);

    std::size_t const bufferSize = to - from;
    std::vector<char> buffer(bufferSize + 1);
    char* bufferData = buffer.data();

    const auto IsEndOfLineSymbol =
            [](const char& ch) -> bool
            {
                return ((ch == '\n') || (ch == '\r'));
            };

    if (binfile.open(QIODevice::ReadOnly))
    {
        binfile.seek(from);

        QDataStream in(&binfile);

        std::size_t const readBytesCount = in.readRawData(bufferData, bufferSize);

        std::size_t previousEol = 0;
        std::size_t currentEol = 0;
        for (std::size_t i = 0; i < readBytesCount; ++i)
        {
            if (IsEndOfLineSymbol(bufferData[i]))
            {
                currentEol = i;
                while (IsEndOfLineSymbol(bufferData[i]) && i < readBytesCount) { ++i; }

                QString const line = QString::fromStdString(
                            std::string(bufferData + previousEol, bufferData + currentEol));
                resultList.append(line);

                previousEol = i;
            }
        }
    }

    return resultList;
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

    gui_selectedEventView = new QTreeWidget();
//    gui_selectedEventView->setColumnCount(2);
    gui_selectedEventView->setHeaderLabels(QStringList() << tr("No.") << tr("Log line"));
//    gui_selectedEventView->setStyleSheet("QTreeWidget { font-family: \"Helvetica\"; font-size: " + QString::number(11) + "pt; }");

    setStyleSheet("QSplitter::handle{background-color: black;}");
    QSplitter* verticalSplitter = new QSplitter(Qt::Orientation::Vertical);
    verticalSplitter->setHandleWidth(3);

    QWidget* viewWidget = new QWidget();
    {
        QHBoxLayout* viewLayout = new QHBoxLayout();
        viewLayout->addWidget(gui_EventsView);

        viewWidget->setLayout(viewLayout);
    }

    verticalSplitter->addWidget(viewWidget);

    QWidget* selectedViewWidget = new QWidget();
    {
        QVBoxLayout* viewLayout = new QVBoxLayout();
        viewLayout->addWidget(new QLabel(tr("Selected event:")));
        viewLayout->addWidget(gui_selectedEventView);

        selectedViewWidget->setLayout(viewLayout);
    }
    verticalSplitter->addWidget(selectedViewWidget);
    verticalSplitter->setCollapsible(1, true);

    setCentralWidget(verticalSplitter);
//    QList<int> sizes;
//    sizes.append(height() * 3/4);
//    sizes.append(height() - height() * 3/4);
//    verticalSplitter->setSizes(sizes);
    verticalSplitter->setStretchFactor(0, 5);
    verticalSplitter->setStretchFactor(1, 1);


    CreateActions();
    CreateMenuBar();
    CreateConnetions();
}

LogViewMainWindow::~LogViewMainWindow() = default;
#include <iostream>
void LogViewMainWindow::LoadLog(
        const QString& filename, const QString& headerParsingConfigJson,
        const QString& eventsParsingConfigJson)
{
    m_loadedFile = filename;

    BaseLinePositionStorage linePositionStorage;
    m_linesStorage = std::make_unique<BasePositionedLinesStorage>();

    EventPatternsHierarchyMatcher lineSelector;
    EventPatternsHierarchy::fromJson(eventsParsingConfigJson, lineSelector.EventPatterns);

    FilesIndexer indexer(linePositionStorage, *m_linesStorage, lineSelector);
    indexer.AddFileIndexes(filename);

    const LogLineHeaderParsingParams logLineHeaderParsingParams = LogLineHeaderParsingParams::FromJson(headerParsingConfigJson);
    m_groupExtractor = std::make_unique<EventGroupExtractor>(logLineHeaderParsingParams);

    m_eventLevels = FindEvents(lineSelector.EventPatterns, *m_linesStorage, *m_groupExtractor);

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

void LogViewMainWindow::slot_EventSelectionChanged(const EventGraphicsItem* /*previous*/, const EventGraphicsItem* current)
{
    gui_selectedEventView->clear();

    if (current->GetEvent().Type == EventType::Single)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(
                                    QStringList()
                                        << QString::number(current->GetEvent().StartLine.Position.NumberInFile)
                                        << current->GetEvent().StartLine.Line);
        item->setBackgroundColor(1, current->GetEvent().ViewColor.toQColor());
        gui_selectedEventView->addTopLevelItem(item);
    }
    else
    {
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(
                                        QStringList()
                                            << QString::number(current->GetEvent().StartLine.Position.NumberInFile)
                                            << current->GetEvent().StartLine.Line);
            item->setBackgroundColor(1, current->GetEvent().ViewColor.toQColor());
            gui_selectedEventView->addTopLevelItem(item);
        }

        QStringList lines = LoadFileBlockToStrings(
                                m_loadedFile,
                                current->GetEvent().StartLine.Position.Offset, current->GetEvent().EndLine.Position.Offset);

        for (int i = 1; i < lines.size(); ++i)
        {
            if (m_groupExtractor->GetGroupFromLine(lines[i]) != current->GetEvent().Group)
            {
                continue;
            }

            QTreeWidgetItem* item = new QTreeWidgetItem(
                                        QStringList()
                                            << QString::number(current->GetEvent().StartLine.Position.NumberInFile + i)
                                            << lines[i]);
            gui_selectedEventView->addTopLevelItem(item);
        }

        {
            QTreeWidgetItem* item = new QTreeWidgetItem(
                                        QStringList()
                                            << QString::number(current->GetEvent().EndLine.Position.NumberInFile)
                                            << current->GetEvent().EndLine.Line);
            item->setBackgroundColor(1, current->GetEvent().ViewColor.toQColor());
            gui_selectedEventView->addTopLevelItem(item);
        }
    }
}

QString LoadFileToQString(const QString& filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        return QString();
    }

    QTextStream in(&f);
    const QString data = in.readAll();

    return data;
}

void LogViewMainWindow::slot_act_openFileTriggred()
{
    const QString headerParsingConfigJson = LoadFileToQString("HeaderParsingConfig.json");
    const QString eventsParsingConfigJson = LoadFileToQString("BackupServiceParseConfig.json");

    LoadLog("log1.log", headerParsingConfigJson, eventsParsingConfigJson);
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
    connect(gui_EventsViewScene, SIGNAL(EventSelectionChanged(const EventGraphicsItem*, const EventGraphicsItem*)),
            this, SLOT(slot_EventSelectionChanged(const EventGraphicsItem*, const EventGraphicsItem*)),
            Qt::QueuedConnection);

    connect(act_openFile, SIGNAL(triggered()),
            this, SLOT(slot_act_openFileTriggred()), Qt::DirectConnection);
}

void LogViewMainWindow::Redraw()
{
    if (m_eventsToView.empty())
    {
        gui_EventsViewScene->clear();
        m_eventsToView = GenerateEventViewItems(m_eventLevels, gui_EventsViewScene->width(), *gui_EventsViewScene);
        for (auto& eventViewItem : m_eventsToView)
        {
            gui_EventsViewScene->addItem(eventViewItem);
        }

        m_previosGraphicsSceneWidth = gui_EventsViewScene->width();
    }
    else
    {
        for (auto& eventViewItem : m_eventsToView)
        {
            eventViewItem->ScaleHorizontally(gui_EventsViewScene->width() / m_previosGraphicsSceneWidth);
        }

        m_previosGraphicsSceneWidth = gui_EventsViewScene->width();
    }

    gui_EventsView->horizontalScrollBar()->setValue(gui_EventsView->horizontalScrollBar()->minimum());
    gui_EventsView->verticalScrollBar()->setValue(gui_EventsView->verticalScrollBar()->minimum());
    gui_EventsView->centerOn(0, 0);
}
