#include "LinePosition.h"

LinePosition::LinePosition() : LinePosition(0, 0)
{
}

LinePosition::LinePosition(FileOffset fileOffset, FileIndex fileId) :
    Offset(fileOffset),
    Index(fileId)
{
}

bool LinePosition::operator==(const LinePosition& other) const
{
    return Offset == other.Offset && Index == other.Index;
}
