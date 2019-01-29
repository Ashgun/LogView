#include "LogViewMainWindow.h"

#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QFileInfo>
#include <QDateTime>

#include <QDebug>

#include "BaseLinePositionStorage.h"
#include "BasePositionedLinesStorage.h"
#include "CustomItem.h"
#include "Events.h"
#include "FilesIndexer.h"
#include "IPositionedLinesStorage.h"
#include "RegExpLogLineParser.h"

#include "EventPatternsEditDialog.h"
#include "LogLineHeaderParsingParamsEditDialog.h"
#include "LogFileWithConfigsOpenDialog.h"
#include "LogFilesSelectionDialog.h"

#include "Common.h"
#include "Utils.h"

#include <stdexcept>

namespace
{

class EventInfoExtractor : public IEventInfoExtractor
{
public:
    explicit EventInfoExtractor(LogLineHeaderParsingParams const& logLineHeaderParsingParams) :
        m_groupName(logLineHeaderParsingParams.GroupNameForGrouping)
    {
        QString groupRegExp("");
        m_lineParser.reset(new RegExpLogLineParser(logLineHeaderParsingParams.HeaderGroupDatas, groupRegExp));
    }

    QString GetGroupFromLine(const EventPattern::PatternString& line) const override
    {
        LogLineInfo info = m_lineParser->Parse(line);
        return info.HeaderItems[m_groupName];
    }

    QString GetMessageFromLine(const EventPattern::PatternString& line) const override
    {
        LogLineInfo info = m_lineParser->Parse(line);
        return info.Message;
    }

private:
    std::unique_ptr<ILogLineParser> m_lineParser;
    QString const m_groupName;
};

QPair<QStringList, QList<int>> LoadFileBlockToStrings(const QString& filename, const quint64 from, const quint64 to,
                                   std::function<bool(const QString& line)> isLineAcceptableFunctor)
{
    const auto IsEndOfLineSymbol =
            [](const char& ch) -> bool
            {
                return ((ch == '\n') || (ch == '\r'));
            };

    QStringList resultList;
    QList<int> resultLinesIndexes;

    QFile binfile(filename);

    std::size_t const bufferSize = 10 * 1024 * 1024;
    std::vector<char> buffer(bufferSize + 1);
    char* bufferData = buffer.data();
    if (binfile.open(QIODevice::ReadOnly))
    {
        if (from > 0)
        {
            binfile.seek(static_cast<qint64>(from));
        }

        int lineIndex = 0;
        QDataStream in(&binfile);
        quint64 bufferStartOffset = from;

        while (!in.atEnd())
        {
            quint64 const readBytesCount = static_cast<quint64>(in.readRawData(bufferData, bufferSize));

            quint64 previousEol = 0;
            quint64 currentEol = 0;
            for (quint64 i = 0; i < readBytesCount; ++i)
            {
                if (IsEndOfLineSymbol(bufferData[i]))
                {
                    currentEol = i;
                    ++i;
                    while (IsEndOfLineSymbol(bufferData[i]) && i < readBytesCount) { ++i; }

                    EventPattern::PatternString const line =
                            EventPattern::PatternString::fromStdString(
                                std::string(bufferData + previousEol, bufferData + currentEol));

                    if (bufferStartOffset + currentEol < to || to == 0)
                    {
                        if (isLineAcceptableFunctor(line))
                        {
                            resultList.append(line);
                            resultLinesIndexes.append(lineIndex);
                        }

                        ++lineIndex;
                    }

                    previousEol = i;
                }
            }

            if (bufferStartOffset + currentEol >= to && to > 0)
            {
                break;
            }

            bufferStartOffset += readBytesCount;
        }

        binfile.close();
    }

    return QPair<QStringList, QList<int>>(resultList, resultLinesIndexes);
}

LogLineHeaderParsingParams LoadLogLineHeaderParsingParams()
{
    QSettings settings(Constants::AppConfig, QSettings::Format::IniFormat);

    if (settings.value(Constants::LogLinesHeaderParsingConfig).isNull())
    {
        throw std::runtime_error(
                    std::string("Field ") +
                    Constants::LogLinesHeaderParsingConfig.toStdString() +
                    " not found in " +
                    Constants::AppConfig.toStdString());
    }

    const QString headerParsingConfigJson = LoadFileToQString(settings.value(Constants::LogLinesHeaderParsingConfig).toString());
    return LogLineHeaderParsingParams::FromJson(headerParsingConfigJson);
}

struct LineForEventData
{
    QString PositionIndex;
    QString PositionNumberInFile;
    QString Line;
    QString ColorCode = IMatchableEventPattern::Color().toColorCode();
    std::size_t FilesGroupIndex = 0;
};

class LogLineDataComparator
{
public:
    explicit LogLineDataComparator(LogLineHeaderParsingParams const& logLineHeaderParsingParams) :
        m_sortingGroupName(logLineHeaderParsingParams.SortingGroup)
    {
        QString groupRegExp("");
        m_lineParser.reset(new RegExpLogLineParser(logLineHeaderParsingParams.HeaderGroupDatas, groupRegExp));
    }

    bool operator()(const LineForEventData &d1, const LineForEventData &d2) const
    {
        const auto l = GetSortingHeader(d1.Line);
        const auto r = GetSortingHeader(d2.Line);

        if (l == r)
        {
            if (d1.PositionIndex.toInt() != d2.PositionIndex.toInt())
            {
                return d1.PositionIndex.toInt() < d2.PositionIndex.toInt();
            }

            if (d1.PositionNumberInFile.toInt() != d2.PositionNumberInFile.toInt())
            {
                return d1.PositionNumberInFile.toInt() < d2.PositionNumberInFile.toInt();
            }

            return d1.FilesGroupIndex < d2.FilesGroupIndex;
        }

        return l < r;
    }

private:
    QString GetSortingHeader(const QString& line) const
    {
        const LogLineInfo lineInfo = m_lineParser->Parse(line);
        return lineInfo.HeaderItems[m_sortingGroupName];
    }

private:
    std::shared_ptr<ILogLineParser> m_lineParser;
    QString const m_sortingGroupName;
};

class LogLineComparator
{
public:
    explicit LogLineComparator(LogLineHeaderParsingParams const& logLineHeaderParsingParams) :
        m_sortingGroupName(logLineHeaderParsingParams.SortingGroup)
    {
        QString groupRegExp("");
        m_lineParser.reset(new RegExpLogLineParser(logLineHeaderParsingParams.HeaderGroupDatas, groupRegExp));
    }

    bool Equals(const EventPattern::PatternString& l, const EventPattern::PatternString& r) const
    {
        const auto hl = GetSortingHeader(l);
        const auto hr = GetSortingHeader(r);

        return hl == hr;
    }

    bool Less(const EventPattern::PatternString& l, const EventPattern::PatternString& r) const
    {
        const auto hl = GetSortingHeader(l);
        const auto hr = GetSortingHeader(r);

        return hl < hr;
    }

private:
    QString GetSortingHeader(const QString& line) const
    {
        const LogLineInfo lineInfo = m_lineParser->Parse(line);
        return lineInfo.HeaderItems[m_sortingGroupName];
    }

private:
    std::shared_ptr<ILogLineParser> m_lineParser;
    QString const m_sortingGroupName;
};

} // namespace

LogViewMainWindow::LogViewMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_logLineHeaderParsingParams(LoadLogLineHeaderParsingParams())
{
    qRegisterMetaType<Event>("Event");
    setMinimumSize(1024, 768);

    gui_selectedEventView = new QTreeWidget();
//    gui_selectedEventView->setColumnCount(2);
    gui_selectedEventView->setHeaderLabels(QStringList() << tr("No.[File][Group]") << tr("Log line"));
//    gui_selectedEventView->setStyleSheet("QTreeWidget { font-family: \"Helvetica\"; font-size: " + QString::number(11) + "pt; }");

    setStyleSheet("QSplitter::handle{background-color: black;}");
    QSplitter* verticalSplitter = new QSplitter(Qt::Orientation::Vertical);
    verticalSplitter->setHandleWidth(3);

    gui_viewsWidget = new QWidget();
    {
        QHBoxLayout* viewLayout = new QHBoxLayout();
        gui_viewsWidget->setLayout(viewLayout);
    }

    verticalSplitter->addWidget(gui_viewsWidget);

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
    CreateConnections();
}

LogViewMainWindow::~LogViewMainWindow() = default;

void LogViewMainWindow::LoadLogs(const QStringList& filenames, const int fileGroupsCount,
        const QString& eventsParsingConfigJson)
{
    AddView(fileGroupsCount);

    m_loadedFiles.push_back(filenames);
    std::sort(m_loadedFiles.back().begin(), m_loadedFiles.back().end(),
            [](const QString& l, const QString& r) -> bool
            {
                return QFileInfo(l).lastModified() < QFileInfo(r).lastModified();
            });

    BaseLinePositionStorage linePositionStorage;
    m_linesStorages.push_back(std::make_unique<BasePositionedLinesStorage>());

    EventPatternsHierarchyMatcher lineSelector;
    EventPatternsHierarchy::fromJson(eventsParsingConfigJson, lineSelector.EventPatterns);

    if (false)
    {
        EventPatternsEditDialog window;
        window.SetEventPatternsHierarchy(lineSelector.EventPatterns);
        int code = window.exec();
        if (code == QDialog::Accepted)
        {
            lineSelector.EventPatterns = window.GetEventPatternsHierarchy();
//            qDebug() << EventPatternsHierarchy::toJson(lineSelector.EventPatterns).toStdString().c_str();
        }
        else
        {
//            qDebug() << "rejected";
        }
    }

    FilesIndexer indexer(linePositionStorage, *m_linesStorages.back(), lineSelector);
    for (const auto& filename : m_loadedFiles.back())
    {
        indexer.AddFileIndexes(filename);
    }

    m_infoExtractor = std::make_unique<EventInfoExtractor>(m_logLineHeaderParsingParams);

    m_eventLevels.push_back(FindEvents(lineSelector.EventPatterns, *m_linesStorages.back(), *m_infoExtractor));
}

void LogViewMainWindow::UpdateViewportParams()
{
    if (m_linesStorages.empty())
    {
        if (!gui_EventsViewScenes.empty() &&
            !gui_EventsViews.empty())
        {
            for (std::size_t i = 0; i < gui_EventsViewScenes.size(); ++i)
            {
                gui_EventsViewScenes[i]->UpdateViewportParams(0, gui_EventsViews[i]->width());
            }
        }

        return;
    }

    if (!gui_EventsViewScenes.empty() &&
        !gui_EventsViews.empty())
    {
        LineNumber maxLinesCount = 0;
        for (std::size_t i = 0; i < gui_EventsViewScenes.size(); ++i)
        {
            const LineNumber lastLineGlobalIndex =
                    (m_linesStorages.size() <= i || m_linesStorages[i]->Size() == 0 ?
                         0 : m_linesStorages[i]->Back().Position.GlobalLineNumber);
            if (lastLineGlobalIndex > maxLinesCount)
            {
                maxLinesCount = lastLineGlobalIndex;
            }
        }

        for (std::size_t i = 0; i < gui_EventsViewScenes.size(); ++i)
        {
            gui_EventsViewScenes[i]->UpdateViewportParams(
                        static_cast<std::size_t>(maxLinesCount), gui_EventsViews[i]->width());
        }
    }
}

void LogViewMainWindow::LoadLogView()
{
    Invalidate();
}

QList<LineForEventData> LoadLinesForEvent(const Event& event, const QStringList& filenames, const IEventInfoExtractor* groupExtractor)
{
    QList<LineForEventData> result;

    if (event.Type == EventType::Single)
    {
        LineForEventData eventViewData;
        eventViewData.PositionIndex = QString::number(event.StartLine.Position.Index + 1);
        eventViewData.PositionNumberInFile = QString::number(event.StartLine.Position.NumberInFile + 1);
        eventViewData.Line = event.StartLine.Line;
        eventViewData.ColorCode = event.ViewColor.toColorCode();
        result.append(eventViewData);
    }
    else
    {
        {
            LineForEventData eventViewData;
            eventViewData.PositionIndex = QString::number(event.StartLine.Position.Index + 1);
            eventViewData.PositionNumberInFile = QString::number(event.StartLine.Position.NumberInFile + 1);
            eventViewData.Line = event.StartLine.Line;
            eventViewData.ColorCode = event.ViewColor.toColorCode();
            result.append(eventViewData);
        }

        std::function<bool(const QString& line)> isLineAcceptableFunctor =
                [&groupExtractor, &event](const QString& line) -> bool
        {
            return groupExtractor == nullptr ||
                   groupExtractor->GetGroupFromLine(line) == event.Group;
        };

        if (event.StartLine.Position.Index == event.EndLine.Position.Index)
        {
            QPair<QStringList, QList<int>> lines = LoadFileBlockToStrings(
                                    filenames.at(event.StartLine.Position.Index),
                                    event.StartLine.Position.Offset, event.EndLine.Position.Offset,
                                    isLineAcceptableFunctor);

            for (int i = 1; i < lines.first.size(); ++i)
            {
                LineForEventData eventViewData;
                eventViewData.PositionIndex = QString::number(event.StartLine.Position.Index + 1);
                eventViewData.PositionNumberInFile = QString::number(event.StartLine.Position.NumberInFile + lines.second.at(i) + 1);
                eventViewData.Line = lines.first[i];
                result.append(eventViewData);
            }
        }
        else
        {
            {
                QPair<QStringList, QList<int>> lines = LoadFileBlockToStrings(
                                        filenames.at(event.StartLine.Position.Index),
                                        event.StartLine.Position.Offset, 0, isLineAcceptableFunctor);

                for (int i = 1; i < lines.first.size(); ++i)
                {
                    LineForEventData eventViewData;
                    eventViewData.PositionIndex = QString::number(event.StartLine.Position.Index + 1);
                    eventViewData.PositionNumberInFile = QString::number(event.StartLine.Position.NumberInFile + lines.second.at(i) + 1);
                    eventViewData.Line = lines.first[i];
                    result.append(eventViewData);
                }
            }

            for (FileIndex i = event.StartLine.Position.Index + 1; i < event.EndLine.Position.Index - 1; ++i)
            {
                QPair<QStringList, QList<int>> lines = LoadFileBlockToStrings(filenames.at(i), 0, 0, isLineAcceptableFunctor);

                for (int j = 0; j < lines.first.size(); ++j)
                {
                    LineForEventData eventViewData;
                    eventViewData.PositionIndex = QString::number(i + 1);
                    eventViewData.PositionNumberInFile = QString::number(lines.second.at(j) + 1);
                    eventViewData.Line = lines.first[j];
                    result.append(eventViewData);
                }
            }

            {
                QPair<QStringList, QList<int>> lines = LoadFileBlockToStrings(
                                        filenames.at(event.EndLine.Position.Index),
                                        0, event.EndLine.Position.Offset,
                                        isLineAcceptableFunctor);

                for (int i = 0; i < lines.first.size(); ++i)
                {
                    LineForEventData eventViewData;
                    eventViewData.PositionIndex = QString::number(event.EndLine.Position.Index + 1);
                    eventViewData.PositionNumberInFile = QString::number(lines.second.at(i) + 1);
                    eventViewData.Line = lines.first[i];
                    result.append(eventViewData);
                }
            }
        }

        {
            LineForEventData eventViewData;
            eventViewData.PositionIndex = QString::number(event.EndLine.Position.Index + 1);
            eventViewData.PositionNumberInFile = QString::number(event.EndLine.Position.NumberInFile + 1);
            eventViewData.Line = event.EndLine.Line;
            eventViewData.ColorCode = event.ViewColor.toColorCode();
            result.append(eventViewData);
        }
    }

    return result;
}

void AppendEventLogLinesWithSorting(
        const QList<LineForEventData>& currentEventLogLines,
        QList<LineForEventData>& allEventsLogLines,
        const LogLineDataComparator& logLineDataComparator)
{
    allEventsLogLines.append(currentEventLogLines);
    std::sort(allEventsLogLines.begin(), allEventsLogLines.end(), logLineDataComparator);
}

void LogViewMainWindow::slot_EventSelectionChanged()
{
    gui_selectedEventView->clear();

    QList<LineForEventData> eventsLogLines;

    for (std::size_t i = 0; i < gui_EventsViewScenes.size(); ++i)
    {
        const auto& gui_EventsViewScene = gui_EventsViewScenes[i];

        for (const auto& currentEventItem : gui_EventsViewScene->GetSelectedEventItems())
        {
            QList<LineForEventData> currentEventLogLines = LoadLinesForEvent(currentEventItem->GetEvent(), m_loadedFiles[i], m_infoExtractor.get());

            for (auto& data : currentEventLogLines)
            {
                data.FilesGroupIndex = i;
            }

            AppendEventLogLinesWithSorting(currentEventLogLines, eventsLogLines, LogLineDataComparator(m_logLineHeaderParsingParams));
        }
    }

    const QString defaultColorCode = IMatchableEventPattern::Color().toColorCode();

    if (eventsLogLines.size() > 1)
    {
        QList<LineForEventData> eventsLogLinesTmp;
        eventsLogLinesTmp.append(eventsLogLines.front());

        for (int i = 1; i < eventsLogLines.size(); ++i)
        {
            if (eventsLogLinesTmp.back().PositionNumberInFile != eventsLogLines.at(i).PositionNumberInFile ||
                eventsLogLinesTmp.back().FilesGroupIndex != eventsLogLines.at(i).FilesGroupIndex)
            {
                eventsLogLinesTmp.append(eventsLogLines.at(i));
            }
            else
            {
                const bool prevItemHasColor = (eventsLogLinesTmp.back().ColorCode != defaultColorCode);
                const bool currItemHasColor = (eventsLogLines.at(i).ColorCode != defaultColorCode);

                if (!prevItemHasColor && currItemHasColor)
                {
                    eventsLogLinesTmp.back() = eventsLogLines.at(i);
                }
            }
        }

        eventsLogLines = std::move(eventsLogLinesTmp);
    }

    for (const auto& logLines : eventsLogLines)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(
                                    QStringList()
                                        << logLines.PositionNumberInFile +
                                            QString("[%1]").arg(logLines.PositionIndex) +
                                            QString("[%1]").arg(logLines.FilesGroupIndex)
                                        << logLines.Line);

        if (logLines.ColorCode != defaultColorCode)
        {
            item->setBackgroundColor(1, IMatchableEventPattern::Color::fromColorCode(logLines.ColorCode).toQColor());
        }

        gui_selectedEventView->addTopLevelItem(item);
    }
}

void LogViewMainWindow::slot_ViewScrolledTo(int value)
{
    for (std::size_t i = 0; i < gui_EventsViews.size(); ++i)
    {
        gui_EventsViews[i]->ScrollTo(value);
    }
}

void LogViewMainWindow::slot_LineViewed(int hPos)
{
    for (std::size_t i = 0; i < gui_EventsViews.size(); ++i)
    {
        gui_EventsViews[i]->ShowReferenceLine(hPos);
    }
}

void LogViewMainWindow::slot_act_openFileTriggred()
{
    LogFileWithConfigsOpenDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    QVector<QStringList> fileLists;
    fileLists.append(dialog.GetOpenLogFileNames());

    OpenFiles(fileLists, QStringList() << dialog.GetEventPatternConfig());
}

void LogViewMainWindow::slot_act_openMultipleFilesTriggred()
{
    LogFilesSelectionDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    const auto& selectedFilesData = dialog.GetLogFilesSelectionData();

    QVector<QStringList> fileLists;
    QStringList configsList;
    for (const auto& data : selectedFilesData)
    {
        fileLists.append(data.LogFileNames);
        configsList.append(data.EventPatternConfig);
    }

    OpenFiles(fileLists, configsList);
}

void LogViewMainWindow::slot_act_closeFileTriggred()
{
    CloseFiles();
}

void LogViewMainWindow::Invalidate()
{
    UpdateViewportParams();
    Redraw();
}

void LogViewMainWindow::OpenFiles(const QVector<QStringList> &fileLists, const QStringList &configFiles)
{
    CloseFiles();

    for (int i = 0; i < fileLists.size(); ++i)
    {
        const QString eventsParsingConfigJson = LoadFileToQString(configFiles.at(i));
        LoadLogs(fileLists.at(i), fileLists.size(), eventsParsingConfigJson);
    }

    LineNumber maxLinesCount = 0;
    for (std::size_t i = 0; i < gui_EventsViewScenes.size(); ++i)
    {
        const LineNumber lastLineGlobalIndex =
                (m_linesStorages.size() <= i || m_linesStorages[i]->Size() == 0 ?
                     0 : m_linesStorages[i]->Back().Position.GlobalLineNumber);
        if (lastLineGlobalIndex > maxLinesCount)
        {
            maxLinesCount = lastLineGlobalIndex;
        }
    }

    std::vector<std::size_t> currentPositions(m_linesStorages.size(), 0);
    LogLineComparator logLineComparator(m_logLineHeaderParsingParams);
    LineNumber globalIndex = 0;
    for (LineNumber ind = 0; ind < maxLinesCount; ++ind)
    {
        QString minLine = "";

        std::vector<std::size_t> availableGroups;
        for (std::size_t i = 0; i < m_linesStorages.size(); ++i)
        {
            if (currentPositions[i] < m_linesStorages[i]->Size())
            {
                availableGroups.push_back(i);
            }
        }

        if (availableGroups.empty())
        {
            break;
        }

        minLine = (*m_linesStorages[availableGroups[0]])[currentPositions[availableGroups[0]]].Line;
        for (std::size_t i = 1; i < availableGroups.size(); ++i)
        {
            if (logLineComparator.Less((*m_linesStorages[availableGroups[i]])[currentPositions[availableGroups[i]]].Line, minLine))
            {
                minLine = (*m_linesStorages[availableGroups[i]])[currentPositions[availableGroups[i]]].Line;
            }
        }

        for (std::size_t i = 0; i < availableGroups.size(); ++i)
        {
            if (logLineComparator.Equals(minLine, (*m_linesStorages[availableGroups[i]])[currentPositions[availableGroups[i]]].Line))
            {
                (*m_linesStorages[availableGroups[i]])[currentPositions[availableGroups[i]]].Position.GlobalLineNumber = globalIndex;
                ++currentPositions[availableGroups[i]];
            }
        }

        ++globalIndex;
    }

    for (std::size_t eventGroup = 0; eventGroup < m_eventLevels.size(); ++eventGroup)
    {
        for (std::size_t eventLevel = 0; eventLevel < m_eventLevels[eventGroup].size(); ++eventLevel)
        {
            for (std::size_t eventIndex = 0; eventIndex < m_eventLevels[eventGroup][eventLevel].size(); ++eventIndex)
            {
                const LineNumber startLineIndex = m_eventLevels[eventGroup][eventLevel][eventIndex].StartLine.Position.NumberInMatchedLines;
                const LineNumber endLineIndex = m_eventLevels[eventGroup][eventLevel][eventIndex].EndLine.Position.NumberInMatchedLines;

                m_eventLevels[eventGroup][eventLevel][eventIndex].StartLine.Position.GlobalLineNumber =
                        (*m_linesStorages[eventGroup])[startLineIndex].Position.GlobalLineNumber;
                m_eventLevels[eventGroup][eventLevel][eventIndex].EndLine.Position.GlobalLineNumber =
                        (*m_linesStorages[eventGroup])[endLineIndex].Position.GlobalLineNumber;
            }
        }
    }

    Invalidate();
}

void LogViewMainWindow::CloseFiles()
{
    gui_selectedEventView->clear();

    for (std::size_t i = 0; i < gui_EventsViews.size(); ++i)
    {
        EventsGraphicsScene* gui_EventsViewScene = gui_EventsViewScenes[i];
        EventsGraphicsView* gui_EventsView = gui_EventsViews[i];

        if (gui_EventsViewScene == nullptr ||
            gui_EventsView == nullptr)
        {
            return;
        }

        gui_EventsViewScene->Reset();

        disconnect(gui_EventsViewScene, SIGNAL(EventSelectionChanged()),
                   this, SLOT(slot_EventSelectionChanged()));
        disconnect(gui_EventsView, SIGNAL(ViewScrolledTo(int)),
                   this, SLOT(slot_ViewScrolledTo(int)));
        disconnect(gui_EventsView, SIGNAL(LineViewed(int)),
                this, SLOT(slot_LineViewed(int)));

        gui_EventsViewScene->deleteLater();
        gui_EventsView->deleteLater();

        gui_EventsView->hide();
    }

    gui_EventsViewScenes.clear();
    gui_EventsViews.clear();

    m_linesStorages.clear();
    m_eventLevels.clear();
    m_loadedFiles.clear();
}

void LogViewMainWindow::AddView(const int fileGroupsCount)
{
    EventsGraphicsScene* eventsViewScene = new EventsGraphicsScene();

    EventsGraphicsView* eventsGraphicsView = new EventsGraphicsView();
    eventsGraphicsView->setScene(eventsViewScene);

    gui_EventsViews.push_back(eventsGraphicsView);
    gui_EventsViewScenes.push_back(eventsViewScene);

    gui_viewsWidget->layout()->addWidget(eventsGraphicsView);
    gui_viewsWidget->setLayout(gui_viewsWidget->layout());

    eventsGraphicsView->resize(gui_viewsWidget->width() / fileGroupsCount - 25, eventsGraphicsView->height());

    connect(eventsViewScene, SIGNAL(EventSelectionChanged()),
            this, SLOT(slot_EventSelectionChanged()),
            Qt::QueuedConnection);

    connect(eventsGraphicsView, SIGNAL(ViewScrolledTo(int)),
            this, SLOT(slot_ViewScrolledTo(int)),
            Qt::QueuedConnection);

    connect(eventsGraphicsView, SIGNAL(LineViewed(int)),
            this, SLOT(slot_LineViewed(int)),
            Qt::QueuedConnection);
}

void LogViewMainWindow::resizeEvent(QResizeEvent* /*event*/)
{
    Invalidate();
}

void LogViewMainWindow::slot_act_copySelectedLinesToClipboard_Triggred()
{
    QClipboard *clipboard = QApplication::clipboard();
//    QString originalText = clipboard->text();

    QString text;
    for (int i = 0; i < gui_selectedEventView->topLevelItemCount(); ++i)
    {
        const auto item = gui_selectedEventView->topLevelItem(i);
        text.append(item->text(1) + "\n");
    }

    clipboard->setText(text);
}

void LogViewMainWindow::slot_act_editEventPatternsConfig_Triggred()
{
    EventPatternsEditDialog window(this);
    /*int code = */window.exec();
}

void LogViewMainWindow::slot_act_editLogLineParsingConfig_Triggred()
{
    LogLineHeaderParsingParamsEditDialog window(this);
    /*int code = */window.exec();
}

void LogViewMainWindow::slot_act_closeAppTriggred()
{
    CloseFiles();

    close();
}

void LogViewMainWindow::CreateActions()
{
    act_openFile = new QAction(tr("&Open log file"));
    act_openFile->setShortcut(QKeySequence("Ctrl+O"));

    act_openMultipleFiles = new QAction(tr("&Open multiple log files"));
    act_openMultipleFiles->setShortcut(QKeySequence("Ctrl+Shift+O"));

    act_exit = new QAction(tr("&Exit"));
//    act_exit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F4));

    act_closeFile = new QAction(tr("&Close file"));
    act_closeFile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F4));

    act_copySelectedLinesToClipboard = new QAction(tr("Copy Selected data to clipboard"));
    // Disabled because of conflicting with shortcut
//    act_copySelectedLinesToClipboard->setShortcut(QKeySequence("Ctrl+C"));
    shortcut_copySelectedLinesToClipboard = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);

    act_editEventPatternsConfig = new QAction(tr("Edit event patterns config"));
    act_editLogLineParsingConfig = new QAction(tr("Edit log line parsing config"));
}

void LogViewMainWindow::CreateMenuBar()
{
    gui_mainMenuBar = new QMenuBar;

    QMenu* fileMenu = new QMenu(tr("&File"));
    fileMenu->addAction(act_openFile);
    fileMenu->addAction(act_openMultipleFiles);
    fileMenu->addAction(act_closeFile);
    fileMenu->addSeparator();
    fileMenu->addAction(act_exit);

    QMenu* editMenu = new QMenu(tr("&Edit"));
    editMenu->addAction(act_copySelectedLinesToClipboard);

    QMenu* toolsMenu = new QMenu(tr("&Tools"));
    toolsMenu->addAction(act_editEventPatternsConfig);
    toolsMenu->addAction(act_editLogLineParsingConfig);

    gui_mainMenuBar->addMenu(fileMenu);
    gui_mainMenuBar->addMenu(editMenu);
    gui_mainMenuBar->addMenu(toolsMenu);

    setMenuBar(gui_mainMenuBar);
}

void LogViewMainWindow::CreateConnections()
{
    connect(act_openFile, SIGNAL(triggered()),
            this, SLOT(slot_act_openFileTriggred()), Qt::DirectConnection);
    connect(act_openMultipleFiles, SIGNAL(triggered()),
            this, SLOT(slot_act_openMultipleFilesTriggred()), Qt::DirectConnection);
    connect(act_closeFile, SIGNAL(triggered()),
            this, SLOT(slot_act_closeFileTriggred()), Qt::DirectConnection);
    connect(act_exit, SIGNAL(triggered()),
            this, SLOT(slot_act_closeAppTriggred()), Qt::DirectConnection);

    connect(act_copySelectedLinesToClipboard, SIGNAL(triggered()),
            this, SLOT(slot_act_copySelectedLinesToClipboard_Triggred()), Qt::DirectConnection);
    connect(shortcut_copySelectedLinesToClipboard, SIGNAL(activated()),
            this, SLOT(slot_act_copySelectedLinesToClipboard_Triggred()), Qt::DirectConnection);

    connect(act_editEventPatternsConfig, SIGNAL(triggered()),
            this, SLOT(slot_act_editEventPatternsConfig_Triggred()), Qt::DirectConnection);
    connect(act_editLogLineParsingConfig, SIGNAL(triggered()),
            this, SLOT(slot_act_editLogLineParsingConfig_Triggred()), Qt::DirectConnection);
}

void LogViewMainWindow::Redraw()
{
    if (gui_EventsViewScenes.empty() ||
        gui_EventsViews.empty())
    {
        return;
    }

    for (std::size_t i = 0; i < m_eventLevels.size(); ++i)
    {
        gui_EventsViewScenes[i]->DrawEventItems(m_eventLevels[i]);

        gui_EventsViews[i]->horizontalScrollBar()->setValue(gui_EventsViews[i]->horizontalScrollBar()->minimum());
        gui_EventsViews[i]->verticalScrollBar()->setValue(gui_EventsViews[i]->verticalScrollBar()->minimum());
        gui_EventsViews[i]->centerOn(0, 0);
    }
}
