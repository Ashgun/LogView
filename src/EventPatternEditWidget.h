#ifndef EVENTPATTERNEDITWIDGET_H
#define EVENTPATTERNEDITWIDGET_H

#include <QWidget>
#include <QTabWidget>

#include "SingleLinePatternEditWidget.h"
#include "ExtendLinePatternEditWidget.h"

#include "Events.h"

class EventPatternEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EventPatternEditWidget(QWidget *parent = nullptr);
    EventPatternEditWidget(const IMatchableEventPattern* pattern, QWidget *parent = nullptr);

    IMatchableEventPatternPtr GetPattern() const;

signals:

public slots:

private:
    void BuildGUI();

private:
    QTabWidget* gui_editWidgetsTab;
    SingleLinePatternEditWidget* gui_SingleLinePatternEditWidget;
    ExtendLinePatternEditWidget* gui_ExtendLinePatternEditWidget;
};

#endif // EVENTPATTERNEDITWIDGET_H
