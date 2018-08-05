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

public:
    LinePosition();
    LinePosition(FileOffset fileOffset, FileIndex fileId);
};

#endif // LINEPOSITION_H
