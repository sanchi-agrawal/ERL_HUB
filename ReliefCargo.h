#ifndef RELIEFCARGO_H
#define RELIEFCARGO_H
#include <iostream>
#include <string>

namespace ReliefOps { namespace Inventory {
class ReliefCargo {
    std::string id, name, category;
    double quantity, unitWeight;
public:
    ReliefCargo();
    ReliefCargo(const std::string&,const std::string&,const std::string&,double,double);
    ReliefCargo(const ReliefCargo& other);
    ReliefCargo& operator=(const ReliefCargo& other) = default; // rule of three: keep assignment explicit
    ~ReliefCargo() = default;

    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getCategory() const { return category; }
    double getQuantity() const { return quantity; }
    double getUnitWeight() const { return unitWeight; }
    double totalWeight() const { return quantity * unitWeight; }

    ReliefCargo& operator+=(double amount);
    ReliefCargo& operator-=(double amount);
    bool operator==(const ReliefCargo& other) const;
    friend std::ostream& operator<<(std::ostream&, const ReliefCargo&);
    friend std::istream& operator>>(std::istream&, ReliefCargo&);
};
}}
#endif
