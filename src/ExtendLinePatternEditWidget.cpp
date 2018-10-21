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

    BuildGUI();
}

ExtendLinePatternEditWidget::ExtendLinePatternEditWidget(const IMatchableEventPattern* patternBase, QWidget* parent) : QWidget(parent)
{
    const ExtendedEventPattern* pattern = dynamic_cast<const ExtendedEventPattern*>(patternBase);

    gui_patternName = new QLineEdit(pattern->Name);
    gui_startPatternLineEdit = new LinePatternEditWidget(pattern->StartPattern);
    gui_endPatternLineEdit = new LinePatternEditWidget(pattern->EndPattern);
    gui_endViewColorEdit = new ColorSelectionViewWidget(pattern->SuccessEndColor);
    gui_altEndPatternLineEdit = new LinePatternEditWidget(pattern->AltEndPattern);
    gui_altEndViewColorEdit = new ColorSelectionViewWidget(pattern->AltEndColor);

    BuildGUI();
}

IMatchableEventPatternPtr ExtendLinePatternEditWidget::GetPattern() const
{
    return CreateExtendedEventPattern(
                gui_patternName->text(),
                gui_startPatternLineEdit->GetEventPattern(),
                gui_endPatternLineEdit->GetEventPattern(), gui_altEndPatternLineEdit->GetEventPattern(),
                gui_endViewColorEdit->GetColor(), gui_altEndViewColorEdit->GetColor());
}

void ExtendLinePatternEditWidget::BuildGUI()
{
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
