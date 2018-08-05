#include "BaseLinePositionStorage.h"

BaseLinePositionStorage::BaseLinePositionStorage()
{
    m_LineNumberToPositionMap.reserve(1000000);
}

void BaseLinePositionStorage::AddPosition(const LinePosition& pos)
{
    m_LineNumberToPositionMap.push_back(pos);
}

std::size_t BaseLinePositionStorage::Size() const
{
    return static_cast<std::size_t>(m_LineNumberToPositionMap.size());
}

const LinePosition& BaseLinePositionStorage::operator [](const LineNumber& lineNumber) const
{
    return m_LineNumberToPositionMap[lineNumber];
}

LinePosition&BaseLinePositionStorage::operator[](const LineNumber& lineNumber)
{
    return m_LineNumberToPositionMap[lineNumber];
}

void BaseLinePositionStorage::Reset()
{
    m_LineNumberToPositionMap.clear();
}
