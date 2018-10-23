#include "ColorSelectionViewWidget.h"

#include <QColorDialog>

ColorSelectionViewWidget::ColorSelectionViewWidget(QWidget* parent) :
    ColorSelectionViewWidget(IMatchableEventPattern::Color::fromQColor(QColor(Qt::white)), parent)
{
}

ColorSelectionViewWidget::ColorSelectionViewWidget(const IMatchableEventPattern::Color& color, QWidget* parent) :
    QFrame(parent),
    m_color(color.toQColor())
{
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(1);
    setMinimumSize(20, 20);
    UpdateColorView();
}

IMatchableEventPattern::Color ColorSelectionViewWidget::GetColor() const
{
    return IMatchableEventPattern::Color::fromQColor(m_color);
}

void ColorSelectionViewWidget::SetColor(const IMatchableEventPattern::Color& color)
{
    m_color = color.toQColor();
    UpdateColorView();
}

void ColorSelectionViewWidget::mousePressEvent(QMouseEvent* /*event*/)
{
    QColor newColor = QColorDialog::getColor(m_color, this);

    if (newColor.isValid())
    {
        m_color = newColor;
        UpdateColorView();
    }
}

void ColorSelectionViewWidget::UpdateColorView()
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, m_color);
    setAutoFillBackground(true);
    setPalette(Pal);
}
