#include "DistributionDepot.h"
#include "IOManipulators.h"
#include "Utilities.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
namespace ReliefOps { namespace Inventory {
DistributionDepot::DistributionDepot(const std::string& id,const std::string& n):depotId(id),name(n){}
void DistributionDepot::addCargo(const ReliefCargo& cargo){
    auto it=stock.find(cargo.getId());
    if(it==stock.end()) stock.emplace(cargo.getId(),cargo);
    else it->second += cargo.getQuantity();
}
ReliefCargo& DistributionDepot::getCargo(const std::string& id){ return stock.at(id); }
const ReliefCargo& DistributionDepot::getCargo(const std::string& id) const{ return stock.at(id); }
bool DistributionDepot::hasCargo(const std::string& id) const{ return stock.find(id)!=stock.end(); }
void DistributionDepot::restock(const std::string& id,double amount){ getCargo(id)+=amount; }
void DistributionDepot::deduct(const std::string& id,double amount){ getCargo(id)-=amount; }
double DistributionDepot::totalWeight() const{
    double total=0; for(const auto& p:stock) total+=p.second.totalWeight(); return total;
}
void DistributionDepot::display() const{
    std::cout<<"\nDepot: "<<name<<" ("<<depotId<<")\n";
    printLine('-',78);
    std::cout<<manifestHeader;
    printLine('-',78);
    std::vector<ReliefCargo> items=allCargo();
    std::sort(items.begin(),items.end(),
        [](const ReliefCargo& a,const ReliefCargo& b){return a.getId()<b.getId();});
    for(const auto& c:items)
        std::cout<<std::left<<std::setw(10)<<c.getId()
                 <<std::setw(28)<<c.getName()
                 <<std::setw(18)<<c.getCategory()
                 <<std::setw(10)<<c.getQuantity()
                 <<std::setw(12)<<c.getUnitWeight()<<"\n";
    printLine('-',78);
    std::cout<<"Total stock weight: "<<totalWeight()<<" tonnes\n";
}
std::vector<ReliefCargo> DistributionDepot::allCargo() const{
    std::vector<ReliefCargo> v; for(const auto& p:stock) v.push_back(p.second); return v;
}
}}
