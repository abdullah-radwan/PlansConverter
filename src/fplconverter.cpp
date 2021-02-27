#include "fplconverter.h"

#include <QDateTime>

FplConverter::FplConverter(FlightPlan* flightPlan) : flightPlan(flightPlan) { }

bool FplConverter::readFile(QString path)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.toStdString().c_str());

    if (result.status != pugi::status_ok)
    {
        qDebug() << "Couldn't read the FPL file:" << result.description();
        return false;
    }

    for (const auto& fplWaypoint : doc.child("flight-plan").child("waypoint-table").children("waypoint"))
    {
        Waypoint waypoint;

        waypoint.ident = fplWaypoint.child_value("identifier");
        waypoint.type = convertToType(fplWaypoint.child_value("type"));
        waypoint.countryCode = fplWaypoint.child_value("country-code");
        waypoint.lat = QString(fplWaypoint.child_value("lat")).toDouble();
        waypoint.lon = QString(fplWaypoint.child_value("lon")).toDouble();

        flightPlan->waypoints.append(waypoint);
    }

    if (flightPlan->waypoints.size() < 2)
    {
        qDebug() << "The FPL file is invalid";
        return false;
    }

    Waypoint& departure = flightPlan->waypoints.first();
    departure.depOrDest = true;
    flightPlan->departure = departure.ident;

    Waypoint& destination = flightPlan->waypoints.last();
    destination.depOrDest = true;
    flightPlan->destination = destination.ident;

    return true;
}

Converter::WaypointType FplConverter::convertToType(QString type)
{
    if (type == "AIRPORT") return Airport;

    if (type == "NDB") return NDB;

    if (type == "VOR") return VOR;

    if (type == "INT") return Intersection;

    return User;
}

bool FplConverter::writeFile(QString path)
{
    pugi::xml_document doc;

    auto root = doc.append_child("flight-plan");

    root.append_attribute("xmlns").set_value("http://www8.garmin.com/xmlschemas/FlightPlan/v1");

    root.append_child("created").append_child(pugi::node_pcdata).set_value(QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toStdString().c_str());

    auto waypointsNode = root.append_child("waypoint-table");

    for (auto& waypoint : flightPlan->waypoints)
    {
        auto waypointNode = waypointsNode.append_child("waypoint");

       waypointNode.append_child("identifier").append_child(pugi::node_pcdata).set_value(waypoint.ident.toStdString().c_str());

        waypointNode.append_child("type").append_child(pugi::node_pcdata).set_value(convertFromType(waypoint.type).toStdString().c_str());

        if (!waypoint.countryCode.isEmpty())
            waypointNode.append_child("country-code").append_child(pugi::node_pcdata).set_value(waypoint.countryCode.toStdString().c_str());

        waypointNode.append_child("lat").append_child(pugi::node_pcdata).set_value(QString::number(waypoint.lat).toStdString().c_str());

        waypointNode.append_child("lon").append_child(pugi::node_pcdata).set_value(QString::number(waypoint.lon).toStdString().c_str());

        waypointNode.append_child("comment");
    }

    auto routeNode = root.append_child("route");

    QString name = flightPlan->departure + " to " + flightPlan->destination;

    routeNode.append_child("route-name").append_child(pugi::node_pcdata).set_value(name.toStdString().c_str());

    routeNode.append_child("flight-plan-index").append_child(pugi::node_pcdata).set_value("1");

    for (auto& waypoint : flightPlan->waypoints)
    {
        auto waypointNode = routeNode.append_child("route-point");

        waypointNode.append_child("waypoint-identifier").append_child(pugi::node_pcdata).set_value(waypoint.ident.toStdString().c_str());

        waypointNode.append_child("waypoint-type").append_child(pugi::node_pcdata).set_value(convertFromType(waypoint.type).toStdString().c_str());

        if (waypoint.countryCode.isEmpty()) waypointNode.append_child("waypoint-country-code");
        else
            waypointNode.append_child("waypoint-country-code").append_child(pugi::node_pcdata).set_value(waypoint.countryCode.toStdString().c_str());
    }

    return doc.save_file(path.toStdString().c_str());
}

QString FplConverter::convertFromType(WaypointType type)
{
    switch (type)
    {
    case Airport:
        return "AIRPORT";

    case NDB:
        return "NDB";

    case VOR:
        return "VOR";

    case Intersection:
        return "INT";

    default:
        return "USER WAYPOINT";
    }
}
