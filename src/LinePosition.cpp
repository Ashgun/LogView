#include "LinePosition.h"

LinePosition::LinePosition() : LinePosition(0, 0, 0, 0, false)
{
}

LinePosition::LinePosition(FileOffset fileOffset, FileIndex fileId, LineNumber numberInMatchedLines,
                           LineNumber numberInFile, const bool matched/* = false*/) :
    Offset(fileOffset),
    Index(fileId),
    NumberInFile(numberInFile),
    NumberInMatchedLines(numberInMatchedLines),
    Matched(matched)
{
}

bool LinePosition::operator==(const LinePosition& other) const
{
    return
            Offset == other.Offset &&
            Index == other.Index &&
            NumberInMatchedLines == other.NumberInMatchedLines &&
            NumberInFile == other.NumberInFile;
}
