#ifndef LINEPOSITION_H
#define LINEPOSITION_H

#include <QString>

typedef quint64 LineNumber;

typedef quint64 FileOffset;
typedef quint8 FileIndex;

class LinePosition
{
public:
    FileOffset Offset;
    FileIndex Index;
    LineNumber NumberInFile;
    LineNumber NumberInMatchedLines;
    LineNumber GlobalLineNumber;
    bool Matched;

public:
    LinePosition();
    LinePosition(FileOffset fileOffset, FileIndex fileId, LineNumber numberInMatchedLines, LineNumber numberInFile, const bool matched = false);
};

#endif // LINEPOSITION_H
