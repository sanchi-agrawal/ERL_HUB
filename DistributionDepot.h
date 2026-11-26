#ifndef DISTRIBUTIONDEPOT_H
#define DISTRIBUTIONDEPOT_H
#include "ReliefCargo.h"
#include <unordered_map>
#include <vector>
#include <string>

class IncidentCommander;

namespace ReliefOps { namespace Inventory {
class DistributionDepot {
    std::string depotId, name;
    std::unordered_map<std::string, ReliefCargo> stock;
public:
    DistributionDepot(const std::string& id="DEP-01",const std::string& n="Central Relief Depot");
    void addCargo(const ReliefCargo& cargo);
    ReliefCargo& getCargo(const std::string& id);
    const ReliefCargo& getCargo(const std::string& id) const;
    bool hasCargo(const std::string& id) const;
    void restock(const std::string& id,double amount);
    void deduct(const std::string& id,double amount);
    double totalWeight() const;
    void display() const;
    std::vector<ReliefCargo> allCargo() const;
    friend class ::IncidentCommander;
};
}}
#endif
