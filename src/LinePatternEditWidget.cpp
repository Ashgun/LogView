#include "LinePatternEditWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

LinePatternEditWidget::LinePatternEditWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumWidth(200);

    gui_patternLineEdit = new QLineEdit();
    gui_TypeSelectionGroup = new QGroupBox(tr("Pattern type"));

    {
        gui_stringType = new QRadioButton(tr("String"));
        gui_regExpType = new QRadioButton(tr("RegExp"));
        gui_stringType->setChecked(true);

        QHBoxLayout *box = new QHBoxLayout;
        box->addWidget(gui_stringType);
        box->addWidget(gui_regExpType);
        box->addStretch(1);
        gui_TypeSelectionGroup->setLayout(box);
    }

    QVBoxLayout* box = new QVBoxLayout();
    box->addWidget(new QLabel(tr("Pattern line:")));
    box->addWidget(gui_patternLineEdit);
    box->addWidget(gui_TypeSelectionGroup);

    setLayout(box);
}

LinePatternEditWidget::LinePatternEditWidget(const EventPattern& pattern, QWidget* parent) :
    LinePatternEditWidget(parent)
{
    SetEventPattern(pattern);
}

LinePatternEditWidget::PatternInfo LinePatternEditWidget::GetInfo() const
{
    return PatternInfo(
                gui_patternLineEdit->text(),
                (gui_stringType->isChecked() ? PatternInfo::second_type::String : PatternInfo::second_type::RegExp));
}

EventPattern LinePatternEditWidget::GetEventPattern() const
{
    const auto& info = GetInfo();
    return info.second == EventPattern::PatternType::String ?
            EventPattern::CreateStringPattern(info.first) :
            EventPattern::CreateRegExpPattern(info.first);
}

void LinePatternEditWidget::SetEventPattern(const EventPattern& pattern)
{
    gui_patternLineEdit->setText(pattern.Pattern);
    gui_stringType->setChecked(pattern.Type == EventPattern::PatternType::String);
    gui_regExpType->setChecked(pattern.Type == EventPattern::PatternType::RegExp);
}
