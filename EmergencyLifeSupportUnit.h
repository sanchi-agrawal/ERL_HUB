#ifndef EMERGENCY_LIFE_SUPPORT_UNIT_H
#define EMERGENCY_LIFE_SUPPORT_UNIT_H
class EmergencyLifeSupportUnit {
    bool operational{true};
    int medicalKits{0};
public:
    explicit EmergencyLifeSupportUnit(int kits=0): medicalKits(kits) {}
    void addKits(int n){ if(n>0) medicalKits+=n; }
    bool isOperational() const { return operational; }
    int getMedicalKits() const { return medicalKits; }
    void disable(){ operational=false; }
};
#endif
