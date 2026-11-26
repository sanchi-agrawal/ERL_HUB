#ifndef MISSION_H
#define MISSION_H
#include "ReliefCargo.h"
#include <string>
#include <vector>

class Mission {
    int id;
    std::string location, need;
    int priority;
    double distance;
    bool medical, completed;
    std::vector<ReliefOps::Inventory::ReliefCargo> allocated;

    // The incident commander has emergency authority to re-triage a mission directly.
    friend class IncidentCommander;
public:
    Mission(int,const std::string&,const std::string&,int,double,bool=false);
    int getId() const{return id;}
    int getPriority() const{return priority;}
    double getDistance() const{return distance;}
    bool isMedical() const{return medical;}
    const std::string& getLocation() const{return location;}
    const std::string& getNeed() const{return need;}
    void allocate(const ReliefOps::Inventory::ReliefCargo& c){allocated.push_back(c);}
    const std::vector<ReliefOps::Inventory::ReliefCargo>& getAllocated() const{return allocated;}
    void complete(){completed=true;}
    bool isCompleted() const{return completed;}
    void display() const;
};
#endif
