# VG investigation: supporting files

Evidence for the bomb-capacity investigation in the main [README](../../README.md).

## Setup

- Game code: commit `aacb59f`. Later commits only change documentation.
- Seed `0x4b4f545445`, gameplay layout, fixed 1/60 s step, bomb trace on (the same as `--trace-bombs`).
- Linux, g++-14. `trace_scenarios.cpp` drives the real `Application::run_frame` with fake key presses. `raylib_stub.cpp` replaces the raylib window, drawing and input calls, so no window is needed. The scenarios place the player on a tile before pressing a key and hold the enemies at tiles (13, 1..3), away from the test row 9. T5 sets the fuse to half a frame so that E and the fuse end fall in the same frame.
- `counted` is the value the capacity rule uses. `verified` comes from scanning the whole `EntityStore`. A difference prints `MISMATCH`.

## Scenarios

| Scenario | What it does |
| --- | --- |
| T1 | Three bombs, a fourth is rejected, a fuse frees a slot, place again |
| T2 | Three bombs, E, place again |
| T3 | One blast removes another own bomb |
| T4 | Restart with three active bombs, then fill to capacity again |
| T5 | E in the frame the fuse ends |
| T6 | Shield on, standing on own bombs: fuse and remote |

## Files

| File | What it is |
| --- | --- |
| `trace_scenarios.cpp` | Scenarios T1 to T6 |
| `raylib_stub.cpp` | Headless replacement for the raylib calls |
| `run_trace.sh` | Builds the scenarios with the game sources and runs them |
| `trace-current.txt` | Trace of the current code: 88 events, 0 MISMATCH |
| `counter-b.diff`, `trace-counter-b.txt` | Counter: −1 for every removed own bomb in `apply_entity_destructions`, reset in `start_round`. 0 MISMATCH |
| `counter-a.diff`, `trace-counter-a.txt` | Counter: −1 when the bomb's own detonation is resolved. 10 MISMATCH |
| `counter-c.diff`, `trace-counter-c.txt` | Like B but without the reset. 5 MISMATCH |

The counter versions are throwaway copies for this test. They were never committed to the game.

## Reproduce

From the repo root on Linux with g++-14:

```
bash docs/vg/run_trace.sh > trace.txt              # current code
bash docs/vg/run_trace.sh counter-a > trace-a.txt  # also counter-b, counter-c
grep -c MISMATCH trace.txt trace-a.txt
```

The script copies `kotte/` and `vendor/` to a temporary folder, applies the patch there if one is given, and builds. The repo is not changed.

On Windows the same trace comes from the game itself: run `kotte.release.exe --trace-bombs` from a console, then place 3 bombs, try a 4th, press E, place 2 bombs, press R and quit with Q.
