#include "HybridMedicalVehicle.h"
#include <iostream>
HybridMedicalVehicle::HybridMedicalVehicle(const std::string& id,const std::string& name,double cap,double fuel)
:ReliefVehicle(id,name,cap,fuel),
 AerialTransport(id,name,cap,fuel),
 GroundConvoy(id,name,cap,fuel){}
void HybridMedicalVehicle::display() const{
    ReliefVehicle::display();
    std::cout<<"  Hybrid medical capability: aerial + ground\n";
}
