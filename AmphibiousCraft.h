#ifndef AMPHIBIOUSCRAFT_H
#define AMPHIBIOUSCRAFT_H
#include "ReliefVehicle.h"
class AmphibiousCraft : public virtual ReliefVehicle {
public:
    AmphibiousCraft(const std::string&,const std::string&,double,double=100);
    std::string type() const override{return "Amphibious Craft";}
    double routeMultiplier() const override{return 1.2;}
};
#endif
