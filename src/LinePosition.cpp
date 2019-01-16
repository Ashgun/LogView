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
    GlobalLineNumber(NumberInMatchedLines),
    Matched(matched)
{
}
