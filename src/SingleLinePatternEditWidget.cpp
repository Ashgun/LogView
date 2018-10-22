#include "SingleLinePatternEditWidget.h"

#include <QVBoxLayout>
#include <QLabel>

SingleLinePatternEditWidget::SingleLinePatternEditWidget(QWidget *parent) : QWidget(parent)
{
    gui_patternName = new QLineEdit();
    gui_patternLineEdit = new LinePatternEditWidget();
    gui_viewColorEdit = new ColorSelectionViewWidget();

    QVBoxLayout* box = new QVBoxLayout;

    box->addWidget(new QLabel(tr("Pattern name:")));
    box->addWidget(gui_patternName);
    box->addWidget(gui_patternLineEdit);
    box->addWidget(gui_viewColorEdit);

    setLayout(box);
}

SingleLinePatternEditWidget::SingleLinePatternEditWidget(const IMatchableEventPattern* patternBase, QWidget* parent) :
    SingleLinePatternEditWidget(parent)
{
    SetLinePattern(patternBase);
}

IMatchableEventPatternPtr SingleLinePatternEditWidget::GetPattern() const
{
    return CreateSingleEventPattern(
                gui_patternName->text(),
                gui_patternLineEdit->GetEventPattern(),
                gui_viewColorEdit->GetColor());
}

void SingleLinePatternEditWidget::SetLinePattern(const IMatchableEventPattern* patternBase)
{
    const SingleEventPattern* pattern = dynamic_cast<const SingleEventPattern*>(patternBase);

    gui_patternName->setText(pattern->Name);
    gui_patternLineEdit->SetEventPattern(pattern->Pattern);
    gui_viewColorEdit->SetColor(pattern->ViewColor);
}
