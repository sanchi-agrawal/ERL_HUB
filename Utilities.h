#ifndef UTILITIES_H
#define UTILITIES_H
#include <cmath>
#include <iostream>
#include <string>

namespace ReliefOps {
namespace Logistics {
inline double calculateDistance(double x1, double y1, double x2, double y2) {
    return std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}
inline double payloadPercentage(double used, double capacity) {
    return capacity <= 0 ? 0.0 : (used / capacity) * 100.0;
}
}
namespace Inventory {
inline void printLine(char c='-', int n=72) { std::cout << std::string(n,c) << '\n'; }
}
}
#endif
