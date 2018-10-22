#ifndef EXTENDLINEPATTERNEDITWIDGET_H
#define EXTENDLINEPATTERNEDITWIDGET_H

#include <QWidget>
#include <QLineEdit>

#include "LinePatternEditWidget.h"
#include "ColorSelectionViewWidget.h"

#include "Events.h"

class ExtendLinePatternEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExtendLinePatternEditWidget(QWidget *parent = nullptr);
    ExtendLinePatternEditWidget(const IMatchableEventPattern* pattern, QWidget *parent = nullptr);

    IMatchableEventPatternPtr GetPattern() const;
    void SetLinePattern(const IMatchableEventPattern* pattern);

signals:

public slots:

private:
    QLineEdit* gui_patternName;
    LinePatternEditWidget* gui_startPatternLineEdit;
    LinePatternEditWidget* gui_endPatternLineEdit;
    ColorSelectionViewWidget* gui_endViewColorEdit;
    LinePatternEditWidget* gui_altEndPatternLineEdit;
    ColorSelectionViewWidget* gui_altEndViewColorEdit;
};

#endif // EXTENDLINEPATTERNEDITWIDGET_H
