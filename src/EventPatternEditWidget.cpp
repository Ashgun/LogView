#include "EventPatternEditWidget.h"

#include <QVBoxLayout>

EventPatternEditWidget::EventPatternEditWidget(QWidget *parent) : QWidget(parent)
{
    gui_SingleLinePatternEditWidget = new SingleLinePatternEditWidget();
    gui_ExtendLinePatternEditWidget = new ExtendLinePatternEditWidget();

    BuildGUI();
}

EventPatternEditWidget::EventPatternEditWidget(const IMatchableEventPattern* pattern, QWidget* parent) : QWidget(parent)
{
    if (pattern->GetType() == EventType::Single)
    {
        gui_SingleLinePatternEditWidget = new SingleLinePatternEditWidget(pattern);
        gui_ExtendLinePatternEditWidget = new ExtendLinePatternEditWidget();
    }
    else
    {
        gui_SingleLinePatternEditWidget = new SingleLinePatternEditWidget();
        gui_ExtendLinePatternEditWidget = new ExtendLinePatternEditWidget(pattern);
    }
}

IMatchableEventPatternPtr EventPatternEditWidget::GetPattern() const
{
    const int currentTabIndex = gui_editWidgetsTab->currentIndex();

    if (currentTabIndex == 0)
    {
        return gui_SingleLinePatternEditWidget->GetPattern();
    }
    else
    {
        return gui_ExtendLinePatternEditWidget->GetPattern();
    }
}

void EventPatternEditWidget::BuildGUI()
{
    gui_editWidgetsTab = new QTabWidget;
    gui_editWidgetsTab->setMovable(false);
    gui_editWidgetsTab->addTab(gui_SingleLinePatternEditWidget, tr("Single line event"));
    gui_editWidgetsTab->addTab(gui_ExtendLinePatternEditWidget, tr("Multiple line event"));

    QVBoxLayout* box = new QVBoxLayout;
    box->addWidget(gui_editWidgetsTab);
    setLayout(box);
}
