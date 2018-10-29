#ifndef UTILS_H
#define UTILS_H

#include <QString>

QString LoadFileToQString(const QString& filename);
void SaveQStringToFile(const QString& str, const QString& filename);

#endif // UTILS_H
