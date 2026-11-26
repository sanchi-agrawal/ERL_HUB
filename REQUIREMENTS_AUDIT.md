# ERL-HUB Requirements Audit

This table is the final verification checklist. A requirement should only be marked implemented after compiling and running the corresponding test.

| Requirement | Implemented? | File/Class | Evidence |
|---|---|---|---|
| ReliefOps::Logistics | Yes | Utilities.h / vehicle code | Namespace exists |
| ReliefOps::Inventory | Yes | ReliefCargo / DistributionDepot | Namespace exists |
| Function overloading for allocation | Yes | IncidentCommander | `allocateCargo()` and `allocateCargo(int, const string&, double)` |
| Call by reference | Yes | IncidentCommander / DistributionDepot | `ReliefCargo&` used |
| Return by reference | Yes | DistributionDepot | `getCargo()` returns reference |
| Inline utilities | Yes | Utilities.h | Inline distance/payload functions |
| Friend IncidentCommander | Yes | DistributionDepot.h / Mission.h | Friend of both; reads `stock` (menu 21) and edits `Mission::priority` (menu 22) |
| ReliefCargo | Yes | ReliefCargo.h/.cpp | Implemented |
| DistributionDepot | Yes | DistributionDepot.h/.cpp | Implemented |
| TransportVehicle | Yes | TransportVehicle.h/.cpp | Implemented |
| Encapsulation | Yes | Core classes | Private/protected data |
| Static variables/functions | Yes | ReliefVehicle | Static analytics |
| Dynamic manifest allocation | Yes | TransportVehicle | Raw `ReliefCargo[]` with `new[]`/`delete[]` and `resizeManifest()` |
| Parameterized constructors | Yes | Core classes | Implemented |
| Copy constructors | Yes | ReliefCargo / ReliefVehicle | Implemented |
| Destructors | Yes | ReliefVehicle | Virtual destructor |
| Deep copy / rule of three | Yes | TransportVehicle / ReliefVehicle / ReliefCargo | Copy ctor, `operator=` and destructor; verified by unit test and menu 17 |
| `+=` | Yes | ReliefCargo | Restocking |
| `-=` | Yes | ReliefCargo | Deduction |
| `==` | Yes | ReliefCargo | ID identity |
| `<<` / `>>` | Yes | ReliefCargo | Stream operators |
| ReliefVehicle | Yes | ReliefVehicle.h | Base class |
| AerialTransport | Yes | AerialTransport | Derived |
| GroundConvoy | Yes | GroundConvoy | Derived |
| AmphibiousCraft | Yes | AmphibiousCraft | Derived |
| Hybrid vehicle | Yes | HybridMedicalVehicle | Multiple inheritance |
| Virtual inheritance | Yes | AerialTransport / GroundConvoy | `virtual ReliefVehicle` |
| GPSBeacon composition | Yes | ReliefVehicle | Member object |
| EmergencyLifeSupportUnit composition | Yes | ReliefVehicle | Member object |
| Runtime polymorphism | Yes | IncidentCommander | `ReliefVehicle*` |
| Virtual destructor | Yes | ReliefVehicle | Declared virtual |
| Custom exceptions | Yes | Exceptions.h | Three classes derive from std::exception |
| Binary fstream | Yes | IncidentCommander | Binary save and validated load/display |
| Atomic/transaction-style update | Yes | IncidentCommander | Temporary files, results checked, then rename; temp files removed on failure |
| vector | Yes | Fleet/missions | STL vector |
| unordered_map | Yes | DistributionDepot | Inventory map |
| PriorityDispatchQueue<T> | Yes | PriorityDispatchQueue.h | Template class |
| Functor | Yes | Functors.h | `UrgentMedicalFirst` drives `PriorityDispatchQueue` in menu 11 |
| I/O manipulator | Yes | IOManipulators.h | `manifestHeader` used by inventory and low-stock tables |
| Menu system | Yes | IncidentCommander | run() |
| README | Yes | README.md | Documentation |
| VIVA | Yes | VIVA.md | 60 Q&A |
| TESTING | Yes | TESTING.md | Test cases |
| Input validation | Yes | IncidentCommander | Line-based readers, range checks, EOF handling |
| Static counters consistent | Yes | ReliefVehicle | `dispatch`, `returnToService`, copy ctor, `operator=`, destructor |
| Real exception paths | Yes | ReliefVehicle / IncidentCommander | Overweight, fuel and corridor exceptions come from real vehicle code |
| Clean build | Yes | whole project | Zero warnings with `-Wall -Wextra -Wpedantic`; sanitizer build clean |
