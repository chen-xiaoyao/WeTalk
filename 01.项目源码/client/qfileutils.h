#ifndef QFILEUTILS_H
#define QFILEUTILS_H

#include "QObject"
#include "QFile"

class QFileUtils: public QObject {
    Q_OBJECT
public:
    QFileUtils(){}
    Q_INVOKABLE qint64 getFileSize(QString filePath){
        QFile* file = new QFile(filePath);
        return file->size();
    }
};

#endif // QFILEUTILS_H
