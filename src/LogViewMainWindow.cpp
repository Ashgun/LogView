#include "LogViewMainWindow.h"

#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>

#include <QDebug>

#include "BaseLinePositionStorage.h"
#include "BasePositionedLinesStorage.h"
#include "CustomItem.h"
#include "Events.h"
#include "FilesIndexer.h"
#include "IPositionedLinesStorage.h"
#include "RegExpLogLineParser.h"

#include "EventPatternsEditDialog.h"
#include "LogFileWithConfigsOpenDialog.h"

#include "Common.h"

#include <stdexcept>

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

QStringList LoadFileBlockToStrings(const QString& filename, const quint64 from, const quint64 to)
{
    QStringList resultList;

    QFile binfile(filename);

    quint64 const bufferSize = to - from;
    std::vector<char> buffer(static_cast<std::size_t>(bufferSize + 1));
    char* bufferData = buffer.data();

    const auto IsEndOfLineSymbol =
            [](const char& ch) -> bool
            {
                return ((ch == '\n') || (ch == '\r'));
            };

    if (binfile.open(QIODevice::ReadOnly))
    {
        binfile.seek(static_cast<qint64>(from));

        QDataStream in(&binfile);

        std::size_t const readBytesCount = static_cast<std::size_t>(in.readRawData(bufferData, static_cast<int>(bufferSize)));

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

} // namespace

LogViewMainWindow::LogViewMainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_logLineHeaderParsingParams(LoadLogLineHeaderParsingParams())
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
    CreateConnections();
}

LogViewMainWindow::~LogViewMainWindow() = default;

void LogViewMainWindow::LoadLog(
        const QString& filename,
        const QString& eventsParsingConfigJson)
{
    m_loadedFile = filename;

    BaseLinePositionStorage linePositionStorage;
    m_linesStorage = std::make_unique<BasePositionedLinesStorage>();

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

    FilesIndexer indexer(linePositionStorage, *m_linesStorage, lineSelector);
    indexer.AddFileIndexes(filename);

    m_groupExtractor = std::make_unique<EventGroupExtractor>(m_logLineHeaderParsingParams);

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
    gui_EventsViewScene->UpdateViewportParams(linesCount, gui_EventsView->width());
}

void LogViewMainWindow::LoadLogView()
{
    Invalidate();
}

QList<QStringList> LoadLinesForEvent(const Event& event, const QString& filename, const IEventGroupExtractor* groupExtractor)
{
    QList<QStringList> result;

    if (event.Type == EventType::Single)
    {
        QStringList eventViewData;
        eventViewData
            << QString::number(event.StartLine.Position.NumberInFile)
            << event.StartLine.Line
            << event.ViewColor.toColorCode();

        result.append(eventViewData);
    }
    else
    {
        {
            QStringList eventViewData;
            eventViewData
                << QString::number(event.StartLine.Position.NumberInFile)
                << event.StartLine.Line
                << event.ViewColor.toColorCode();
            result.append(eventViewData);
        }

        QStringList lines = LoadFileBlockToStrings(
                                filename,
                                event.StartLine.Position.Offset, event.EndLine.Position.Offset);

        for (int i = 1; i < lines.size(); ++i)
        {
            if (groupExtractor == nullptr ||
                groupExtractor->GetGroupFromLine(lines[i]) != event.Group)
            {
                continue;
            }

            QStringList eventViewData;
            eventViewData
                << QString::number(event.StartLine.Position.NumberInFile + static_cast<unsigned int>(i))
                << lines[i];
            result.append(eventViewData);
        }

        {
            QStringList eventViewData;
            eventViewData
                << QString::number(event.EndLine.Position.NumberInFile)
                << event.EndLine.Line
                << event.ViewColor.toColorCode();
            result.append(eventViewData);
        }
    }

    return result;
}

class LogLineDataComparator
{
public:
    explicit LogLineDataComparator(LogLineHeaderParsingParams const& logLineHeaderParsingParams) :
        m_sortingGroupName(logLineHeaderParsingParams.SortingGroup)
    {
        QString groupRegExp("");
        m_lineParser.reset(new RegExpLogLineParser(logLineHeaderParsingParams.HeaderGroupRegExps, groupRegExp));
    }

    bool operator()(const QStringList &d1, const QStringList &d2) const
    {
        return GetSortingHeader(d1.at(1)) < GetSortingHeader(d2.at(1));
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

void AppendEventLogLinesWithSorting(
        const QList<QStringList>& currentEventLogLines,
        QList<QStringList>& allEventsLogLines,
        const LogLineDataComparator& logLineDataComparator)
{
    allEventsLogLines.append(currentEventLogLines);
    std::sort(allEventsLogLines.begin(), allEventsLogLines.end(), logLineDataComparator);
}

void LogViewMainWindow::slot_EventSelectionChanged()
{
    gui_selectedEventView->clear();

    QList<QStringList> eventsLogLines;
    for (const auto& currentEventItem : gui_EventsViewScene->GetSelectedEventItems())
    {
        QList<QStringList> currentEventLogLines = LoadLinesForEvent(currentEventItem->GetEvent(), m_loadedFile, m_groupExtractor.get());
        AppendEventLogLinesWithSorting(currentEventLogLines, eventsLogLines, LogLineDataComparator(m_logLineHeaderParsingParams));
    }

    for (const auto& logLines : eventsLogLines)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << logLines[0] << logLines[1]);

        if (logLines.size() == 3)
        {
            item->setBackgroundColor(1, IMatchableEventPattern::Color::fromColorCode(logLines[2]).toQColor());
        }

        gui_selectedEventView->addTopLevelItem(item);
    }
}

void LogViewMainWindow::slot_act_openFileTriggred()
{
    LogFileWithConfigsOpenDialog dialog;

    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    const QString eventsParsingConfigJson = LoadFileToQString(dialog.GetEventPatternConfig());

    LoadLog(dialog.GetOpenLogFileName(), eventsParsingConfigJson);
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

void LogViewMainWindow::CreateActions()
{
    act_openFile = new QAction(tr("&Open log file"));

    act_copySelectedLinesToClipboard = new QAction(tr("Copy Selected data to clipboard"));
    // Disabled because of conflicting with shortcut
//    act_copySelectedLinesToClipboard->setShortcut(QKeySequence("Ctrl+C"));
    shortcut_copySelectedLinesToClipboard = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this);
}

void LogViewMainWindow::CreateMenuBar()
{
    gui_mainMenuBar = new QMenuBar;

    QMenu* fileMenu = new QMenu(tr("&File"));
    fileMenu->addAction(act_openFile);

    QMenu* editMenu = new QMenu(tr("&Edit"));
    editMenu->addAction(act_copySelectedLinesToClipboard);

    gui_mainMenuBar->addMenu(fileMenu);
    gui_mainMenuBar->addMenu(editMenu);

    setMenuBar(gui_mainMenuBar);
}

void LogViewMainWindow::CreateConnections()
{
    connect(gui_EventsViewScene, SIGNAL(EventSelectionChanged()),
            this, SLOT(slot_EventSelectionChanged()),
            Qt::QueuedConnection);

    connect(act_openFile, SIGNAL(triggered()),
            this, SLOT(slot_act_openFileTriggred()), Qt::DirectConnection);

    connect(act_copySelectedLinesToClipboard, SIGNAL(triggered()),
            this, SLOT(slot_act_copySelectedLinesToClipboard_Triggred()), Qt::DirectConnection);
    connect(shortcut_copySelectedLinesToClipboard, SIGNAL(activated()),
            this, SLOT(slot_act_copySelectedLinesToClipboard_Triggred()), Qt::DirectConnection);
}

void LogViewMainWindow::Redraw()
{
    gui_EventsViewScene->DrawEventItems(m_eventLevels);

    gui_EventsView->horizontalScrollBar()->setValue(gui_EventsView->horizontalScrollBar()->minimum());
    gui_EventsView->verticalScrollBar()->setValue(gui_EventsView->verticalScrollBar()->minimum());
    gui_EventsView->centerOn(0, 0);
}
