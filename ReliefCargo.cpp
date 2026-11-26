#include "ReliefCargo.h"
#include <stdexcept>

namespace ReliefOps { namespace Inventory {
ReliefCargo::ReliefCargo(): quantity(0), unitWeight(0) {}
ReliefCargo::ReliefCargo(const std::string& i,const std::string& n,const std::string& c,double q,double w)
:id(i),name(n),category(c),quantity(q),unitWeight(w){}
ReliefCargo::ReliefCargo(const ReliefCargo& o)
:id(o.id),name(o.name),category(o.category),quantity(o.quantity),unitWeight(o.unitWeight){}
ReliefCargo& ReliefCargo::operator+=(double amount){
    if(amount<0) throw std::invalid_argument("Restock amount cannot be negative.");
    quantity+=amount; return *this;
}
ReliefCargo& ReliefCargo::operator-=(double amount){
    if(amount<0 || amount>quantity) throw std::invalid_argument("Insufficient cargo quantity.");
    quantity-=amount; return *this;
}
bool ReliefCargo::operator==(const ReliefCargo& o) const { return id==o.id; }
std::ostream& operator<<(std::ostream& out,const ReliefCargo& c){
    out<<c.id<<" | "<<c.name<<" | "<<c.category<<" | Qty: "<<c.quantity<<" | Unit wt: "<<c.unitWeight<<" t";
    return out;
}
std::istream& operator>>(std::istream& in,ReliefCargo& c){
    in>>c.id>>c.name>>c.category>>c.quantity>>c.unitWeight; return in;
}
}}
