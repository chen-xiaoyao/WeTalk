#ifndef QREADINI_H
#define QREADINI_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <QObject>
#include <QObject>

class QSettingIni: public QObject
{
    Q_OBJECT
public:
    QSettingIni(QString fileName){
        qSettings = new QSettings(fileName, QSettings::IniFormat);
    }

    Q_INVOKABLE void write(QString key, QVariant value){
        qSettings->setValue(key, value);
    };

    Q_INVOKABLE QVariant read(QString key, QVariant defaultValue){
        return qSettings->value(key, defaultValue);
    };

private:
    QSettings* qSettings;
};


#endif // QREADINI_H
