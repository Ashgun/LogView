#ifndef COLORSELECTIONVIEWWIDGET_H
#define COLORSELECTIONVIEWWIDGET_H

#include <QWidget>

#include "Events.h"

class ColorSelectionViewWidget : public QWidget
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
