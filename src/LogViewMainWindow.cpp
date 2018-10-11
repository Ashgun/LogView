#include "LogViewMainWindow.h"

#include <QHBoxLayout>
#include <QScrollBar>

#include <QDebug>

#include "CustomItem.h"
#include "EventGraphicsItem.h"
#include "Events.h"
#include "IPositionedLinesStorage.h"

namespace ViewParams
{

const int BaseVerticalSkip = 15;
const int BaseEventHeight = 15;

const int VerticalSpace = 3;

const int BaseHorizontalSkip = 7;

} // namespace ViewParams

namespace
{

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

LogViewMainWindow::LogViewMainWindow(
        IPositionedLinesStorage& linesStorage,
        const std::vector<std::vector<Event>>& eventLevels,
        QWidget *parent) :
    QMainWindow(parent)
{
    qRegisterMetaType<Event>("Event");

    std::size_t linesCount = linesStorage.Size();

    gui_EventsViewScene = new EventsGraphicsScene(this);
    gui_EventsViewScene->setSceneRect(
                0, 0,
                width() * 2,
                (linesCount + 1) * (ViewParams::BaseEventHeight + ViewParams::VerticalSpace) +
                    2 * ViewParams::BaseVerticalSkip);

    gui_EventsView = new QGraphicsView();
    gui_EventsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    gui_EventsView->setScene(gui_EventsViewScene);

//    QHBoxLayout* layout = new QHBoxLayout();
//    layout->addWidget(gui_EventsView);
//    setLayout(layout);

//    layout()->addWidget(gui_EventsView);

    setCentralWidget(gui_EventsView);

    std::list<EventGraphicsItem*> eventsToView =
            GenerateEventViewItems(eventLevels, gui_EventsViewScene->width(), *gui_EventsViewScene);

    for (auto& eventViewItem : eventsToView)
    {
        gui_EventsViewScene->addItem(eventViewItem);
    }
    eventsToView.clear();

    gui_EventsView->horizontalScrollBar()->setValue(gui_EventsView->horizontalScrollBar()->minimum());
    gui_EventsView->verticalScrollBar()->setValue(gui_EventsView->verticalScrollBar()->minimum());
    gui_EventsView->centerOn(0, 0);

    connect(gui_EventsViewScene, SIGNAL(EventSelected(Event)),
            this, SLOT(slot_EventSelected(Event)),
            Qt::QueuedConnection);
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
