#include "IncidentCommander.h"
#include "IOManipulators.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace ReliefOps::Inventory;

// ---------------------------------------------------------------------------
// Input helpers. Every reader consumes a whole line, so stray characters never
// leak into the next prompt, and end-of-input exits cleanly instead of looping.
// ---------------------------------------------------------------------------
namespace {

const char* const kMagic = "ERL1";                      // binary file signature/version
const char* const kTransitFile = "data/transit_logs.bin";
const char* const kTransitTmp  = "data/transit_logs.tmp";
const char* const kAuditFile   = "data/manifest_audit.bin";
const char* const kAuditTmp    = "data/manifest_audit.tmp";

[[noreturn]] void inputClosed(){
    std::cout<<"\nInput closed. Exiting safely.\n";
    std::exit(0);
}

std::string trim(const std::string& s){
    const char* ws=" \t\r\n";
    std::size_t a=s.find_first_not_of(ws);
    if(a==std::string::npos) return "";
    std::size_t b=s.find_last_not_of(ws);
    return s.substr(a,b-a+1);
}

std::string readLine(const std::string& prompt){
    std::string s;
    while(true){
        std::cout<<prompt;
        if(!std::getline(std::cin,s)) inputClosed();
        s=trim(s);
        if(!s.empty()) return s;
        std::cout<<"Input cannot be empty.\n";
    }
}

std::string readId(const std::string& prompt){
    while(true){
        std::string s=readLine(prompt);
        if(s.find_first_of(" \t")!=std::string::npos){
            std::cout<<"An ID must be a single word with no spaces.\n";
            continue;
        }
        for(char& ch:s) ch=static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        return s;
    }
}

int readInt(const std::string& prompt){
    while(true){
        std::istringstream iss(readLine(prompt));
        int x;
        if((iss>>x) && (iss>>std::ws).eof()) return x;
        std::cout<<"Invalid input. Enter a whole number.\n";
    }
}

int readIntInRange(const std::string& prompt,int lo,int hi){
    while(true){
        int x=readInt(prompt);
        if(x>=lo && x<=hi) return x;
        std::cout<<"Enter a value from "<<lo<<" to "<<hi<<".\n";
    }
}

double readDouble(const std::string& prompt){
    while(true){
        std::istringstream iss(readLine(prompt));
        double x;
        if((iss>>x) && (iss>>std::ws).eof() && std::isfinite(x)) return x;
        std::cout<<"Invalid input. Enter a number.\n";
    }
}

double readPositiveDouble(const std::string& prompt){
    while(true){
        double x=readDouble(prompt);
        if(x>0) return x;
        std::cout<<"Value must be greater than zero.\n";
    }
}

double readNonNegativeDouble(const std::string& prompt){
    while(true){
        double x=readDouble(prompt);
        if(x>=0) return x;
        std::cout<<"Value cannot be negative.\n";
    }
}

bool readYesNo(const std::string& prompt){
    while(true){
        std::string s=readLine(prompt);
        for(char& ch:s) ch=static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        if(s=="y"||s=="yes") return true;
        if(s=="n"||s=="no") return false;
        std::cout<<"Please answer y or n.\n";
    }
}

// ---------------------------------------------------------------------------
// Binary helpers
// ---------------------------------------------------------------------------
template<typename T> void writePod(std::ostream& o,const T& v){
    o.write(reinterpret_cast<const char*>(&v),sizeof(T));
}
template<typename T> bool readPod(std::istream& i,T& v){
    i.read(reinterpret_cast<char*>(&v),sizeof(T));
    return static_cast<bool>(i);
}
void writeStr(std::ostream& o,const std::string& s){
    std::uint32_t n=static_cast<std::uint32_t>(s.size());
    writePod(o,n);
    o.write(s.data(),static_cast<std::streamsize>(n));
}
bool readStr(std::istream& i,std::string& s){
    std::uint32_t n=0;
    if(!readPod(i,n) || n>100000) return false;
    s.assign(n,'\0');
    if(n>0) i.read(&s[0],static_cast<std::streamsize>(n));
    return static_cast<bool>(i);
}
bool readMagic(std::istream& i){
    char buf[4]={0,0,0,0};
    i.read(buf,4);
    return i && std::string(buf,4)==kMagic;
}

} // namespace

// ---------------------------------------------------------------------------
// Setup and lookup
// ---------------------------------------------------------------------------
IncidentCommander::IncidentCommander():depot("DEP-01","Central Relief Depot"){
    seedData();
}

void IncidentCommander::seedData(){
    depot.addCargo(ReliefCargo("C001","Ration Kits","Food",500,0.01));
    depot.addCargo(ReliefCargo("C002","Trauma Medicine","Medical",120,0.002));
    depot.addCargo(ReliefCargo("C003","Clean Water","Water",300,0.02));
    depot.addCargo(ReliefCargo("C004","Thermal Blankets","Shelter",200,0.005));

    fleet.push_back(std::make_unique<GroundConvoy>("GV01","Heavy All-Terrain Truck",20,100));
    fleet.push_back(std::make_unique<AerialTransport>("AV01","Emergency Medical Helicopter",5,100));
    fleet.push_back(std::make_unique<AerialTransport>("AV02","Search-and-Rescue Quadcopter",1,100));
    fleet.push_back(std::make_unique<AmphibiousCraft>("AC01","Amphibious Relief Vehicle",12,100));
    fleet.push_back(std::make_unique<HybridMedicalVehicle>("HM01","Hybrid Mobile Medical Unit",8,100));
}

ReliefVehicle* IncidentCommander::findVehicle(const std::string& id) const{
    for(const auto& v:fleet) if(v->getId()==id) return v.get();
    return nullptr;
}

Mission* IncidentCommander::findMission(int id){
    for(auto& m:missions) if(m.getId()==id) return &m;
    return nullptr;
}

// ---------------------------------------------------------------------------
// Vehicles
// ---------------------------------------------------------------------------
void IncidentCommander::addVehicle(){
    int t=readIntInRange("1 Ground  2 Aerial  3 Amphibious  4 Hybrid: ",1,4);
    std::string id=readId("Vehicle ID: ");
    if(findVehicle(id)){
        std::cout<<"A vehicle with ID "<<id<<" already exists.\n";
        return;
    }
    std::string name=readLine("Vehicle name: ");
    double cap=readPositiveDouble("Payload capacity (tonnes): ");
    double fuel=readPositiveDouble("Fuel: ");

    if(t==1) fleet.push_back(std::make_unique<GroundConvoy>(id,name,cap,fuel));
    else if(t==2) fleet.push_back(std::make_unique<AerialTransport>(id,name,cap,fuel));
    else if(t==3) fleet.push_back(std::make_unique<AmphibiousCraft>(id,name,cap,fuel));
    else fleet.push_back(std::make_unique<HybridMedicalVehicle>(id,name,cap,fuel));
    std::cout<<"Vehicle "<<id<<" registered.\n";
}

static void printVehicleHeader(){
    std::cout<<"\n"<<std::left<<std::setw(8)<<"ID"
             <<std::setw(30)<<"NAME"<<std::setw(24)<<"TYPE"
             <<std::setw(8)<<"CAP"<<std::setw(8)<<"FUEL"
             <<std::setw(9)<<"LOAD"<<"STATUS\n";
}

void IncidentCommander::viewVehicles() const{
    printVehicleHeader();
    for(const auto& v:fleet) v->display();
}

void IncidentCommander::vehicleStatus() const{
    ReliefVehicle* v=findVehicle(readId("Vehicle ID: "));
    if(!v){std::cout<<"Vehicle not found.\n";return;}
    printVehicleHeader();
    v->display();
}

void IncidentCommander::markCorridor(){
    ReliefVehicle* v=findVehicle(readId("Vehicle ID: "));
    if(!v){std::cout<<"Vehicle not found.\n";return;}
    bool damaged=readYesNo("Is the vehicle's corridor damaged? (y = damaged, n = cleared): ");
    v->setCorridorDamaged(damaged);
    std::cout<<"Corridor for "<<v->getId()<<" marked as "<<(damaged?"DAMAGED":"clear")<<".\n";
}

void IncidentCommander::returnVehicle(){
    ReliefVehicle* v=findVehicle(readId("Vehicle ID: "));
    if(!v){std::cout<<"Vehicle not found.\n";return;}
    v->returnToService();
    std::cout<<"Vehicle "<<v->getId()<<" returned to service: manifest cleared and fuel restored.\n";
}

// ---------------------------------------------------------------------------
// Inventory
// ---------------------------------------------------------------------------
void IncidentCommander::addCargo(){
    std::string id=readId("Cargo ID: ");
    if(depot.hasCargo(id)){
        double q=readPositiveDouble("Cargo ID already exists. Quantity to add to existing stock: ");
        depot.restock(id,q);
        std::cout<<"Existing cargo stock increased.\n";
        return;
    }
    std::string name=readLine("Cargo name: ");
    std::string cat=readLine("Category: ");
    double q=readPositiveDouble("Quantity: ");
    double w=readPositiveDouble("Unit weight (tonnes): ");
    depot.addCargo(ReliefCargo(id,name,cat,q,w));
    std::cout<<"Cargo added.\n";
}

void IncidentCommander::restock(){
    std::string id=readId("Cargo ID: ");
    if(!depot.hasCargo(id)){std::cout<<"Cargo not found.\n";return;}
    double amount=readPositiveDouble("Amount to restock: ");
    try{
        depot.restock(id,amount);
        std::cout<<"Restocked.\n";
    }catch(const std::exception& e){
        std::cout<<"Restock failed: "<<e.what()<<"\n";
    }
}

void IncidentCommander::viewInventory() const{
    depot.display();
}

// Friend access: IncidentCommander reads the depot's private stock map directly.
void IncidentCommander::lowStockReport() const{
    double threshold=readNonNegativeDouble("Show cargo with quantity below: ");
    std::vector<const ReliefCargo*> low;
    for(const auto& p:depot.stock)
        if(p.second.getQuantity()<threshold) low.push_back(&p.second);
    if(low.empty()){std::cout<<"No cargo is below "<<threshold<<".\n";return;}
    std::sort(low.begin(),low.end(),
        [](const ReliefCargo* a,const ReliefCargo* b){return a->getId()<b->getId();});
    std::cout<<"\nLOW STOCK (below "<<threshold<<")\n"<<manifestHeader;
    for(const ReliefCargo* c:low)
        std::cout<<std::left<<std::setw(10)<<c->getId()
                 <<std::setw(28)<<c->getName()
                 <<std::setw(18)<<c->getCategory()
                 <<std::setw(10)<<c->getQuantity()
                 <<std::setw(12)<<c->getUnitWeight()<<"\n";
}

// ---------------------------------------------------------------------------
// Missions
// ---------------------------------------------------------------------------
void IncidentCommander::createMission(){
    std::string loc=readLine("Destination: ");
    std::string need=readLine("Primary need: ");
    int p=readIntInRange("Priority (1-10): ",1,10);
    double d=readPositiveDouble("Distance (km): ");
    bool med=readYesNo("Urgent medical? (y/n): ");
    missions.emplace_back(nextMissionId,loc,need,p,d,med);
    std::cout<<"Mission #"<<nextMissionId<<" created.\n";
    ++nextMissionId;
}

void IncidentCommander::allocateCargo(){
    if(missions.empty()){std::cout<<"No missions.\n";return;}
    int mid=readInt("Mission ID: ");
    std::string cid=readId("Cargo ID: ");
    double amount=readPositiveDouble("Quantity to allocate: ");
    allocateCargo(mid,cid,amount);
}

// Overloaded allocation routine: the second form accepts mission parameters directly.
void IncidentCommander::allocateCargo(int mid,const std::string& cid,double amount){
    Mission* m=findMission(mid);
    if(!m){std::cout<<"Mission not found.\n";return;}
    if(m->isCompleted()){std::cout<<"Mission #"<<mid<<" is already completed.\n";return;}
    if(!depot.hasCargo(cid)){std::cout<<"Cargo not found.\n";return;}
    try{
        ReliefCargo& cargo=depot.getCargo(cid); // return by reference
        if(amount<=0) throw std::invalid_argument("Quantity must be greater than zero.");
        if(amount>cargo.getQuantity()) throw std::invalid_argument("Not enough stock.");
        cargo-=amount;
        m->allocate(ReliefCargo(cargo.getId(),cargo.getName(),cargo.getCategory(),amount,cargo.getUnitWeight()));
        std::cout<<"Cargo allocated successfully.\n";
    }catch(const std::exception& e){std::cout<<"Allocation failed: "<<e.what()<<"\n";}
}

void IncidentCommander::dispatchVehicle(){
    if(missions.empty()){std::cout<<"No missions.\n";return;}

    int mid=readInt("Mission ID: ");
    std::string vid=readId("Vehicle ID: ");

    Mission* m=findMission(mid);
    if(!m){std::cout<<"Mission not found.\n";return;}
    ReliefVehicle* v=findVehicle(vid);
    if(!v){std::cout<<"Vehicle not found.\n";return;}

    if(m->isCompleted()){std::cout<<"Mission #"<<mid<<" is already completed.\n";return;}
    if(!v->isOperational()){
        std::cout<<"Vehicle "<<vid<<" is already dispatched. Return it to service first (menu 19).\n";
        return;
    }
    if(m->getAllocated().empty()){
        std::cout<<"Mission #"<<mid<<" has no cargo allocated. Allocate cargo first (menu 7).\n";
        return;
    }
    if(m->isMedical() && !v->isMedicalCapable())
        std::cout<<"Note: "<<vid<<" is not a medical-capable vehicle; an aerial or hybrid vehicle is recommended for urgent medical missions.\n";

    try{
        v->clearManifest();
        for(const auto& c:m->getAllocated()) v->loadCargo(c);
        v->dispatch(m->getDistance());
        m->complete();
        std::cout<<"Mission #"<<mid<<" completed.\n";
    }
    catch(const std::exception& e){
        v->clearManifest();   // roll back any partially loaded cargo
        std::cout<<"Dispatch failed: "<<e.what()<<"\n";
    }
}

void IncidentCommander::cancelMission(){
    if(missions.empty()){std::cout<<"No missions.\n";return;}
    int mid=readInt("Mission ID to cancel: ");
    auto it=std::find_if(missions.begin(),missions.end(),
        [&](const Mission& m){return m.getId()==mid;});
    if(it==missions.end()){std::cout<<"Mission not found.\n";return;}
    if(it->isCompleted()){std::cout<<"Completed missions cannot be cancelled.\n";return;}
    for(const auto& c:it->getAllocated()) depot.addCargo(c);   // return reserved cargo to the depot
    missions.erase(it);
    std::cout<<"Mission #"<<mid<<" cancelled; reserved cargo returned to the depot.\n";
}

void IncidentCommander::missionPriority() const{
    if(missions.empty()){std::cout<<"No missions.\n";return;}
    std::vector<Mission> copy=missions;
    std::stable_sort(copy.begin(),copy.end(),
        [](const Mission& a,const Mission& b){
            return a.getPriority()>b.getPriority();
        });
    for(const auto& m:copy)m.display();
}

void IncidentCommander::urgentDispatch(){
    // The functor says "a is served before b"; priority_queue's top is the element
    // for which comp(top,x) is false, so the comparator is passed inverted.
    PriorityDispatchQueue<Mission> q([](const Mission& a,const Mission& b){
        return UrgentMedicalFirst{}(b,a);
    });
    for(const auto& m:missions) if(!m.isCompleted()) q.push(m);

    if(q.empty()){std::cout<<"No pending missions.\n";return;}
    std::cout<<"Priority queue order:\n";
    while(!q.empty()){
        Mission m=q.pop();
        m.display();
    }
}

// Friend access: IncidentCommander modifies Mission's private priority directly.
void IncidentCommander::emergencyTriageOverride(Mission& mission,int newPriority){
    if(mission.completed){std::cout<<"Mission #"<<mission.id<<" is already completed.\n";return;}
    if(newPriority<1||newPriority>10){std::cout<<"Priority must be between 1 and 10.\n";return;}
    std::cout<<"Emergency triage override: Mission #"<<mission.id
             <<" priority "<<mission.priority<<" -> "<<newPriority<<"\n";
    mission.priority=newPriority;
}

void IncidentCommander::triageOverride(){
    if(missions.empty()){std::cout<<"No missions.\n";return;}
    int mid=readInt("Mission ID: ");
    Mission* m=findMission(mid);
    if(!m){std::cout<<"Mission not found.\n";return;}
    int p=readIntInRange("New priority (1-10): ",1,10);
    emergencyTriageOverride(*m,p);
}

// ---------------------------------------------------------------------------
// Demonstrations and statistics
// ---------------------------------------------------------------------------
void IncidentCommander::demonstratePolymorphism() const{
    std::cout<<"\nRuntime polymorphism demonstration:\n";
    for(const auto& v:fleet){
        ReliefVehicle* base=v.get();
        base->evaluateRoute();
    }
}

void IncidentCommander::showStatistics() const{
    std::cout<<"Fleet size: "<<fleet.size()<<"\n";
    std::cout<<"Operational (available) vehicles: "
             <<ReliefVehicle::getTotalOperationalVehicles()<<"\n";
    std::cout<<"Total dispatched tonnage: "
             <<ReliefVehicle::getTotalDispatchedTonnage()<<" tonnes\n";
    std::cout<<"Mission count: "<<missions.size()<<"\n";
}

// Each case triggers the exception through the real vehicle code path.
void IncidentCommander::demonstrateExceptions(){
    int x=readIntInRange("1 Overweight  2 Fuel depletion  3 Damaged corridor: ",1,3);
    try{
        if(x==1){
            AerialTransport demo("DEMO","Demo Quadcopter",1,100);          // 1 tonne capacity
            demo.loadCargo(ReliefCargo("DEMO-C","Demo Water","Water",200,0.02)); // 4 tonnes
        }else if(x==2){
            GroundConvoy demo("DEMO","Demo Truck",10,1);                    // only 1 unit of fuel
            demo.dispatch(100);
        }else{
            AmphibiousCraft demo("DEMO","Demo Amphibian",10,100);
            demo.setCorridorDamaged(true);
            demo.dispatch(10);
        }
    }
    catch(const CargoOverweightException& e){
        std::cout<<"Caught CargoOverweightException: "<<e.what()<<"\n";
    }
    catch(const FuelDepletionException& e){
        std::cout<<"Caught FuelDepletionException: "<<e.what()<<"\n";
    }
    catch(const DamagedCorridorException& e){
        std::cout<<"Caught DamagedCorridorException: "<<e.what()<<"\n";
    }
    catch(const std::exception& e){
        std::cout<<"Caught std::exception: "<<e.what()<<"\n";
    }
}

void IncidentCommander::demonstrateCopy(){
    ReliefCargo original("CP01","Copy Demo Cargo","Test",10,0.01);
    ReliefCargo copy(original);
    copy+=5;
    std::cout<<"Original quantity: "<<original.getQuantity()
             <<" | Copied object quantity: "<<copy.getQuantity()<<"\n";
}

// ---------------------------------------------------------------------------
// Binary persistence (audit-oriented)
// ---------------------------------------------------------------------------
void IncidentCommander::saveRecords() const{
    // Transaction-style approach: write to temporary files first.
    std::ofstream out(kTransitTmp,std::ios::binary|std::ios::trunc);
    std::ofstream audit(kAuditTmp,std::ios::binary|std::ios::trunc);

    auto discardTemps=[&](){
        out.close(); audit.close();
        std::remove(kTransitTmp);
        std::remove(kAuditTmp);
    };

    if(!out||!audit){
        std::cout<<"File error while creating temporary records. "
                   "Run the program from the ERL-HUB folder so the data/ folder exists.\n";
        discardTemps();
        return;
    }

    // --- transit log: missions ---
    out.write(kMagic,4);
    writePod(out,static_cast<std::int32_t>(missions.size()));
    for(const auto& m:missions){
        writePod(out,static_cast<std::int32_t>(m.getId()));
        writePod(out,static_cast<std::int32_t>(m.getPriority()));
        writePod(out,static_cast<double>(m.getDistance()));
        writePod(out,static_cast<std::uint8_t>(m.isMedical()?1:0));
        writePod(out,static_cast<std::uint8_t>(m.isCompleted()?1:0));
        writeStr(out,m.getLocation());
        writeStr(out,m.getNeed());
        writePod(out,static_cast<std::int32_t>(m.getAllocated().size()));
        for(const auto& c:m.getAllocated()){
            writeStr(out,c.getId());
            writePod(out,static_cast<double>(c.getQuantity()));
        }
    }

    // --- manifest audit: vehicles ---
    audit.write(kMagic,4);
    writePod(audit,static_cast<std::int32_t>(fleet.size()));
    for(const auto& v:fleet){
        writeStr(audit,v->getId()+"|"+v->getName()+"|"+v->type());
        writePod(audit,static_cast<double>(v->getFuel()));
        writePod(audit,static_cast<double>(v->manifestWeight()));
        writePod(audit,static_cast<std::uint8_t>(v->isOperational()?1:0));
    }

    if(!out.good()||!audit.good()){
        std::cout<<"Write error; existing records were left untouched.\n";
        discardTemps();
        return;
    }
    out.close();
    audit.close();

    // Replace old records only after the temporary writes succeeded.
    std::remove(kTransitFile);
    std::remove(kAuditFile);
    if(std::rename(kTransitTmp,kTransitFile)!=0 || std::rename(kAuditTmp,kAuditFile)!=0){
        std::cout<<"Could not finalize the record files.\n";
        return;
    }
    std::cout<<"Binary records saved successfully.\n";
}

void IncidentCommander::loadRecords(){
    std::ifstream in(kTransitFile,std::ios::binary);
    if(!in){std::cout<<"No transit log file found. Save records first (menu 13).\n";return;}

    std::int32_t count=0;
    if(!readMagic(in) || !readPod(in,count) || count<0 || count>100000){
        std::cout<<"Transit log is corrupt or from an incompatible version.\n";
        return;
    }
    std::cout<<"Loaded "<<count<<" mission audit record(s):\n";
    for(std::int32_t i=0;i<count;++i){
        std::int32_t id=0,pri=0,nAlloc=0;
        double dist=0;
        std::uint8_t med=0,done=0;
        std::string loc,need;
        if(!readPod(in,id)||!readPod(in,pri)||!readPod(in,dist)||!readPod(in,med)||!readPod(in,done)
           ||!readStr(in,loc)||!readStr(in,need)||!readPod(in,nAlloc)||nAlloc<0||nAlloc>100000){
            std::cout<<"Transit log ended unexpectedly; stopped at record "<<i+1<<".\n";
            return;
        }
        for(std::int32_t k=0;k<nAlloc;++k){
            std::string cid; double q=0;
            if(!readStr(in,cid)||!readPod(in,q)){
                std::cout<<"Transit log ended unexpectedly inside record "<<i+1<<".\n";
                return;
            }
        }
        std::cout<<"  Mission #"<<id<<" | "<<loc<<" | Need: "<<need<<" | Priority: "<<pri
                 <<" | "<<dist<<" km | "<<(med?"URGENT MEDICAL":"Standard")
                 <<" | "<<(done?"Completed":"Pending")<<" | cargo lines: "<<nAlloc<<"\n";
    }

    std::ifstream audit(kAuditFile,std::ios::binary);
    if(!audit){std::cout<<"No manifest audit file found.\n";return;}
    std::int32_t n=0;
    if(!readMagic(audit) || !readPod(audit,n) || n<0 || n>100000){
        std::cout<<"Manifest audit is corrupt or from an incompatible version.\n";
        return;
    }
    std::cout<<"Loaded "<<n<<" vehicle manifest audit record(s):\n";
    for(std::int32_t i=0;i<n;++i){
        std::string desc; double fuel=0,w=0; std::uint8_t op=0;
        if(!readStr(audit,desc)||!readPod(audit,fuel)||!readPod(audit,w)||!readPod(audit,op)){
            std::cout<<"Manifest audit ended unexpectedly; stopped at record "<<i+1<<".\n";
            return;
        }
        std::cout<<"  "<<desc<<" | fuel "<<fuel<<" | manifest "<<w<<" t | "
                 <<(op?"available":"dispatched")<<"\n";
    }
}

// ---------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------
void IncidentCommander::run(){
    int c;
    do{
        std::cout<<"\n===== ERL-HUB =====\n"
                 "1  Add/register vehicle\n"
                 "2  View all vehicles\n"
                 "3  Add cargo\n"
                 "4  Restock depot\n"
                 "5  View inventory\n"
                 "6  Create mission\n"
                 "7  Allocate cargo\n"
                 "8  Dispatch vehicle\n"
                 "9  Vehicle status\n"
                 "10 Mission priorities\n"
                 "11 Urgent medical priority queue\n"
                 "12 Runtime polymorphism\n"
                 "13 Save binary records\n"
                 "14 Load binary records\n"
                 "15 Exception demonstration\n"
                 "16 Statistics\n"
                 "17 Copy constructor/deep-copy demo\n"
                 "18 Mark vehicle corridor damaged/clear\n"
                 "19 Return vehicle to service (refuel, clear manifest)\n"
                 "20 Cancel mission (return cargo to depot)\n"
                 "21 Low-stock report\n"
                 "22 Emergency triage override (change mission priority)\n"
                 "0  Exit\n";

        c=readInt("Choice: ");

        try{
            switch(c){
                case 1:addVehicle();break;
                case 2:viewVehicles();break;
                case 3:addCargo();break;
                case 4:restock();break;
                case 5:viewInventory();break;
                case 6:createMission();break;
                case 7:allocateCargo();break;
                case 8:dispatchVehicle();break;
                case 9:vehicleStatus();break;
                case 10:missionPriority();break;
                case 11:urgentDispatch();break;
                case 12:demonstratePolymorphism();break;
                case 13:saveRecords();break;
                case 14:loadRecords();break;
                case 15:demonstrateExceptions();break;
                case 16:showStatistics();break;
                case 17:demonstrateCopy();break;
                case 18:markCorridor();break;
                case 19:returnVehicle();break;
                case 20:cancelMission();break;
                case 21:lowStockReport();break;
                case 22:triageOverride();break;
                case 0:std::cout<<"Exiting safely.\n";break;
                default:std::cout<<"Invalid choice.\n";
            }
        }catch(const std::exception& e){
            // Safety net: no unexpected error should ever terminate the program.
            std::cout<<"Operation failed: "<<e.what()<<"\n";
        }
    }while(c!=0);
}
