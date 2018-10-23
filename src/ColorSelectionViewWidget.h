#ifndef COLORSELECTIONVIEWWIDGET_H
#define COLORSELECTIONVIEWWIDGET_H

#include <QFrame>

#include "Events.h"

class ColorSelectionViewWidget : public QFrame
{
    Q_OBJECT
public:
    explicit ColorSelectionViewWidget(QWidget *parent = nullptr);
    ColorSelectionViewWidget(const IMatchableEventPattern::Color& color, QWidget *parent = nullptr);

    IMatchableEventPattern::Color GetColor() const;
    void SetColor(const IMatchableEventPattern::Color& color);

protected slots:
    void mousePressEvent(QMouseEvent * event) override;

private:
    void UpdateColorView();

private:
    QColor m_color;
};

#endif // COLORSELECTIONVIEWWIDGET_H
