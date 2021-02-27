#ifndef CONVERTER_H
#define CONVERTER_H

#include "pugixml.hpp"

#include <QString>
#include <QList>
#include <QtMath>
#include <QDebug>

class Converter
{
public:
    enum WaypointType { Airport, NDB, VOR, Intersection, User };

    struct Waypoint
    {
        QString ident;
        WaypointType type;
        bool depOrDest = false;
        QString countryCode;
        double lat;
        double lon;
        double alt = 0;
        QString airway;
    };

    struct FlightPlan
    {
        QString departure;
        QString destination;
        QList<Waypoint> waypoints;
    };

    virtual bool readFile(QString path) = 0;

    virtual bool writeFile(QString path) = 0;
};

#endif // CONVERTER_H
