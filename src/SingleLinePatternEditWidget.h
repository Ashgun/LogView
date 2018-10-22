#ifndef SINGLELINEPATTERNEDITWIDGET_H
#define SINGLELINEPATTERNEDITWIDGET_H

#include <QWidget>
#include <QLineEdit>

#include "LinePatternEditWidget.h"
#include "ColorSelectionViewWidget.h"

#include "Events.h"

class SingleLinePatternEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SingleLinePatternEditWidget(QWidget *parent = nullptr);
    SingleLinePatternEditWidget(const IMatchableEventPattern* pattern, QWidget *parent = nullptr);

    IMatchableEventPatternPtr GetPattern() const;
    void SetLinePattern(const IMatchableEventPattern* pattern);

signals:

public slots:

private:
    QLineEdit* gui_patternName;
    LinePatternEditWidget* gui_patternLineEdit;
    ColorSelectionViewWidget* gui_viewColorEdit;
};

#endif // SINGLELINEPATTERNEDITWIDGET_H
