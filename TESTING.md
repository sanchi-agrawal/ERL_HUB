# TESTING — ERL-HUB

All tests below were run against the final build (`g++ -std=c++17 -Wall -Wextra -Wpedantic`, zero warnings) and an AddressSanitizer/UndefinedBehaviorSanitizer build (no findings).

## Functional tests
| Test | Steps | Expected result |
|---|---|---|
| Vehicle registration | Menu 1, valid type/ID/name/capacity/fuel | Vehicle appears in menu 2 |
| Duplicate vehicle ID | Menu 1 with `GV01` | "already exists", fleet unchanged |
| Cargo addition | Menu 3, new ID | Cargo appears in menu 5 |
| Cargo with existing ID | Menu 3 with `C001` | Quantity is added to existing stock |
| Restock | Menu 4, `C001`, 10 | Quantity increases by 10 |
| Negative / zero restock | Menu 4, `-5` then `0` | Both rejected, program continues |
| Mission validation | Menu 6 with priority `99`/`0`, distance `-50`/`0` | Each re-prompted until valid |
| Cargo allocation | Menu 7 | Depot quantity decreases, mission holds the cargo |
| Over-allocation | Menu 7 with more than the stock | "Not enough stock", stock unchanged |
| Unknown IDs | Menu 7/8 with bad mission, cargo or vehicle ID | "not found", program continues |
| Dispatch | Menu 8 with allocated mission and READY vehicle | Dispatched, fuel reduced, mission completed |
| Dispatch without cargo | Menu 8 on a mission with nothing allocated | Refused with a hint |
| Repeat dispatch | Menu 8 on a completed mission | "already completed" |
| Busy vehicle | Menu 8 with a vehicle that is DISPATCHED | Refused; hint to use menu 19 |
| Statistics counter | Dispatch one vehicle, then menu 16 | Available vehicles drops from 5 to 4; tonnage updates |
| Return to service | Menu 19 on the dispatched vehicle | Available vehicles back to 5, manifest empty, fuel restored |
| Overweight (real path) | Allocate 200 × Clean Water (4 t), dispatch `AV02` (1 t) | `CargoOverweightException` caught, manifest rolled back, mission still pending; dispatching `GV01` then succeeds |
| Damaged corridor (real path) | Menu 18 mark `GV01` damaged, dispatch it | `DamagedCorridorException` caught; after clearing, dispatch succeeds |
| Fuel depletion (real path) | Menu 15, option 2 (or a very long mission) | `FuelDepletionException` caught |
| Exception demo | Menu 15, options 1–3 | Each specific exception type is caught through the real vehicle code |
| Cancel mission | Menu 20 on a pending mission with allocated cargo | Mission removed, cargo returned to depot |
| Low-stock report | Menu 21 with threshold 150 | Lists cargo below 150 (friend access to `stock`) |
| Triage override | Menu 22, mission 1 → priority 10 | Priority changes (friend access to `Mission`); invalid values rejected |
| Priority order | Menu 11 | Urgent medical first, then higher priority, then older mission |
| Mission priorities | Menu 10 | Sorted by priority, stable for ties |
| Polymorphism | Menu 12 | Virtual route evaluation through `ReliefVehicle*` |
| Copy constructor | Menu 17 | Copied cargo changes without changing the original |

## Input and file tests
| Test | Steps | Expected result |
|---|---|---|
| Invalid input | Letters, `5x`, empty line at any prompt | Rejected with a message, program continues |
| End of input | Close stdin (Ctrl+D / Ctrl+Z) at a prompt | Clean exit, no endless loop |
| Save twice | Menu 13 two times in a row | Both succeed (old file replaced) |
| Load | Menu 14 after saving | Prints every saved mission and vehicle record |
| Load with no files | Menu 14 on a fresh folder | "No transit log file found" |
| Corrupt file | Replace `data/transit_logs.bin` with junk, menu 14 | "corrupt or from an incompatible version" |
| Missing `data/` | Rename the folder, menu 13 | Clear error message, no leftover temp files |

## Object-model checks (unit test, AddressSanitizer build)
| Test | Expected result |
|---|---|
| Copy constructor of a dispatched vehicle | Copy is not counted as available; manifest deep-copied |
| `operator=` between available and dispatched vehicles | Static counter stays correct |
| Mutating the copy's manifest | Original manifest unchanged (deep copy) |
| Self-assignment | Safe |
| All vehicles destroyed | Counter returns to 0 |
