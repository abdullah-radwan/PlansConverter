#include "fmsconverter.h"

#include <QFile>
#include <QDate>

FmsConverter::FmsConverter(FlightPlan* flightPlan) : flightPlan(flightPlan) { }

bool FmsConverter::readFile(QString path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Couldn't open the FMS file";
        return false;
    }

    QTextStream fileStream(&file);

    bool atNumEnr = false;

    while (!fileStream.atEnd())
    {
        QString line = fileStream.readLine();

        if (atNumEnr)
        {
            QStringList fmsWaypoint = line.split(" ");

            Waypoint waypoint;

            waypoint.type = convertToType(fmsWaypoint[0].toInt());

            waypoint.ident = fmsWaypoint[1];

            if (fmsWaypoint[2] != "DRCT" && fmsWaypoint[2] != "ADEP" && fmsWaypoint[2] != "ADES") waypoint.airway = fmsWaypoint[2];

            waypoint.alt = fmsWaypoint[3].toDouble();

            waypoint.lat = fmsWaypoint[4].toDouble();

            waypoint.lon = fmsWaypoint[5].toDouble();

            flightPlan->waypoints.append(waypoint);
        }
        else if (line.startsWith("ADEP")) flightPlan->departure = line.split(" ").last();
        else if (line.startsWith("ADES")) flightPlan->destination = line.split(" ").last();
        else if (line.startsWith("NUMENR")) atNumEnr = true;
    }

    file.close();

    if (flightPlan->waypoints.size() < 2 || flightPlan->departure.isEmpty() || flightPlan->destination.isEmpty())
    {
        qDebug() << "The FMS file is invalid";
        return false;
    }

    flightPlan->waypoints.first().depOrDest = true;
    flightPlan->waypoints.last().depOrDest = true;

    return true;
}

Converter::WaypointType FmsConverter::convertToType(int type)
{
    switch (type)
    {
    case 1:
        return Converter::Airport;

    case 2:
        return Converter::NDB;

    case 3:
        return Converter::VOR;

    case 11:
        return Converter::Intersection;

    default:
        return Converter::User;
    }
}

bool FmsConverter::writeFile(QString path)
{
    qDebug() << "Writing into" << path;
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Couldn't open the FMS file";
        return false;
    }

    QTextStream out(&file);
    out << "I" << Qt::endl;
    out << "1100 Version" << Qt::endl;
    out << "CYCLE " << getCycle() << Qt::endl;
    out << "ADEP " << flightPlan->waypoints.first().ident << Qt::endl;
    out << "ADES " << flightPlan->waypoints.last().ident << Qt::endl;
    out << "NUMENR " << flightPlan->waypoints.size() << Qt::endl;

    for (auto& waypoint : flightPlan->waypoints)
    {
        QString airway;

        if (waypoint.depOrDest)
        {
            if (waypoint.ident == flightPlan->departure) airway = "ADEP";
            else airway = "ADES";
        }
        else airway = (waypoint.airway.isEmpty() ? "DRCT" : waypoint.airway);

        out << convertFromType(waypoint.type) << " " << waypoint.ident << " " << airway << " "
            << waypoint.alt << " " << waypoint.lat << " " << waypoint.lon << Qt::endl;
    }

    file.close();

    qDebug() << "The file converted was successfully";

    return true;
}

int FmsConverter::convertFromType(WaypointType type)
{
    switch (type)
    {
    case Airport:
        return 1;

    case NDB:
        return 2;

    case VOR:
        return 3;

    case Intersection:
        return 11;

    default:
        return 28;
    }
}

QString FmsConverter::getCycle()
{
    QDate currentDate = QDate::currentDate();
    QDate date = QDate::fromString("2021/02/25", "yyyy/MM/dd");

    int cycle = 2;
    int year = date.year();

    QMap<QDate, int> cycleMap;
    cycleMap.insert(date, cycle);

    while (true)
    {
        date = date.addDays(28);

        if (currentDate.daysTo(date) >= 0)
        {
            QString lastCycle;

            if (cycleMap.last() < 10) lastCycle = "0" + QString::number(cycleMap.last());
            else lastCycle = QString::number(cycleMap.last());

            QString sCycle = QString::number(cycleMap.lastKey().year()).remove(0, 2) + lastCycle;
            qDebug() << "Cycle:" << sCycle;
            return sCycle;
        }

        if (date.year() > year)
        {
            cycleMap.clear();
            cycle = 0;
            year = date.year();
        }

        cycle++;
        cycleMap.insert(date, cycle);
    }
}
