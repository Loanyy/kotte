# Bomberman World — Okan Ozcan

**Student:** Okan Ozcan

## How to play

- **Controls:** WASD or arrow keys move. Space places a bomb. E detonates all your bombs. F starts the shield. R restarts the round at any time. Q or Escape quits.
- **Objective and rules:** Kill all three enemies with bomb blasts to win. A blast or touching an enemy kills you. If you die in the same frame as the last enemy, you lose. After a win or a loss the world stops (movement, enemies, fuses, Space, E and F) until you press R.
  - Bombs: up to 3 at once, 2 s fuse, range 3. A bomb is solid. You can walk off a bomb you are standing on, but not back onto it. One bomb per tile.
  - Pickup: the cyan pickup adds 1 range to bombs placed after it (3 to 4, capped at 5). It is not solid, and blasts do not destroy it.
  - Remote (E): detonates every live bomb you own in that frame. Other bombs still explode when their fuse ends. If E and the fuse end in the same frame, the bomb explodes once.
  - Shield (F): once per round, 3 s. Blasts and enemy contact do not kill you, but walls, crates, bombs and enemies still block you. Pressing F again does not extend it. The player turns white and the HUD shows the time left, then `shield: used`.
- **Configurations / demo setup:** run `kotte.release.exe` (64-bit Windows). The default is the small gameplay map (15 × 11 tiles, 24 entities, 3 enemies). `--stress` opens the Week 6 stress world (200 × 120 tiles, 4570 entities, 294 enemies). `--trace-bombs` prints a `TRACE` line for every bomb event. The seed is fixed, so every round starts the same.
- **Running the Release ZIP:** raylib is linked statically, so no raylib DLL is needed. The build uses the Visual C++ runtime (`/MD`). If it is missing, install the included `vc_redist.x64.exe`.

## Features and validation

**Linux** checks are small test programs that run the real `run_frame` with fake key presses at a fixed 1/60 s step and a headless stand-in for raylib. Last run on 27 Oct on `aacb59f` (same game code as this commit): 161 checks and `kotte.tests`, 0 failures. **Windows** checks are my own hand-played checks of the Release x64 build.

| Feature | Check, observed result, and expected behaviour |
| --- | --- |
| 1. Player damage and death | Linux: standing on my own bomb gave `YOU LOSE (blast)`; walking into an enemy gave `YOU LOSE (enemy)`. An enemy passing close by without touching did not kill. Windows (18 Oct): both deaths seen. As expected. |
| 2. Round states and objective | Linux: a blast that killed the last enemy with me in range gave Lost; without me in range, Won. In Won, 60 frames of move, Space, E and F changed nothing. Windows (18–19 Oct): won and lost rounds, the world stopped. As expected. |
| 3. Safe restart | Linux: R with a live bomb and a blast on screen gave 24 entities, 3 enemies, no bombs or queued requests, and old handles no longer resolved. 100 restarts did not grow the store. Windows (23 Oct): restart with an active bomb was clean. As expected. |
| 4. Solid bombs with owner escape | Linux: after placing a bomb I walked off it in 5 frames, and walking back stopped at its edge. An enemy turned at a bomb and never overlapped it. Windows (19 Oct): correct by hand. As expected. |
| 5. Blast-range pickup | Linux: touching the pickup changed range 3 to 4 and removed it. A bomb placed before kept range 3; one placed after hit 12 tiles instead of 10. R restored the pickup and range 3. Windows (19 Oct): correct by hand. As expected. |
| #6 Multiple bombs | Linux: 3 bombs placed, the 4th rejected, a free slot after each blast. A blast that destroyed my other bomb freed both slots in that frame. Windows (19 and 23 Oct): limit and replacement correct. As expected. |
| #13 Remote detonation | Linux: E with no bombs did nothing; E with 3 bombs gave 3 blasts that frame and 0/3. E in the fuse-end frame gave one blast. Windows (23 Oct): E with 0, 1 and 3 bombs correct. As expected. |
| #8 Shield | Linux: with the shield on, my own bomb and 60 frames against an enemy did not kill me, and the enemy still blocked me. After it ended, the next blast and contact killed me. A second F did not extend it; R reset it. Windows (27 Oct): same by hand. As expected, apart from the small timing difference described under Known issues. |

**Known issues:**

- Crate collision boxes from the base code reach 12 px into the tile above and leave the lower part of their own tile free. A blast passing above a crate destroys it and stops there. A player standing in the lower free part can place a bomb on the crate's tile. I kept the base sizes.
- Enemy contact is a blocked move between player and enemy, so an enemy walking away from a standing player is not a hit.
- At a fixed 1/60 s step, the shield lasts 181 frames instead of 180 because the float countdown leaves about 2e-6 s. The extra 17 ms is kept.

## Sources and assistance

Built on the shared Kotte foundations (`2026-week-06-reference`, commit `19e2866`, by Ulf Benjaminsson, MIT licence in `LICENSE.txt`). Uses raylib 6.0 (zlib licence, `vendor/raylib/LICENSE`) and cpp_prngs (`vendor/cpp_prngs/LICENSE.md`). The Bombman art in `assets/` came with Kotte and is not used. No other external code or collaborators.

AI assistance: I used Claude Code (an AI coding agent) to draw up the roadmap and the daily plans, and while implementing them step by step. I chose the scope, the extensions and the rules. Claude Code wrote code changes for the planned steps, the Linux test programs, the trace scenarios and counter variants for the investigation, and first drafts of this README, `DEVLOG.md` and `ROADMAP.md`. I reviewed each change before merging and asked for changes where needed. I did all Windows builds and hand-played checks myself; the Windows results above are mine.

## Architectural investigation — VG only

### Decision

How should the game know if the player can place another bomb? Now `count_owned_bombs` (`application.cpp`) counts the handles in `active_bomb_handles_` that still resolve to a bomb owned by the player, and `apply_bomb_placements` uses it for the 3-bomb limit. The alternative is a stored counter: +1 when a bomb is created, −1 when `apply_entity_destructions` removes one of the player's bombs, and a reset in `start_round`.

### Method

`--trace-bombs` prints a line for every bomb event. `counted` is the value the limit uses; `verified` comes from scanning the whole `EntityStore`, and a difference prints `MISMATCH`. Six scripted scenarios (filling to capacity, E, a blast removing my other bomb (T3), R with three live bombs (T4), and more) ran on the current code and on throwaway counter versions: **B** is the counter above, **A** subtracts at the bomb's own detonation instead, **C** is B without the reset. I also ran `--trace-bombs` by hand on Windows on 23 Oct. Setup, scenarios, patches, raw traces and rerun steps are in [docs/vg](docs/vg/README.md).

### Evidence

| Version | MISMATCH lines | First wrong result |
| --- | --- | --- |
| Current (computed) | 0 of 88 | — |
| B: counter at the destruction function + reset | 0 of 88 | — |
| A: counter at detonation | 10 of 88 | T3: counted 1, verified 0 after a blast removed my second bomb |
| C: B without the reset | 5 of 79 | T4: after R counted 3, verified 0, every placement rejected |

My Windows trace also had no MISMATCH.

### Interpretation

A bomb leaves the world in three ways: its own detonation, being hit by another blast, and restart. The first two end in `apply_entity_destructions`, the only caller of `EntityStore::destroy`. Restart does not: `start_round` clears the store directly. The computed count does not care which path was used, because only live, valid bombs resolve.

B is correct because it updates at that one central point, so it would also cover later features that remove bombs through destroy requests. It breaks when code skips that point, such as restart (C) or a later direct `destroy` or `clear`. A updates on an event (the explosion) instead of the removal, so the T3 bomb is never subtracted and a slot leaks. A counter must also check the owner, which the computed count already does. I did not time either version.

### Conclusion

For one player and three bombs, counting live owned bombs is the better choice: it was correct on every tested path, including restart, with no extra state to keep in sync. A counter kept like B is worth it only if the count is read very often for many owners or long bomb lists. It stays correct only while all removals go through the central function and restart resets it, so I would keep the `verified` trace as a debug check.
