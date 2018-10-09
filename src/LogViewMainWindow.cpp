#include "LogViewMainWindow.h"

#include <QHBoxLayout>
#include <QScrollBar>

#include <QDebug>

#include "CustomItem.h"
#include "EventGraphicsItem.h"
#include "Events.h"

LogViewMainWindow::LogViewMainWindow(const std::vector<std::vector<Event>>& eventLevels, QWidget *parent) : QMainWindow(parent)
{
    qRegisterMetaType<Event>("Event");

    gui_EventsViewScene = new EventsGraphicsScene(this);
    gui_EventsViewScene->setSceneRect(0, 0, width() * 2, height() * 10);

    gui_EventsView = new QGraphicsView();
    gui_EventsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    gui_EventsView->setScene(gui_EventsViewScene);

//    QHBoxLayout* layout = new QHBoxLayout();
//    layout->addWidget(gui_EventsView);
//    setLayout(layout);

//    layout()->addWidget(gui_EventsView);

    setCentralWidget(gui_EventsView);


//    gui_EventsViewScene->addItem(new CustomItem(20, 20, 60, 60));
//    gui_EventsViewScene->addItem(new CustomItem(30, 30, 60, 60));
//    gui_EventsViewScene->addItem(new CustomItem(40, 40, 60, 60));

//    Event event;
//    event.Name = "event1";
//    event.Level = 0;
//    gui_EventsViewScene->addItem(new EventGraphicsItem(event, 20, 20, 100, 50, *gui_EventsViewScene));

//    event.Name = "Event2q";
//    event.Level = 1;
//    gui_EventsViewScene->addItem(new EventGraphicsItem(event, 30, 25, 60, 30, *gui_EventsViewScene));


    std::vector<std::map<QString, std::size_t>> groupIndixes(eventLevels.size());

    for (std::size_t level = 0; level < eventLevels.size(); ++level)
    {
        std::map<QString, std::size_t>& groups = groupIndixes[level];
        if (level > 1)
        {
            groups = groupIndixes[level - 1];
        }
        for (std::size_t i = 0; i < eventLevels[level].size(); ++i)
        {
            if (groups.find(eventLevels[level][i].Group) == groups.end())
            {
                const std::size_t groupsCount = groups.size();
                groups[eventLevels[level][i].Group] = groupsCount;
            }
        }

        for (std::size_t i = 0; i < eventLevels[level].size(); ++i)
        {
            const std::size_t groupIndex = groups[eventLevels[level][i].Group];

            const int baseY = 15;
            const int baseHeight = 15;
            const int verticalSpace = 2;

            const int baseX = 5;

            const qreal y = eventLevels[level][i].StartLine.Position.Number * baseHeight + baseY;
            const qreal height =
                    (eventLevels[level][i].EndLine.Position.Number - eventLevels[level][i].StartLine.Position.Number) * baseHeight +
                    baseHeight - verticalSpace;

            const qreal groupViewWidth = gui_EventsViewScene->width() / groups.size();
            const qreal shiftX = baseX + eventLevels[level][i].Level * baseX;
            const qreal x = shiftX + groupViewWidth * groupIndex;
            const qreal width = groupViewWidth - 2 * shiftX;

            gui_EventsViewScene->addItem(
                        new EventGraphicsItem(
                            eventLevels[level][i],
                            x, y, width, height,
                            *gui_EventsViewScene)
                        );
        }
    }

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
    qDebug() << event.StartLine.Line;
    qDebug() << event.EndLine.Line;
}
