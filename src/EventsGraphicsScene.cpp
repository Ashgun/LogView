#include "EventsGraphicsScene.h"

#include <QApplication>

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

} // namespace

EventsGraphicsScene::EventsGraphicsScene(QObject* parent) :
    QGraphicsScene(parent)
{
}

void EventsGraphicsScene::DrawEventItems(const std::vector<std::vector<Event> >& eventLevels)
{
    if (m_eventsToView.empty())
    {
        clear();
        m_eventsToView = GenerateEventViewItems(eventLevels, width(), *this);
        for (auto& eventViewItem : m_eventsToView)
        {
            addItem(eventViewItem);
        }
    }
    else
    {
        for (auto& eventViewItem : m_eventsToView)
        {
            eventViewItem->ScaleHorizontally(width() / m_previosGraphicsSceneWidth);
        }
    }

    m_previosGraphicsSceneWidth = width();
}

void EventsGraphicsScene::UpdateViewportParams(const std::size_t linesCount, const int baseViewPortWidth)
{
    setSceneRect(0, 0,
                 baseViewPortWidth - 15,
                 (linesCount + 1) * (ViewParams::BaseEventHeight) + 2 * ViewParams::BaseVerticalSkip);
}

const QList<EventGraphicsItem*>&EventsGraphicsScene::GetSelectedEventItems() const
{
    return m_selectedEventItems;
}

void EventsGraphicsScene::OnEventSelection(EventGraphicsItem* eventGraphicsItem)
{
    const bool isCtrlPressed = QApplication::keyboardModifiers().testFlag(Qt::ControlModifier);

    if (!isCtrlPressed)
    {
        for (auto& eventItem : m_selectedEventItems)
        {
            if (eventItem != nullptr)
            {
                eventItem->Unselect();
            }
        }

        m_selectedEventItems.clear();
    }

    m_selectedEventItems.push_back(eventGraphicsItem);
    eventGraphicsItem->Select();

    emit EventSelectionChanged();
}
