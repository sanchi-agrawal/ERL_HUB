#ifndef AERIALTRANSPORT_H
#define AERIALTRANSPORT_H
#include "ReliefVehicle.h"
class AerialTransport : public virtual ReliefVehicle {
public:
    AerialTransport(const std::string&,const std::string&,double,double=100);
    std::string type() const override{return "Aerial Transport";}
    double routeMultiplier() const override{return 0.8;}
    bool isMedicalCapable() const override{return true;}
};
#endif
