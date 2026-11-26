#ifndef RELIEFVEHICLE_H
#define RELIEFVEHICLE_H
#include "TransportVehicle.h"
#include "GPSBeacon.h"
#include "EmergencyLifeSupportUnit.h"
#include "Exceptions.h"
#include <string>

class ReliefVehicle : public TransportVehicle {
protected:
    std::string vehicleId, vehicleName;
    double payloadCapacity, fuel, maxFuel;
    bool operational, corridorDamaged;
    GPSBeacon gps;
    EmergencyLifeSupportUnit lifeSupport;
    static int totalOperationalVehicles;
    static double totalDispatchedTonnage;
public:
    ReliefVehicle(const std::string& id,const std::string& name,double capacity,double fuel=100);
    ReliefVehicle(const ReliefVehicle& other);
    ReliefVehicle& operator=(const ReliefVehicle& other);
    virtual ~ReliefVehicle();
    virtual std::string type() const=0;
    virtual double routeMultiplier() const=0;
    virtual bool isMedicalCapable() const { return false; }
    virtual void evaluateRoute() const;
    virtual void display() const;
    void loadCargo(const ReliefOps::Inventory::ReliefCargo& cargo);
    void clearManifest(){ manifestCount=0; }
    void dispatch(double distance);
    void refuel(double amount);
    void returnToService();
    void setCorridorDamaged(bool value){ corridorDamaged=value; }
    bool isCorridorDamaged() const{return corridorDamaged;}
    bool isOperational() const{return operational;}
    const std::string& getId() const{return vehicleId;}
    const std::string& getName() const{return vehicleName;}
    double getPayloadCapacity() const{return payloadCapacity;}
    double getFuel() const{return fuel;}
    static int getTotalOperationalVehicles(){return totalOperationalVehicles;}
    static double getTotalDispatchedTonnage(){return totalDispatchedTonnage;}
};
#endif
