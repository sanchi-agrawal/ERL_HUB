#ifndef GPSBEACON_H
#define GPSBEACON_H
#include <string>
class GPSBeacon {
    double latitude{}, longitude{};
    bool active{true};
public:
    GPSBeacon() = default;
    GPSBeacon(double lat,double lon): latitude(lat),longitude(lon) {}
    void update(double lat,double lon){ latitude=lat; longitude=lon; }
    std::string location() const;
    bool isActive() const { return active; }
};
#endif
