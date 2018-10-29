#include "Utils.h"

#include <QFile>
#include <QTextStream>

#include <stdexcept>

QString LoadFileToQString(const QString& filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        throw std::runtime_error(std::string("Can't open file ") + filename.toStdString() + " for reading");
    }

    QTextStream in(&f);
    const QString data = in.readAll();

    return data;
}

void SaveQStringToFile(const QString& str, const QString& filename)
{
    QFile f(filename);
    if (!f.open(QFile::WriteOnly | QFile::Text))
    {
        throw std::runtime_error(std::string("Can't open file ") + filename.toStdString() + " for writing");
    }

    QTextStream out(&f);
    out << str;
}
