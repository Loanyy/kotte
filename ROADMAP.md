# Assignment roadmap

Working notes for the final assignment. The submission report is `README.md`.

- Start: `2026-week-06-reference` (`19e2866`), branch `final-assignment`
- Deadline: Friday 30 October 2026, 16:00
- Extensions: #6 multiple bombs, #13 remote detonation, #8 temporary shield

## Plan

| Day | Date | Work |
| --- | --- | --- |
| 1 | 17 Oct | Baseline check, small gameplay map, `EntityStore::clear` |
| 2 | 18 Oct | Player damage, Playing/Won/Lost, safe restart |
| 3 | 19 Oct | Solid bombs, range pickup, #6 multiple bombs |
| 4 | 20 Oct | #13 remote detonation, bomb trace for the VG study |
| 5 | 21 Oct | #8 shield (evening) |
| 6 | 22 Oct | Shield, bomb and restart interactions (evening) |
| 7 | 23 Oct | Full check of all eight features, feature freeze (evening) |
| 8–9 | 24–25 Oct | Learn the code, fix remaining defects |
| 10 | 26 Oct | VG trace and evidence table |
| 11 | 27 Oct | README |
| 12 | 28 Oct | Presentation rehearsal |
| 13 | 29 Oct | Release /W4 rebuild, ZIP check, final commit link |

## Status

Only checks that were actually run are marked done.

| Item | Linux | Windows (Visual Studio) |
| --- | --- | --- |
| Baseline `19e2866`: Release x64 build, `kotte.tests` | Tests passed | Build and tests passed |
| Baseline play check | Not possible | Bombs destroy crates, Q quits |
| Gameplay map: 15x11, 23 entities, 3 enemies | Opened | Opened as expected |
| `--stress`: 200x120, 4570 entities, 294 enemies | Same as baseline | Same numbers |
| `EntityStore::clear` tests | Passed | Release rebuild, no warnings, tests passed |
| Merged `final-assignment` | Tests passed, game opens | Rebuild 2 succeeded, no warnings; tests passed; both layouts correct |
| Day 2: player damage, Won/Lost, restart | Tests passed, scripted play checks passed | Rebuild 2 succeeded, no warnings; tests passed; blast and enemy deaths, win, freeze and restart work |
| `--stress` after day 2 | 4570 entities, 294 enemies | 200x120, 4570 entities, 294 enemies |

Day 1 done. Day 2 done.
