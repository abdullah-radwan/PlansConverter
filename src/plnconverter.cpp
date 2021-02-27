#include "plnconverter.h"

PlnConverter::PlnConverter(FlightPlan* flightPlan) : flightPlan(flightPlan) { }

bool PlnConverter::readFile(QString path)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.toStdString().c_str());

    if (result.status != pugi::status_ok)
    {
        qDebug() << "Couldn't read the PLN file:" << result.description();
        return false;
    }

    auto planNode = doc.child("SimBase.Document").child("FlightPlan.FlightPlan");

    flightPlan->departure = planNode.child_value("DepartureID");

    flightPlan->destination = planNode.child_value("DestinationID");

    for (const auto& plnWaypoint : planNode.children("ATCWaypoint"))
    {
        Waypoint waypoint;

        waypoint.ident = plnWaypoint.attribute("id").value();

        waypoint.type = convertToType(QString(plnWaypoint.child_value("ATCWaypointType")).toUpper());

        convertLLAToWaypoint(waypoint, plnWaypoint.child_value("WorldPosition"));

        waypoint.airway = plnWaypoint.child_value("ATCAirway");

        waypoint.countryCode = plnWaypoint.child("ICAO").child_value("ICAORegion");

        flightPlan->waypoints.append(waypoint);
    }

    if (flightPlan->waypoints.size() < 2 || flightPlan->departure.isEmpty() || flightPlan->destination.isEmpty())
    {
        qDebug() << "The PLN file is invalid";
        return false;
    }

    flightPlan->waypoints.first().depOrDest = true;
    flightPlan->waypoints.last().depOrDest = true;

    return true;
}

Converter::WaypointType PlnConverter::convertToType(QString type)
{
    if (type == "AIRPORT") return Airport;

    if (type == "NDB") return NDB;

    if (type == "VOR") return VOR;

    if (type == "INTERSECTION") return Intersection;

    return User;
}

void PlnConverter::convertLLAToWaypoint(Waypoint& waypoint, QString lla)
{
    QStringList split = lla.split(",");

    waypoint.lat = convertPDMSToDecimal(split[0]);
    waypoint.lon = convertPDMSToDecimal(split[1]);
    waypoint.alt = split[2].toDouble();
}

double PlnConverter::convertPDMSToDecimal(QString pdms)
{
    QStringList split = pdms.split(" ");

    int d = split[0].chopped(1).remove(0, 1).toInt();
    int m = split[1].chopped(1).toInt();
    double s = split[2].chopped(1).toDouble();

    double decimal = d + (m / 60.0) + (s / 3600);

    QChar p = pdms[0].toUpper();

    if (p == 'S' || p == 'W') return -decimal;

    return decimal;
}

bool PlnConverter::writeFile(QString path)
{
    pugi::xml_document doc;

    auto root = doc.append_child("SimBase.Document");

    root.append_attribute("Type").set_value("AceXML");
    root.append_attribute("version").set_value("1,0");

    root.append_child("Descr").append_child(pugi::node_pcdata).set_value("AceXML Document");

    auto flightNode = root.append_child("FlightPlan.FlightPlan");

    QString title = flightPlan->departure + " to " + flightPlan->destination;
    flightNode.append_child("Title").append_child(pugi::node_pcdata).set_value(title.toStdString().c_str());
    flightNode.append_child("Descr").append_child(pugi::node_pcdata).set_value(title.toStdString().c_str());

    flightNode.append_child("FPType").append_child(pugi::node_pcdata).set_value(typeIFR ? "IFR" : "VFR");

    if (!routeType.isEmpty()) flightNode.append_child("RouteType").append_child(pugi::node_pcdata).set_value(routeType.toStdString().c_str());

    flightNode.append_child("CruisingAlt").append_child(pugi::node_pcdata).set_value(QString::number(int(cruizeAlt)).toStdString().c_str());

    flightNode.append_child("DepartureName").append_child(pugi::node_pcdata).set_value(flightPlan->departure.toStdString().c_str());
    flightNode.append_child("DepartureID").append_child(pugi::node_pcdata).set_value(flightPlan->departure.toStdString().c_str());

    Waypoint& departureWaypoint = flightPlan->waypoints.first();
    QString departureLLA = convertWaypointToLLA(departureWaypoint);
    flightNode.append_child("DepartureLLA").append_child(pugi::node_pcdata).set_value(departureLLA.toStdString().c_str());

    flightNode.append_child("DestinationName").append_child(pugi::node_pcdata).set_value(flightPlan->destination.toStdString().c_str());
    flightNode.append_child("DestinationID").append_child(pugi::node_pcdata).set_value(flightPlan->destination.toStdString().c_str());

    Waypoint& destinationWaypoint = flightPlan->waypoints.last();
    QString destinationLLA = convertWaypointToLLA(destinationWaypoint);
    flightNode.append_child("DestinationLLA").append_child(pugi::node_pcdata).set_value(destinationLLA.toStdString().c_str());

    auto versionNode = flightNode.append_child("AppVersion");

    versionNode.append_child("AppVersionMajor").append_child(pugi::node_pcdata).set_value("10");
    versionNode.append_child("AppVersionBuild").append_child(pugi::node_pcdata).set_value("61472");

    for (auto& waypoint : flightPlan->waypoints)
    {
        auto waypointNode = flightNode.append_child("ATCWaypoint");

        waypointNode.append_attribute("id").set_value(waypoint.ident.toStdString().c_str());

        waypointNode.append_child("ATCWaypointType").append_child(pugi::node_pcdata).set_value(convertFromType(waypoint.type).toStdString().c_str());

        QString waypointLLA = convertWaypointToLLA(waypoint);
        waypointNode.append_child("WorldPosition").append_child(pugi::node_pcdata).set_value(waypointLLA.toStdString().c_str());

        if (!waypoint.airway.isEmpty())
            waypointNode.append_child("ATCAirway").append_child(pugi::node_pcdata).set_value(waypoint.airway.toStdString().c_str());

        auto icaoNode = waypointNode.append_child("ICAO");

        if (!waypoint.depOrDest && waypoint.type != Airport && waypoint.type != User && !waypoint.countryCode.isEmpty())
            icaoNode.append_child("ICAORegion").append_child(pugi::node_pcdata).set_value(waypoint.countryCode.toStdString().c_str());

        icaoNode.append_child("ICAOIdent").append_child(pugi::node_pcdata).set_value(waypoint.ident.toStdString().c_str());
    }

    return doc.save_file(path.toStdString().c_str());
}

QString PlnConverter::convertFromType(WaypointType type)
{
    switch (type)
    {
    case Airport:
        return "Airport";

    case NDB:
        return "NDB";

    case VOR:
        return "VOR";

    case Intersection:
        return "Intersection";

    default:
        return "User";
    }
}

QString PlnConverter::convertWaypointToLLA(Waypoint& waypoint)
{ 
    return convertDecimalToPDMS(waypoint.lat, true) + "," + convertDecimalToPDMS(waypoint.lon, false) + ",+" + QString::number(waypoint.alt, 'f', 2);
}

QString PlnConverter::convertDecimalToPDMS(double decimal, bool latitude)
{
    double absDegrees = abs(decimal);

    char p = latitude ? (decimal < 0 ? 'S' : 'N') : (decimal < 0 ? 'W' : 'E');

    double d = floor(absDegrees);

    double m = (absDegrees - d) * 60;

    double s = (m - floor(m)) * 60;

    QString pdms = "%1%2° %3' %4\"";

    return pdms.arg(p).arg(d).arg(floor(m)).arg(QString::number(s, 'f', 2));;
}
