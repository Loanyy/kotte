# Assignment roadmap

Short status of the final assignment. The submission report is `README.md`.

- Start: `2026-week-06-reference` (`19e2866`), branch `final-assignment`
- Deadline: Friday 30 October 2026, 16:00
- Extensions: #6 multiple bombs, #13 remote detonation, #8 temporary shield

| Day | Date | Work | Status |
| --- | --- | --- | --- |
| 1 | 17 Oct | Small gameplay map, `EntityStore::clear` | Done |
| 2 | 18 Oct | Player damage, Playing/Won/Lost, safe restart | Done |
| 3 | 19 Oct | Solid bombs, range pickup, #6 multiple bombs | Done |
| 4 | 21 Oct | #13 remote detonation, bomb trace | Done |
| 5 | 27 Oct | #8 shield, VG investigation, README | Done |
| 6 | 27 Oct | Final check: Release x64 /W4 build, tests, HUD, ZIP | Done on `c7cdf48` |
| 7 | 27 Oct | Simplify submission docs | Docs only; ZIP to rebuild with the new README |
| — | by 30 Oct 16:00 | Submission on Studium (Okan) | Not submitted yet |

## Status

- Each day's features were checked on Linux (scripted) and on Windows by hand (Release x64 /W4, no warnings, tests pass). `--stress`: 4570 entities, 294 enemies.
- Final check on `c7cdf48` (Windows, 27 Oct): clean tree, Release x64 /W4 with 0 warnings, tests pass, HUD shows `blast range: 3` and `4`, no raylib DLL needed, the extracted ZIP runs, `--stress` and `--trace-bombs` work with no MISMATCH.
- After that only documentation changed. The game code and EXE are the same.
