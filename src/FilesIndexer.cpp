#include "FilesIndexer.h"

#include "ILinePositionStorage.h"
#include "ILineSelector.h"

#include <QDataStream>
#include <QFile>
#include <QTextStream>

#include <vector>

namespace {

inline bool IsEndOfLineSymbol(char const ch)
{
    return ((ch == '\n') || (ch == '\r'));
}

} // namespace

FilesIndexer::FilesIndexer(ILinePositionStorage& linePositionStorage, const ILineSelector& lineSelector) :
    m_linePositionStorage(linePositionStorage),
    m_lineSelector(lineSelector),
    m_fileIndex(0)
{
}

void FilesIndexer::AddFileIndexes(const QString& filename)
{
    QFile binfile(filename);

    std::size_t const bufferSize = 1 * 1024 * 1024;
    std::vector<char> buffer(bufferSize + 1);
    char* bufferData = buffer.data();
    if (binfile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&binfile);
        std::size_t bufferStartOffset = 0;

        while (!in.atEnd())
        {
            std::size_t const readBytesCount = in.readRawData(bufferData, bufferSize);

            bool lineAdded = false;

            std::size_t previousEol = 0;
            std::size_t currentEol = 0;
            for (std::size_t i = 0; i < readBytesCount; ++i)
            {
                if (IsEndOfLineSymbol(bufferData[i]))
                {
                    currentEol = i;
                    ++i;
                    while (IsEndOfLineSymbol(bufferData[i]) && i < readBytesCount) { ++i; }

                    std::string const line(bufferData + previousEol, bufferData + currentEol);

                    if (m_lineSelector.LineShouldBeSelected(line))
                    {
                        lineAdded =  true;
                        LinePosition const pos(static_cast<FileOffset>(bufferStartOffset + previousEol), m_fileIndex);
                        m_linePositionStorage.AddPosition(pos);
                    }

                    previousEol = i;
                }
            }

            if (lineAdded && !in.atEnd())
            {
                binfile.seek(m_linePositionStorage[m_linePositionStorage.Size() - 1].Offset);
            }
            bufferStartOffset += readBytesCount;
        }

        binfile.close();

        ++m_fileIndex;
    }
}

void FilesIndexer::AddFilesIndexes(const QStringList& filenames)
{
    for (QString const& filename : filenames)
    {
        AddFileIndexes(filename);
    }
}
