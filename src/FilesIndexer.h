#ifndef FILESINDEXER_H
#define FILESINDEXER_H

#include "LinePosition.h"

#include <QString>
#include <QStringList>

class EventsHierarchyMatcher;
class ILinePositionStorage;
class IPositionedLinesStorage;

class FilesIndexer
{
public:
    FilesIndexer(ILinePositionStorage& linePositionStorage, IPositionedLinesStorage& linesStorage,
                 EventsHierarchyMatcher const& lineSelector);

    void AddFileIndexes(QString const& filename);
    void AddFilesIndexes(QStringList const& filenames);

private:
    ILinePositionStorage& m_linePositionStorage;
    EventsHierarchyMatcher const& m_lineSelector;
    IPositionedLinesStorage& m_linesStorage;
    FileIndex m_fileIndex;
};

#endif // FILESINDEXER_H
