# ERL-HUB — Post-Earthquake Multi-Agency Emergency Relief Logistics Hub

**Course:** CIC-211 Object-Oriented Programming  
**Author:** _<your name, roll number, section>_  
**Submission date:** _<date>_

## Problem Statement
Following a major earthquake, relief agencies must coordinate vehicles, critical supplies, missions, routes and audit records from a field distribution hub.

## Objectives
- Manage heterogeneous relief vehicles.
- Manage limited relief inventory.
- Create and prioritize emergency missions.
- Allocate cargo and dispatch suitable vehicles.
- Demonstrate CIC-211 OOP concepts from Units I–IV.
- Persist transit and manifest audit records in binary files.

## Prerequisites
A C++17 compiler (g++ 7 or newer; MinGW-w64 on Windows). No external libraries.

## Project Structure
```
ERL-HUB/
├── main.cpp
├── include/     headers (.h)
├── src/         implementations (.cpp)
├── data/        binary audit files are written here (folder must exist)
├── Makefile     optional: `make`
├── README.md  TESTING.md  REQUIREMENTS_AUDIT.md  VIVA.md  CHANGELOG.md
```

## Compile
```bash
g++ -std=c++17 -Iinclude main.cpp src/*.cpp -o erl_hub
```
Add `-Wall -Wextra -Wpedantic` to see warnings (the project builds with none). With `make` installed, `make` does the same.
If your shell does not expand `src/*.cpp` (Windows PowerShell), use Git Bash or list the files explicitly.

## Run
Windows MinGW:
```bash
erl_hub.exe
```
Linux/macOS:
```bash
./erl_hub
```
Run from the ERL-HUB root directory so the `data` folder is found.

## Menu
| # | Action | # | Action |
|---|---|---|---|
| 1 | Register vehicle | 12 | Runtime polymorphism demo |
| 2 | View all vehicles | 13 | Save binary records |
| 3 | Add cargo | 14 | Load / display binary records |
| 4 | Restock depot | 15 | Exception demonstration |
| 5 | View inventory | 16 | Statistics |
| 6 | Create mission | 17 | Copy-constructor / deep-copy demo |
| 7 | Allocate cargo to a mission | 18 | Mark vehicle corridor damaged / clear |
| 8 | Dispatch vehicle | 19 | Return vehicle to service |
| 9 | Vehicle status | 20 | Cancel mission (cargo returns to depot) |
| 10 | Mission priorities | 21 | Low-stock report |
| 11 | Urgent-medical priority queue | 22 | Emergency triage override |

## Sample Run
```
Choice: 6
Destination: Kathmandu
Primary need: Trauma care
Priority (1-10): 9
Distance (km): 40
Urgent medical? (y/n): y
Mission #1 created.
Choice: 7        (mission 1, cargo C002, quantity 50)
Cargo allocated successfully.
Choice: 8        (mission 1, vehicle AV01)
Vehicle AV01 dispatched successfully. Fuel remaining: 84
Mission #1 completed.
```

## Typical Workflow
Create a mission (6) → allocate cargo from the depot (7) → dispatch a vehicle (8) → when it is back, return it to service (19). Vehicles that were dispatched cannot be dispatched again until returned; a completed mission cannot be dispatched twice; a cancelled mission returns its reserved cargo to the depot.

## Input Handling
Every prompt reads a whole line. Letters where a number is expected, out-of-range priorities (1–10), non-positive quantities/distances/capacities, duplicate vehicle IDs and unknown IDs are all rejected with a message, and the program keeps running. IDs are case-insensitive (`av01` = `AV01`). If input ends (Ctrl+D / Ctrl+Z) the program exits cleanly.

## OOP Concepts Used
| Concept | Where |
|---|---|
| Encapsulation | `ReliefCargo`, `DistributionDepot`, `Mission`, `ReliefVehicle` keep data private/protected |
| Namespaces | `ReliefOps::Inventory` (cargo, depot, `printLine`), `ReliefOps::Logistics` (`calculateDistance`, `payloadPercentage`) |
| Function overloading | `IncidentCommander::allocateCargo()` and `allocateCargo(int, const string&, double)` |
| Call/return by reference | `DistributionDepot::getCargo()` returns `ReliefCargo&`; allocation modifies it in place |
| Inline functions | `Utilities.h` |
| Friend class | `IncidentCommander` is a friend of `DistributionDepot` (reads `stock` in the low-stock report) and of `Mission` (changes `priority` in the triage override) |
| Constructors / copy constructors / destructors | `ReliefCargo`, `ReliefVehicle`, `TransportVehicle` |
| Dynamic memory + deep copy | `TransportVehicle` owns a raw `ReliefCargo[]` manifest (copy ctor, `operator=`, destructor, `resizeManifest`) |
| Rule of three | `TransportVehicle`, `ReliefVehicle` and `ReliefCargo` define copy constructor, copy assignment and destructor |
| Static members | `ReliefVehicle` tracks available vehicles and dispatched tonnage |
| Operator overloading | `ReliefCargo`: `+=`, `-=`, `==`, `<<`, `>>` |
| Inheritance | `ReliefVehicle` → `AerialTransport`, `GroundConvoy`, `AmphibiousCraft` |
| Virtual inheritance | `AerialTransport` and `GroundConvoy` inherit `virtual ReliefVehicle`; `HybridMedicalVehicle` derives from both (diamond) |
| Composition | each vehicle owns a `GPSBeacon` and an `EmergencyLifeSupportUnit` |
| Runtime polymorphism | virtual `type()`, `routeMultiplier()`, `display()`, `evaluateRoute()`, `isMedicalCapable()` via `ReliefVehicle*`; virtual destructor |
| Exceptions | `CargoOverweightException`, `FuelDepletionException`, `DamagedCorridorException` derive from `std::exception` |
| Templates | `PriorityDispatchQueue<T>` |
| Functor | `UrgentMedicalFirst` orders the dispatch queue |
| I/O manipulator | `manifestHeader` prints the inventory table heading |
| STL | `vector` (fleet, missions), `unordered_map` (depot stock), `priority_queue`, `unique_ptr` |

## Class Hierarchy
`TransportVehicle` (dynamic manifest) → `ReliefVehicle` (abstract, polymorphic base) → `AerialTransport`, `GroundConvoy`, `AmphibiousCraft`; `HybridMedicalVehicle` inherits aerial + ground with a shared virtual base.

## Dispatch Rules
- Mission must exist, be pending, and have cargo allocated.
- Vehicle must be available (not already dispatched) and its corridor not damaged.
- Load must fit the payload capacity (`CargoOverweightException`) and the vehicle must have enough fuel for `distance × route multiplier × 0.5` (`FuelDepletionException`).
- Urgent medical missions get a recommendation to use an aerial or hybrid vehicle.
- If any step fails, the manifest is rolled back and the cargo stays reserved on the mission.

## File Handling
Transit logs (missions, cargo lines) and manifest audit records (fleet status) are stored in binary files under `data/`, each starting with an `ERL1` signature. Records are first written to temporary files and only replace the old files after every write succeeded. **Load** reads the files back, validates them, and prints each record; a corrupt or incompatible file is reported instead of crashing.

## Limitations
The binary persistence layer is audit-oriented: it displays saved records but does not reconstruct the live in-memory fleet and missions. The console UI is intentionally simple so the project stays focused on CIC-211 OOP concepts.
