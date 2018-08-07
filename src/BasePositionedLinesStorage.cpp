#include "BasePositionedLinesStorage.h"

void BasePositionedLinesStorage::AddLine(const PositionedLine& positionedLine)
{
    m_lines.push_back(positionedLine);
}

std::size_t BasePositionedLinesStorage::Size() const
{
    return m_lines.size();
}

const PositionedLine&BasePositionedLinesStorage::operator [](const std::size_t index) const
{
    return m_lines[index];
}

PositionedLine&BasePositionedLinesStorage::operator [](const std::size_t index)
{
    return m_lines[index];
}
