#include "ReliefVehicle.h"
#include "Utilities.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

int ReliefVehicle::totalOperationalVehicles=0;
double ReliefVehicle::totalDispatchedTonnage=0;

ReliefVehicle::ReliefVehicle(const std::string& id,const std::string& name,double cap,double f)
:vehicleId(id),vehicleName(name),payloadCapacity(cap),fuel(f),maxFuel(f),
 operational(true),corridorDamaged(false),gps(28.61,77.21),lifeSupport(2){
    ++totalOperationalVehicles;
}
ReliefVehicle::ReliefVehicle(const ReliefVehicle& o)
:TransportVehicle(o),vehicleId(o.vehicleId+"_COPY"),vehicleName(o.vehicleName),
 payloadCapacity(o.payloadCapacity),fuel(o.fuel),maxFuel(o.maxFuel),
 operational(o.operational),corridorDamaged(o.corridorDamaged),
 gps(o.gps),lifeSupport(o.lifeSupport){
    if(operational) ++totalOperationalVehicles;
}
ReliefVehicle& ReliefVehicle::operator=(const ReliefVehicle& o){
    if(this!=&o){
        TransportVehicle::operator=(o);   // deep-copies the dynamic manifest
        vehicleId=o.vehicleId; vehicleName=o.vehicleName;
        payloadCapacity=o.payloadCapacity; fuel=o.fuel; maxFuel=o.maxFuel;
        corridorDamaged=o.corridorDamaged; gps=o.gps; lifeSupport=o.lifeSupport;
        // keep the static operational counter consistent with the new state
        if(operational && !o.operational) --totalOperationalVehicles;
        else if(!operational && o.operational) ++totalOperationalVehicles;
        operational=o.operational;
    }
    return *this;
}
ReliefVehicle::~ReliefVehicle(){ if(operational) --totalOperationalVehicles; }

void ReliefVehicle::evaluateRoute() const{
    std::cout<<vehicleName<<" route evaluated from GPS "<<gps.location()
             <<" using multiplier "<<routeMultiplier()<<".\n";
}
void ReliefVehicle::display() const{
    std::ostringstream load;
    load<<std::fixed<<std::setprecision(1)
        <<ReliefOps::Logistics::payloadPercentage(manifestWeight(),payloadCapacity)<<"%";
    const char* status = !operational ? "DISPATCHED"
                       : (corridorDamaged ? "CORRIDOR DAMAGED" : "READY");
    std::cout<<std::left<<std::setw(8)<<vehicleId
             <<std::setw(30)<<vehicleName
             <<std::setw(24)<<type()
             <<std::setw(8)<<payloadCapacity
             <<std::setw(8)<<fuel
             <<std::setw(9)<<load.str()
             <<status<<"\n";
}
void ReliefVehicle::loadCargo(const ReliefOps::Inventory::ReliefCargo& c){
    if(manifestWeight()+c.totalWeight()>payloadCapacity)
        throw CargoOverweightException("Cargo exceeds vehicle payload capacity.");
    addToManifest(c);
}
void ReliefVehicle::dispatch(double distance){
    if(!operational)
        throw std::runtime_error("Vehicle is not available (already dispatched).");
    if(distance<=0)
        throw std::invalid_argument("Route distance must be positive.");
    if(corridorDamaged)
        throw DamagedCorridorException("Selected corridor is damaged and unsafe.");
    double requiredFuel=distance*routeMultiplier()*0.5;
    if(requiredFuel>fuel)
        throw FuelDepletionException("Vehicle does not have enough fuel for this route.");
    fuel-=requiredFuel;
    totalDispatchedTonnage+=manifestWeight();
    operational=false;
    --totalOperationalVehicles;
    std::cout<<"Vehicle "<<vehicleId<<" dispatched successfully. Fuel remaining: "<<fuel<<"\n";
}
void ReliefVehicle::refuel(double amount){
    if(amount>0) fuel=std::min(maxFuel,fuel+amount);
}
void ReliefVehicle::returnToService(){
    if(!operational){ operational=true; ++totalOperationalVehicles; }
    clearManifest();
    fuel=maxFuel;
}
