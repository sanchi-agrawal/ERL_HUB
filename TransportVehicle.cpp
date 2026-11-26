#include "TransportVehicle.h"

TransportVehicle::TransportVehicle():manifest(new ReliefOps::Inventory::ReliefCargo[4]),manifestCount(0),manifestCapacity(4){}

TransportVehicle::TransportVehicle(const TransportVehicle& other)
:manifest(new ReliefOps::Inventory::ReliefCargo[other.manifestCapacity]),
 manifestCount(other.manifestCount),manifestCapacity(other.manifestCapacity){
    for(std::size_t i=0;i<manifestCount;++i) manifest[i]=other.manifest[i];
}

TransportVehicle& TransportVehicle::operator=(const TransportVehicle& other){
    if(this!=&other){
        delete[] manifest;
        manifestCapacity=other.manifestCapacity;
        manifestCount=other.manifestCount;
        manifest=new ReliefOps::Inventory::ReliefCargo[manifestCapacity];
        for(std::size_t i=0;i<manifestCount;++i) manifest[i]=other.manifest[i];
    }
    return *this;
}

TransportVehicle::~TransportVehicle(){ delete[] manifest; }

void TransportVehicle::resizeManifest(std::size_t newCapacity){
    auto* newManifest=new ReliefOps::Inventory::ReliefCargo[newCapacity];
    for(std::size_t i=0;i<manifestCount;++i) newManifest[i]=manifest[i];
    delete[] manifest;
    manifest=newManifest;
    manifestCapacity=newCapacity;
}

void TransportVehicle::addToManifest(const ReliefOps::Inventory::ReliefCargo& c){
    if(manifestCount==manifestCapacity) resizeManifest(manifestCapacity*2);
    manifest[manifestCount++]=c;
}

double TransportVehicle::manifestWeight() const{
    double w=0;
    for(std::size_t i=0;i<manifestCount;++i) w+=manifest[i].totalWeight();
    return w;
}
