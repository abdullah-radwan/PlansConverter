#ifndef PLNCONVERTER_H
#define PLNCONVERTER_H

#include "converter.h"

class PlnConverter : public Converter
{
public:
    bool typeIFR;
    QString routeType;
    double cruizeAlt;

    PlnConverter(FlightPlan* flightPlan);

    bool readFile(QString path) override;

    bool writeFile(QString path) override;

private:
    FlightPlan* flightPlan;

    WaypointType convertToType(QString type);
    void convertLLAToWaypoint(Waypoint& waypoint, QString lla);
    double convertPDMSToDecimal(QString dms);

    QString convertFromType(WaypointType type);
    QString convertWaypointToLLA(Waypoint& waypoint);
    QString convertDecimalToPDMS(double decimal, bool latitude);
};

#endif // PLNCONVERTER_H
