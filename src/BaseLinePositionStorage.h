#ifndef BASELINEPOSITIONSTORAGE_H
#define BASELINEPOSITIONSTORAGE_H

#include "ILinePositionStorage.h"

#include <QVector>

class BaseLinePositionStorage : public ILinePositionStorage
{
public:
    BaseLinePositionStorage();

    ~BaseLinePositionStorage() override = default;

    // ILinePositionStorage interface
public:
    void AddPosition(const LinePosition& pos) override;
    std::size_t Size() const override;
    const LinePosition& operator[](const LineNumber& lineNumber) const override;
    LinePosition& operator[](const LineNumber& lineNumber) override;
    void Reset() override;

private:
    QVector<LinePosition> m_LineNumberToPositionMap;
};

#endif // BASELINEPOSITIONSTORAGE_H
