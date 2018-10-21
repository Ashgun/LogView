#include "ColorSelectionViewWidget.h"

#include <QColorDialog>

ColorSelectionViewWidget::ColorSelectionViewWidget(QWidget* parent) :
    QWidget(parent),
    m_color(Qt::white)
{
    UpdateColorView();
}

ColorSelectionViewWidget::ColorSelectionViewWidget(const IMatchableEventPattern::Color& color, QWidget* parent) :
    QWidget(parent),
    m_color(color.toQColor())
{
    UpdateColorView();
}

IMatchableEventPattern::Color ColorSelectionViewWidget::GetColor() const
{
    return IMatchableEventPattern::Color::fromQColor(m_color);
}

void ColorSelectionViewWidget::mousePressEvent(QMouseEvent* /*event*/)
{
    m_color = QColorDialog::getColor(m_color, this);
    UpdateColorView();
}

void ColorSelectionViewWidget::UpdateColorView()
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, m_color);
    setAutoFillBackground(true);
    setPalette(Pal);
}
