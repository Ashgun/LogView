#ifndef LINEPATTERNEDITWIDGET_H
#define LINEPATTERNEDITWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QRadioButton>
#include <QGroupBox>

#include "EventPattern.h"

class LinePatternEditWidget : public QWidget
{
    Q_OBJECT

public:
    using PatternInfo = QPair<EventPattern::PatternString, EventPattern::PatternType>;

public:
    explicit LinePatternEditWidget(QWidget *parent = nullptr);
    LinePatternEditWidget(const EventPattern& pattern, QWidget *parent = nullptr);

    PatternInfo GetInfo() const;
    EventPattern GetEventPattern() const;

signals:

public slots:

private:
    QLineEdit* gui_patternLineEdit;
    QGroupBox* gui_TypeSelectionGroup;
    QRadioButton* gui_stringType;
    QRadioButton* gui_regExpType;
};

#endif // LINEPATTERNEDITWIDGET_H
