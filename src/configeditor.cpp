#include "configeditor.h"

#include <QDate>
#include <QDebug>

ConfigEditor::ConfigEditor(QString dirPath) : settings(dirPath + "/config.cfg", QSettings::IniFormat) { }

void ConfigEditor::readConfig()
{
    qDebug() << "Reading config";

    config.typeIFR = settings.value("TypeIFR", true).toBool();
    config.altitude = settings.value("Altitude", 0).toInt();
    config.deleteOutput = settings.value("DeleteOutput", true).toBool();
    config.checkUpdates = settings.value("CheckDate", 0).toDate().addDays(14) <= QDate::currentDate();

    settings.beginGroup("InputData");
    config.inputData.fplSelected = settings.value("FplSelected", true).toBool();
    config.inputData.plnSelected = settings.value("PlnSelected", false).toBool();
    config.inputData.fmsSelected = settings.value("FmsSelected", false).toBool();
    config.inputData.fplFolder = settings.value("FplFolder").toString();
    config.inputData.plnFolder = settings.value("PlnFolder").toString();
    config.inputData.fmsFolder = settings.value("FmsFolder").toString();
    settings.endGroup();

    settings.beginGroup("OutputData");
    config.outputData.fplSelected = settings.value("FplSelected", false).toBool();
    config.outputData.plnSelected = settings.value("PlnSelected", true).toBool();
    config.outputData.fmsSelected = settings.value("FmsSelected", false).toBool();
    config.outputData.fplFolder = settings.value("FplFolder").toString();
    config.outputData.plnFolder = settings.value("PlnFolder").toString();
    config.outputData.fmsFolder = settings.value("FmsFolder").toString();
    settings.endGroup();

    settings.beginGroup("MainWindow");
    config.mainSize = settings.value("Size", QSize(-1, -1)).toSize();
    config.mainPos = settings.value("Pos", QPoint(-1, -1)).toPoint();
    settings.endGroup();

    qDebug() << "Config was read";
}

void ConfigEditor::writeConfig()
{
    qDebug() << "Writing config";

    settings.setValue("TypeIFR", config.typeIFR);
    settings.setValue("Altitude", config.altitude);
    settings.setValue("DeleteOutput", config.deleteOutput);
    if (config.checkUpdates || !settings.value("CheckDate", 0).toDate().isValid()) settings.setValue("CheckDate", QDate::currentDate());

    settings.beginGroup("InputData");
    settings.setValue("FplSelected", config.inputData.fplSelected);
    settings.setValue("PlnSelected", config.inputData.plnSelected);
    settings.setValue("FmsSelected", config.inputData.fmsSelected);
    settings.setValue("FplFolder", config.inputData.fplFolder);
    settings.setValue("PlnFolder", config.inputData.plnFolder);
    settings.setValue("FmsFolder", config.inputData.fmsFolder);
    settings.endGroup();

    settings.beginGroup("OutputData");
    settings.setValue("FplSelected", config.outputData.fplSelected);
    settings.setValue("PlnSelected", config.outputData.plnSelected);
    settings.setValue("FmsSelected", config.outputData.fmsSelected);
    settings.setValue("FplFolder", config.outputData.fplFolder);
    settings.setValue("PlnFolder", config.outputData.plnFolder);
    settings.setValue("FmsFolder", config.outputData.fmsFolder);
    settings.endGroup();

    settings.beginGroup("MainWindow");
    settings.setValue("Size", config.mainSize);
    settings.setValue("Pos", config.mainPos);
    settings.endGroup();

    qDebug() << "Config was written";
}
