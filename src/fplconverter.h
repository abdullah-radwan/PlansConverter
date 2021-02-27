#ifndef FPLCONVERTER_H
#define FPLCONVERTER_H

#include "converter.h"

class FplConverter : public Converter
{
public:
    FplConverter(FlightPlan* flightPlan);

    bool readFile(QString path) override;

    bool writeFile(QString path) override;

private:
    FlightPlan* flightPlan;

    WaypointType convertToType(QString type);
    QString convertFromType(WaypointType type);
};

#endif // FPLCONVERTER_H
