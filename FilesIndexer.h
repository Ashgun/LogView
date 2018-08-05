#ifndef FILESINDEXER_H
#define FILESINDEXER_H

#include "ILinePositionStorage.h"

#include "LinePosition.h"

#include <QString>
#include <QStringList>

class FilesIndexer
{
public:
    FilesIndexer(ILinePositionStorage& linePositionStorage);

    void AddFileIndexes(QString const& filename);
    void AddFilesIndexes(QStringList const& filenames);

private:
    ILinePositionStorage& m_linePositionStorage;
    FileIndex m_fileIndex;
};

#endif // FILESINDEXER_H
