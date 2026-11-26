#ifndef INCIDENTCOMMANDER_H
#define INCIDENTCOMMANDER_H
#include "DistributionDepot.h"
#include "Mission.h"
#include "ReliefVehicle.h"
#include "AerialTransport.h"
#include "GroundConvoy.h"
#include "AmphibiousCraft.h"
#include "HybridMedicalVehicle.h"
#include "PriorityDispatchQueue.h"
#include "Functors.h"
#include <memory>
#include <string>
#include <vector>

class IncidentCommander {
    ReliefOps::Inventory::DistributionDepot depot;
    std::vector<std::unique_ptr<ReliefVehicle>> fleet;
    std::vector<Mission> missions;
    int nextMissionId{1};

    void seedData();
    ReliefVehicle* findVehicle(const std::string& id) const;
    Mission* findMission(int id);
public:
    IncidentCommander();
    void run();
    void addVehicle();
    void viewVehicles() const;
    void addCargo();
    void restock();
    void viewInventory() const;
    void createMission();
    void allocateCargo();
    void allocateCargo(int missionId, const std::string& cargoId, double amount);
    void dispatchVehicle();
    void vehicleStatus() const;
    void missionPriority() const;
    void urgentDispatch();
    void demonstratePolymorphism() const;
    void saveRecords() const;
    void loadRecords();
    void showStatistics() const;
    void demonstrateExceptions();
    void demonstrateCopy();

    void markCorridor();
    void returnVehicle();
    void cancelMission();
    void lowStockReport() const;     // uses friend access to the depot's private stock
    void triageOverride();
    void emergencyTriageOverride(Mission& mission, int newPriority); // uses friend access to Mission
};
#endif
