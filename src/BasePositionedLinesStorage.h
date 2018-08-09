#ifndef BASEPOSITIONEDLINESSTORAGE_H
#define BASEPOSITIONEDLINESSTORAGE_H

#include "IPositionedLinesStorage.h"

#include <vector>

class BasePositionedLinesStorage : public IPositionedLinesStorage
{
private:
    using PositionedLines = std::vector<PositionedLine>;

public:
    BasePositionedLinesStorage() = default;
    explicit BasePositionedLinesStorage(BasePositionedLinesStorage const& other);
    ~BasePositionedLinesStorage() = default;

    void AddLine(const PositionedLine& positionedLine) override;
    std::size_t Size() const override;
    const PositionedLine&operator [](const std::size_t index) const override;
    PositionedLine&operator [](const std::size_t index) override;

    std::unique_ptr<IPositionedLinesStorage> Clone() const override;

private:
    PositionedLines m_lines;
};

#endif // BASEPOSITIONEDLINESSTORAGE_H
