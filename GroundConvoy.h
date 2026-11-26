#ifndef GROUNDCONVOY_H
#define GROUNDCONVOY_H
#include "ReliefVehicle.h"
class GroundConvoy : public virtual ReliefVehicle {
public:
    GroundConvoy(const std::string&,const std::string&,double,double=100);
    std::string type() const override{return "Ground Convoy";}
    double routeMultiplier() const override{return 1.0;}
};
#endif
