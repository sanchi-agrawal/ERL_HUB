#ifndef TRANSPORTVEHICLE_H
#define TRANSPORTVEHICLE_H
#include "ReliefCargo.h"
#include <cstddef>

class TransportVehicle {
protected:
    // Explicit dynamic memory is used for the transport manifest as required by Unit II.
    ReliefOps::Inventory::ReliefCargo* manifest;
    std::size_t manifestCount;
    std::size_t manifestCapacity;
    void resizeManifest(std::size_t newCapacity);
public:
    TransportVehicle();
    TransportVehicle(const TransportVehicle& other);
    TransportVehicle& operator=(const TransportVehicle& other);
    virtual ~TransportVehicle();
    void addToManifest(const ReliefOps::Inventory::ReliefCargo& c);
    double manifestWeight() const;
};
#endif
