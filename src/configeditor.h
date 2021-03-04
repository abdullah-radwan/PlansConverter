#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

#include <QSettings>
#include <QSize>
#include <QPoint>

class ConfigEditor
{
public:
    ConfigEditor(QString dirPath);

    struct FormatData
    {
        bool fplSelected;
        bool plnSelected;
        bool fmsSelected;

        QString fplFolder;
        QString plnFolder;
        QString fmsFolder;
    };

    struct Config
    {
        bool typeIFR;
        int altitude;
        bool deleteInput;
        bool checkUpdates;

        FormatData inputData;
        FormatData outputData;

        QSize mainSize;
        QPoint mainPos;
    } config;

    void readConfig();
    void writeConfig();

private:
    QSettings settings;
};

#endif // CONFIGEDITOR_H
