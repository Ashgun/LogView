#ifndef LINEPOSITION_H
#define LINEPOSITION_H

#include <QString>

typedef quint32 LineNumber;

typedef quint64 FileOffset;
typedef quint8 FileIndex;

class LinePosition
{
public:
    FileOffset Offset;
    FileIndex Index;
    LineNumber NumberInFile;
    LineNumber NumberInMatchedLines;
    bool Matched;

public:
    LinePosition();
    LinePosition(FileOffset fileOffset, FileIndex fileId, LineNumber numberInMatchedLines, LineNumber numberInFile, const bool matched = false);
    bool operator==(LinePosition const& other) const;
};

#endif // LINEPOSITION_H
