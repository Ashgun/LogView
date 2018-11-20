#include "FilesIndexer.h"

#include "Events.h"
#include "ILinePositionStorage.h"
#include "IPositionedLinesStorage.h"

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

FilesIndexer::FilesIndexer(ILinePositionStorage& linePositionStorage, IPositionedLinesStorage& linesStorage,
                           const EventPatternsHierarchyMatcher& lineSelector) :
    m_linePositionStorage(linePositionStorage),
    m_lineSelector(lineSelector),
    m_linesStorage(linesStorage),
    m_fileIndex(0)
{
}

void FilesIndexer::AddFileIndexes(const QString& filename)
{
    QFile binfile(filename);

    std::size_t const bufferSize = 10 * 1024 * 1024;
    std::vector<char> buffer(bufferSize + 1);
    char* bufferData = buffer.data();
    if (binfile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&binfile);
        std::size_t bufferStartOffset = 0;

        while (!in.atEnd())
        {
            quint64 const readBytesCount = static_cast<quint64>(in.readRawData(bufferData, bufferSize));

            bool lineAdded = false;

            quint64 previousEol = 0;
            quint64 currentEol = 0;
            quint64 currentLineNumber = 0;
            for (quint64 i = 0; i < readBytesCount; ++i)
            {
                if (IsEndOfLineSymbol(bufferData[i]))
                {
                    currentEol = i;
                    ++i;
                    while (IsEndOfLineSymbol(bufferData[i]) && i < readBytesCount) { ++i; }

                    EventPattern::PatternString const line =
                            EventPattern::PatternString::fromStdString(
                                std::string(bufferData + previousEol, bufferData + currentEol));

                    LinePosition const pos(
                                static_cast<FileOffset>(bufferStartOffset + previousEol),
                                m_fileIndex, m_linePositionStorage.Size(), currentLineNumber++);
                    m_linePositionStorage.AddPosition(pos);

                    int const hierarchyLevel = m_lineSelector.GetLevelInHierarchy(line);
                    if (hierarchyLevel >= 0)
                    {
                        m_linesStorage.AddLine(line, pos, hierarchyLevel);
                        lineAdded = true;
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
