#ifndef CONVERTEROP_H
#define CONVERTEROP_H

#include "converter.h"
#include "fplconverter.h"
#include "plnconverter.h"
#include "fmsconverter.h"

#include <QObject>

class ConverterOp : public QObject
{
    Q_OBJECT
public:
    enum ConvertFormat { FPL, PLN, FMS };

    struct Data
    {
        ConvertFormat inputFormat;
        QString inputPath;

        bool typeIFR;
        QString routeType;
        QString route;
        int altitude;

        ConvertFormat outputFormat;
        QString outputPath;

        bool deleteOutput;
    } data;

    Converter::FlightPlan flightPlan;

    ConverterOp();

signals:
    void convertingFile();
    void done();
    void error(QString error);

public slots:
    void convert();

private:
    FplConverter fplConverter;
    PlnConverter plnConverter;
    FmsConverter fmsConverter;

    void setAltitude();
    bool setRoute();
};

#endif // CONVERTEROP_H
