#include "SingleLinePatternEditWidget.h"

#include <QVBoxLayout>
#include <QLabel>

SingleLinePatternEditWidget::SingleLinePatternEditWidget(QWidget *parent) : QWidget(parent)
{
    gui_patternName = new QLineEdit();
    gui_patternLineEdit = new LinePatternEditWidget();
    gui_viewColorEdit = new ColorSelectionViewWidget();

    BuildGUI();
}

SingleLinePatternEditWidget::SingleLinePatternEditWidget(const IMatchableEventPattern* patternBase, QWidget* parent) : QWidget(parent)
{
    const SingleEventPattern* pattern = dynamic_cast<const SingleEventPattern*>(patternBase);

    gui_patternName = new QLineEdit(pattern->Name);
    gui_patternLineEdit = new LinePatternEditWidget(pattern->Pattern);
    gui_viewColorEdit = new ColorSelectionViewWidget(pattern->ViewColor);

    BuildGUI();
}

IMatchableEventPatternPtr SingleLinePatternEditWidget::GetPattern() const
{
    return CreateSingleEventPattern(
                gui_patternName->text(),
                gui_patternLineEdit->GetEventPattern(),
                gui_viewColorEdit->GetColor());
}

void SingleLinePatternEditWidget::BuildGUI()
{
    QVBoxLayout* box = new QVBoxLayout;

    box->addWidget(new QLabel(tr("Pattern name:")));
    box->addWidget(gui_patternName);
    box->addWidget(gui_patternLineEdit);
    box->addWidget(gui_viewColorEdit);

    setLayout(box);
}
