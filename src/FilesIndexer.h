#ifndef FILESINDEXER_H
#define FILESINDEXER_H

#include "LinePosition.h"

#include <QString>
#include <QStringList>

class EventPatternsHierarchyMatcher;
class ILinePositionStorage;
class IPositionedLinesStorage;

class FilesIndexer
{
public:
    FilesIndexer(ILinePositionStorage& linePositionStorage, IPositionedLinesStorage& linesStorage,
                 EventPatternsHierarchyMatcher const& lineSelector);

    void AddFileIndexes(QString const& filename);
    void AddFilesIndexes(QStringList const& filenames);

private:
    ILinePositionStorage& m_linePositionStorage;
    EventPatternsHierarchyMatcher const& m_lineSelector;
    IPositionedLinesStorage& m_linesStorage;
    FileIndex m_fileIndex;
};

#endif // FILESINDEXER_H
