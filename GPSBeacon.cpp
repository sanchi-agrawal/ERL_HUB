#include "GPSBeacon.h"
#include <sstream>
std::string GPSBeacon::location() const {
    std::ostringstream out;
    out << "(" << latitude << ", " << longitude << ")";
    return out.str();
}
