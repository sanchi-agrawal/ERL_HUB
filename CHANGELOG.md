# Changelog — fixes applied to ERL-HUB

## Bugs fixed
1. **Crash on negative restock** — amounts are now validated and the operation is also wrapped in `try/catch`; the menu loop has a catch-all so no exception can terminate the program.
2. **Infinite loop at end of input** — all input readers detect EOF and exit cleanly.
3. **Repeat dispatch** — a completed mission or an already-dispatched vehicle is refused; the manifest is cleared before each load, so cargo can no longer stack up or be double counted.
4. **Wrong operational-vehicle counter** — `dispatch()` now decrements it; `returnToService()` restores it; `ReliefVehicle::operator=` keeps it consistent.
5. **Missing validation** — priority 1–10, positive distance/capacity/fuel/quantity/weight, duplicate vehicle IDs, empty input, trailing garbage (`5x`).
6. **Failed dispatch** — partial manifest loads are rolled back; missions can now be cancelled (cargo returns to the depot).

## Features wired up
- Real `DamagedCorridorException` path: menu 18 marks a corridor; `refuel` and `clearManifest` are reachable through menu 19.
- Exception demo (menu 15) now triggers each exception through the real vehicle code instead of throwing it by hand.
- **Friend class** genuinely used: `IncidentCommander` reads `DistributionDepot::stock` (menu 21) and edits `Mission::priority` (menu 22 / `emergencyTriageOverride`).
- **Functor** `UrgentMedicalFirst` now drives the `PriorityDispatchQueue` (redundant sort removed); ties broken by mission age.
- **I/O manipulator** `manifestHeader`, `printLine` and `payloadPercentage` are used in the inventory and vehicle tables.
- **Load records** now reads, validates and prints the saved missions and vehicles.
- Medical-capable vehicle recommendation (`isMedicalCapable()` virtual) for urgent medical missions.

## Code quality
- Zero warnings with `-Wall -Wextra -Wpedantic` (added `ReliefCargo::operator=`, rule of three on `ReliefVehicle`).
- `<cstdio>` included; `remove`/`rename` results checked; temp files cleaned on failure.
- Binary files carry an `ERL1` signature; old-format files are detected, not misread.
- Vehicle table column widths fixed (long type names no longer run into the next column).
- Removed the stale `erl_hub` binary and old `.bin` files from the package; added `Makefile` and `data/.gitkeep`.
