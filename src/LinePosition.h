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
    LineNumber Number;

public:
    LinePosition();
    LinePosition(FileOffset fileOffset, FileIndex fileId, LineNumber number);
    bool operator==(LinePosition const& other) const;
};

#endif // LINEPOSITION_H
