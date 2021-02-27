#include "converterop.h"

#include <QFile>
#include <QDebug>

ConverterOp::ConverterOp() : fplConverter(&flightPlan), plnConverter(&flightPlan), fmsConverter(&flightPlan) { }

void ConverterOp::convert()
{
    emit convertingFile();

    flightPlan = Converter::FlightPlan();

    qDebug() << "Reading" << data.inputPath;

    bool success = false;

    switch (data.inputFormat)
    {
    case FPL:
        success = fplConverter.readFile(data.inputPath);
        break;

    case PLN:
        success = plnConverter.readFile(data.inputPath);
        break;

    case FMS:
        success = fmsConverter.readFile(data.inputPath);
        break;
    }

    if (!success)
    {
        qDebug() << "Couldn't read the input file";
        emit error("couldn't read the input file.");

        return;
    }

    if (data.altitude) setAltitude();

    if (!data.route.isEmpty()) if (!setRoute()) return;

    qDebug() << "Writing into" << data.outputPath;

    switch (data.outputFormat)
    {
    case FPL:
        success = fplConverter.writeFile(data.outputPath);
        break;

    case PLN:
        plnConverter.typeIFR = data.typeIFR;
        plnConverter.routeType = data.routeType;
        plnConverter.cruizeAlt = data.altitude;

        success = plnConverter.writeFile(data.outputPath);
        break;

    case FMS:
        success = fmsConverter.writeFile(data.outputPath);
        break;
    }

    if (!success)
    {
        qDebug() << "Couldn't write the output file";
        emit error("couldn't write the output file.");

        return;
    }

    if (data.deleteOutput)
    {
        if (QFile(data.inputPath).remove()) qDebug() << "The input file was deleted successfully";
        else qDebug() << "Couldn't delete the input file";
    }

    qDebug() << "The input file converted was successfully";

    emit done();
}

void ConverterOp::setAltitude()
{
    for (auto& waypoint : flightPlan.waypoints)
        if (!waypoint.depOrDest) waypoint.alt = data.altitude;
}

bool ConverterOp::setRoute()
{
    QStringList route = data.route.split(" ");

    for (auto& waypoint : route)
        if (waypoint.isEmpty()) route.removeOne(waypoint);

    qDebug() << "Route:" << route;

    for (auto& waypoint : flightPlan.waypoints)
    {
        if (waypoint.depOrDest) continue;

        if (waypoint.ident == route.first()) { route.removeFirst(); continue; }

        if (route.size() < 2)
        {
            qDebug() << "Invalid route" << route;
            emit error("the flight route is invalid.");

            return false;
        }

        if (waypoint.ident == route.at(1))
        {
            waypoint.airway = route.first();

            route.removeFirst();
            route.removeFirst();

            continue;
        }

        waypoint.airway = route.first();
    }

    return true;
}
