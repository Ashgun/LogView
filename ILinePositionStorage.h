#ifndef ILINEPOSITIONSTORAGE_H
#define ILINEPOSITIONSTORAGE_H

#include "LinePosition.h"

class ILinePositionStorage
{
public:
    virtual ~ILinePositionStorage() = default;

    virtual void AddPosition(LinePosition const& pos) = 0;
    virtual std::size_t Size() const = 0;

    virtual LinePosition const& operator[](LineNumber const& lineNumber) const = 0;
    virtual LinePosition& operator[](LineNumber const& lineNumber) = 0;

    virtual void Reset() = 0;
};

#endif // ILINEPOSITIONSTORAGE_H
