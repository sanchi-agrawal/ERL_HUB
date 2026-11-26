#ifndef HYBRIDMEDICALVEHICLE_H
#define HYBRIDMEDICALVEHICLE_H
#include "AerialTransport.h"
#include "GroundConvoy.h"

class HybridMedicalVehicle : public AerialTransport, public GroundConvoy {
public:
    HybridMedicalVehicle(const std::string&,const std::string&,double,double=100);
    std::string type() const override{return "Hybrid Medical Vehicle";}
    double routeMultiplier() const override{return 0.9;}
    bool isMedicalCapable() const override{return true;}
    void display() const override;
};
#endif
