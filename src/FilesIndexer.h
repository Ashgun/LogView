#ifndef FILESINDEXER_H
#define FILESINDEXER_H

#include "LinePosition.h"

#include <QString>
#include <QStringList>

class ILinePositionStorage;
class ILineSelector;

class FilesIndexer
{
public:
    FilesIndexer(ILinePositionStorage& linePositionStorage, ILineSelector const& lineSelector);

    void AddFileIndexes(QString const& filename);
    void AddFilesIndexes(QStringList const& filenames);

private:
    ILinePositionStorage& m_linePositionStorage;
    ILineSelector const& m_lineSelector;
    FileIndex m_fileIndex;
};

#endif // FILESINDEXER_H
