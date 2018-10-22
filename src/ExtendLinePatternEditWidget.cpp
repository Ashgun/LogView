#include "ExtendLinePatternEditWidget.h"

#include <QVBoxLayout>
#include <QLabel>

ExtendLinePatternEditWidget::ExtendLinePatternEditWidget(QWidget *parent) : QWidget(parent)
{
    gui_patternName = new QLineEdit();
    gui_startPatternLineEdit = new LinePatternEditWidget();
    gui_endPatternLineEdit = new LinePatternEditWidget();
    gui_endViewColorEdit = new ColorSelectionViewWidget();
    gui_altEndPatternLineEdit = new LinePatternEditWidget();
    gui_altEndViewColorEdit = new ColorSelectionViewWidget();

    QVBoxLayout* box = new QVBoxLayout;

    box->addWidget(new QLabel(tr("Pattern name:")));
    box->addWidget(gui_patternName);
    box->addWidget(gui_startPatternLineEdit);
    box->addWidget(gui_endPatternLineEdit);
    box->addWidget(gui_endViewColorEdit);
    box->addWidget(gui_altEndPatternLineEdit);
    box->addWidget(gui_altEndViewColorEdit);

    setLayout(box);
}

ExtendLinePatternEditWidget::ExtendLinePatternEditWidget(const IMatchableEventPattern* patternBase, QWidget* parent) :
    ExtendLinePatternEditWidget(parent)
{
    SetLinePattern(patternBase);
}

IMatchableEventPatternPtr ExtendLinePatternEditWidget::GetPattern() const
{
    return CreateExtendedEventPattern(
                gui_patternName->text(),
                gui_startPatternLineEdit->GetEventPattern(),
                gui_endPatternLineEdit->GetEventPattern(), gui_altEndPatternLineEdit->GetEventPattern(),
                gui_endViewColorEdit->GetColor(), gui_altEndViewColorEdit->GetColor());
}

void ExtendLinePatternEditWidget::SetLinePattern(const IMatchableEventPattern* patternBase)
{
    const ExtendedEventPattern* pattern = dynamic_cast<const ExtendedEventPattern*>(patternBase);

    gui_patternName->setText(pattern->Name);
    gui_startPatternLineEdit->SetEventPattern(pattern->StartPattern);
    gui_endPatternLineEdit->SetEventPattern(pattern->EndPattern);
    gui_altEndPatternLineEdit->SetEventPattern(pattern->AltEndPattern);
    gui_endViewColorEdit->SetColor(pattern->SuccessEndColor);
    gui_altEndViewColorEdit->SetColor(pattern->AltEndColor);
}
