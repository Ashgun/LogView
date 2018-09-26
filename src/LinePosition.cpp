#include "LinePosition.h"

LinePosition::LinePosition() : LinePosition(0, 0, 0)
{
}

LinePosition::LinePosition(FileOffset fileOffset, FileIndex fileId, LineNumber number) :
    Offset(fileOffset),
    Index(fileId),
    Number(number)
{
}

bool LinePosition::operator==(const LinePosition& other) const
{
    return Offset == other.Offset && Index == other.Index && Number == other.Number;
}
