#include "FilesIndexer.h"

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

FilesIndexer::FilesIndexer(ILinePositionStorage& linePositionStorage) :
    m_linePositionStorage(linePositionStorage),
    m_fileIndex(0)
{

}
#include <QDebug>
//void FilesIndexer::AddFileIndexes(const QString& filename)
//{
//    QFile textfile(filename);
//    if (textfile.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        QTextStream in(&textfile);
//        LineNumber lineNumber(0);
//        while (!in.atEnd())
//        {
//            LinePosition pos(static_cast<FileOffset>(in.pos()), m_fileIndex);
//            m_linePositionStorage.AddPosition(pos);

//            in.readLine();

//            ++lineNumber;

//            qDebug() << lineNumber << /*pos.Offset*/0;
//        }
//        textfile.close();
//    }

//    ++m_fileIndex;
//}

void FilesIndexer::AddFileIndexes(const QString& filename)
{
    QFile binfile(filename);

    std::size_t const bufferSize = 1 * 1024 * 1024;
    std::vector<char> buffer(bufferSize + 1);
    char* bufferData = buffer.data();
    if (binfile.open(QIODevice::ReadOnly))
    {
        QDataStream in(&binfile);

        LinePosition const pos(static_cast<FileOffset>(0), m_fileIndex);
        m_linePositionStorage.AddPosition(pos);

        std::size_t bufferStartOffset = 0;

        while (!in.atEnd())
        {
            std::size_t const readBytesCount = in.readRawData(bufferData, bufferSize);

            std::size_t previousEol = 0;
            std::size_t currentEol = 0;
            for (std::size_t i = 0; i < readBytesCount; ++i)
            {
                if (IsEndOfLineSymbol(bufferData[i]))
                {
                    currentEol = i;
                    ++i;
                    while (IsEndOfLineSymbol(bufferData[i]) && i < readBytesCount) { ++i; }

                    if (m_linePositionStorage.Size() <= 5)
                    {
                        qDebug() << std::string(bufferData + previousEol, bufferData + currentEol).c_str();
                    }

                    LinePosition const pos(static_cast<FileOffset>(bufferStartOffset + i), m_fileIndex);
                    m_linePositionStorage.AddPosition(pos);

                    previousEol = i;
                }
            }

            binfile.seek(m_linePositionStorage[m_linePositionStorage.Size() - 1].Offset);
            bufferStartOffset += readBytesCount;
        }

        binfile.close();

        ++m_fileIndex;
    }
}

//void FilesIndexer::AddFileIndexes(const QString& filename)
//{
//    QFile inputFile(filename);
//    inputFile.open(QIODevice::ReadOnly);
//    if (!inputFile.isOpen())
//    {
//        return;
//    }

//    QTextStream stream(&inputFile);
//    QString line = stream.readLine();
//    while (!line.isNull())
//    {
//        LinePosition const pos(static_cast<FileOffset>(stream.pos()), m_fileIndex);
//        m_linePositionStorage.AddPosition(pos);

//        line = stream.readLine();
//        if (m_linePositionStorage.Size() < 5)
//        {
//            qDebug() << line;
//        }
//    }
//}

void FilesIndexer::AddFilesIndexes(const QStringList& filenames)
{
    for (QString const& filename : filenames)
    {
        AddFileIndexes(filename);
    }
}
