#include "EventPatternEditWidget.h"

#include <QVBoxLayout>

EventPatternEditWidget::EventPatternEditWidget(QWidget *parent) : QWidget(parent)
{
    gui_SingleLinePatternEditWidget = new SingleLinePatternEditWidget();
    gui_ExtendLinePatternEditWidget = new ExtendLinePatternEditWidget();

    QWidget* singleLineEventEditContainer = new QWidget;
    {
        QVBoxLayout* box = new QVBoxLayout;
        box->addWidget(gui_SingleLinePatternEditWidget);
        box->addStretch();
        singleLineEventEditContainer->setLayout(box);
    }

    gui_editWidgetsTab = new QTabWidget;
    gui_editWidgetsTab->setMovable(false);
    gui_editWidgetsTab->addTab(singleLineEventEditContainer, tr("Single line event"));
    gui_editWidgetsTab->addTab(gui_ExtendLinePatternEditWidget, tr("Multiple line event"));

    QVBoxLayout* box = new QVBoxLayout;
    box->addWidget(gui_editWidgetsTab);
    setLayout(box);
}

EventPatternEditWidget::EventPatternEditWidget(const IMatchableEventPattern* pattern, QWidget* parent) :
    EventPatternEditWidget(parent)
{
    SetLinePattern(pattern);
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

void EventPatternEditWidget::SetLinePattern(const IMatchableEventPattern* pattern)
{
    if (pattern->GetType() == EventType::Single)
    {
        gui_SingleLinePatternEditWidget->SetLinePattern(pattern);
        gui_editWidgetsTab->setCurrentIndex(0);
    }
    else
    {
        gui_ExtendLinePatternEditWidget->SetLinePattern(pattern);
        gui_editWidgetsTab->setCurrentIndex(1);
    }
}
