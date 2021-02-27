#ifndef FMSCONVERTER_H
#define FMSCONVERTER_H

#include "converter.h"

class FmsConverter : public Converter
{
public:
    FmsConverter(FlightPlan* flightPlan);

    bool readFile(QString path) override;

    bool writeFile(QString path) override;

private:
    FlightPlan* flightPlan;

    WaypointType convertToType(int type);
    int convertFromType(WaypointType type);

    QString getCycle();
};

#endif // FMSCONVERTER_H
