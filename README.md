# Bomberman World — Okan Ozcan

**Student:** Okan Ozcan

## How to play

- **Controls:** WASD or arrow keys move. Space places a bomb. E detonates all your bombs. F starts the shield. R restarts the round at any time. Q or Escape quits.
- **Objective and rules:** Kill all three enemies with bomb blasts to win. A blast or touching an enemy kills you. If you die in the same frame as the last enemy, you lose. After a win or a loss, movement, enemies, fuses, bombs, E and F stop until you press R.
  - Bombs: up to 3 at once, 2 s fuse, range 3. A bomb is solid. You can walk off a bomb you are still standing on, but you cannot walk back onto it. One bomb per tile.
  - Pickup: the cyan pickup adds 1 range to bombs placed after it (3 to 4, the code caps it at 5). It is not solid, and blasts do not destroy it.
  - Remote (E): detonates every live bomb you own in that frame. Bombs you do not detonate still explode when the fuse ends. If E and the fuse end in the same frame, the bomb explodes once.
  - Shield (F): once per round, 3 s. Blasts and enemy contact do not kill you, but walls, crates, bombs and enemies still block you. Pressing F again does not extend it. The player turns white and the HUD shows the time left, then `shield: used`.
- **Configurations / demo setup:** Default is the small gameplay map (15 × 11 tiles, 24 entities, 3 enemies). `kotte.release.exe --stress` opens the Week 6 stress world (200 × 120 tiles, 4570 entities, 294 enemies). `--trace-bombs` prints a `TRACE` line to the console for every bomb event (see the investigation). The flags can be combined. The seed is fixed, so every round starts with the same world. The game uses no asset files.

## Features and validation

**Linux** checks are small test programs that run the real `run_frame` with fake key presses at a fixed 1/60 s step, with a headless stand-in for the raylib window calls. Some scenarios place the player or an enemy on a tile to set up the situation. Last run on 27 Oct on commit `aacb59f` (the same game code as this commit): 161 checks and `kotte.tests`, 0 failures. **Windows** checks are my own hand-played checks of the Release x64 build, with dates.

| Feature | Check, observed result, and expected behaviour |
| --- | --- |
| 1. Player damage and death | Linux: standing on my own bomb gave `YOU LOSE (blast)` when the fuse ended. Walking into an enemy gave `YOU LOSE (enemy)` after 7 frames. An enemy passing close by was a broad-phase candidate but never touched me, and the round stayed Playing. A blast and an enemy contact in the same frame caused one transition. Windows (18 Oct): both deaths seen. All as expected. |
| 2. Round states and objective | Linux: one blast that killed the last enemies with me in range gave Lost, not Won. Without me in range it gave Won. In Won I held move, Space, E and F for 60 frames: nothing moved and no bomb or shield started. In Lost the enemies and a live fuse stayed frozen for 180 frames. Windows (18–19 Oct): I won and lost rounds and the world stopped. As expected. |
| 3. Safe restart | Linux: R with a live bomb and a blast effect on screen gave a new round with 24 entities, 3 enemies, no bombs, effects or queued requests. The grid held exactly the live entities, and the old player, enemy and bomb handles no longer resolved. The same with the shield on also gave a ready shield. E and R in the same frame left no late blast. 100 restarts did not grow the store. Windows (23 Oct): restart with an active bomb was clean. As expected. |
| 4. Solid bombs with owner escape | Linux: I placed a bomb and walked left. I was off it after 5 frames, and walking back stopped at its edge. An enemy walking into a bomb turned and never overlapped it. Space again on the same tile still left one bomb. After the blast I walked across the old tile. Windows (19 Oct): solid bombs correct by hand. As expected. |
| 5. Blast-range pickup | Linux: standing 2 px from the pickup did not collect it. Touching it changed the range from 3 to 4 and removed it, and walking over its old tile again kept 4. Bomb A, placed before the pickup, kept range 3. Bomb B, placed after, hit 12 tiles instead of 10. R brought back the pickup and range 3. Windows (19 Oct): correct by hand. As expected. |
| #6 Multiple bombs (capacity 3) | Linux: five cycles of placing 3 bombs, a rejected 4th, one fuse ending and a new bomb. Each cycle ended at 3/3. A blast that destroyed my second bomb freed both slots in that frame. R with 3 live bombs gave 0/3, then 3 again and a rejected 4th. Windows (19 and 23 Oct): limit and replacement correct. As expected. |
| #13 Remote-detonated bombs | Linux: E with no bombs did nothing. E with 3 bombs gave 3 blasts in that frame and 0/3, and a new bomb could be placed and detonated right after. E in the frame the fuse ended gave exactly one blast. In Won and Lost, E and the fuse did nothing until R. Windows (23 Oct): E with 0, 1 and 3 bombs correct. As expected. |
| #8 Shield | Linux: with the shield on, my own bomb (fuse and E) and 60 frames of walking into an enemy did not kill me. The enemy and crates still blocked me. After the shield ended, the next blast gave `YOU LOSE (blast)` and the next contact `YOU LOSE (enemy)`. A second F did not extend it, and R reset it. Windows (27 Oct): the same rules by hand. As expected, except that it lasts 181 frames instead of 180 (see known issues). |

**Known issues:**
- Crate collision boxes (base code) reach 12 px into the tile above and leave the lower part of the crate's own tile free. A blast passing the tile above a crate therefore destroys that crate and stops there. A player standing in that lower part places the bomb on the crate's tile. I kept the base sizes.
- Enemy contact is a blocked move between the player and an enemy. Standing still next to an enemy that walks away is not a hit.
- At a fixed 1/60 s step the shield lasts 181 frames, not 180, because the float countdown leaves about 2e-6 s. The extra 17 ms is kept.

## Sources and assistance

This project builds on the shared Kotte foundations (`2026-week-06-reference`, commit `19e2866`, by Ulf Benjaminsson, MIT licence in `LICENSE.txt`). It uses raylib 6.0 (zlib licence, `vendor/raylib/LICENSE`) and cpp_prngs (`vendor/cpp_prngs/LICENSE.md`). The Bombman art in `assets/` came with Kotte and is not used. No other external code or collaborators.

AI assistance: I used Claude Code, an AI coding agent, for the whole project. I chose the scope, the three extensions and the rules (simple code in Kotte's style, small commits, no extra systems). It proposed the daily plan, wrote most of the code changes, the scripted Linux checks, the bomb trace scenarios and the counter variants, and drafted this README, `DEVLOG.md` and `ROADMAP.md`. I read each change before it was merged and asked for changes, for example shorter comments and simpler code. I did every Windows build and hand-played check myself, and the Windows results above are mine. The Linux results come from the AI-written test programs. Before the presentation I am going through every changed function and the investigation so I can explain them without help.

## Architectural investigation — VG only

### Decision

How should the game know whether the player can place another bomb? Now `count_owned_bombs` (`application.cpp`) counts the handles in `active_bomb_handles_` that still resolve to a bomb owned by the player. `apply_bomb_placements` uses it for the 3-bomb limit, and the HUD shows it. The alternative is a stored counter: +1 when a bomb is placed, −1 when one of the player's bombs is removed, read without a loop.

### Method

`--trace-bombs` prints one line for every round start, placement check, placement, detonation (fuse or E), removal and restart. `counted` is the value the capacity rule uses. `verified` is an independent scan of the whole `EntityStore` (`EntityStore::count_if`), and a difference prints `MISMATCH`. I ran six scripted scenarios on Linux on commit `aacb59f` (seed `0x4b4f545445`, gameplay layout, fixed 1/60 s step): T1 fill to 3, reject a 4th, let a fuse free a slot and place again; T2 three bombs and E; T3 one blast removes my other bomb; T4 R with three live bombs, then fill to 3 and reject a 4th; T5 E in the frame the fuse ends; T6 bombs while the shield is on. For the alternative I did not change the game. I ran the same scenarios on throwaway copies where `count_owned_bombs` returns a counter. **B** is the correctly kept counter: −1 in `apply_entity_destructions`, the only place that calls `EntityStore::destroy`, and a reset in `start_round`. **A** and **C** each miss one of those duties: A subtracts when the bomb's own detonation is resolved, and C has no reset. Files, setup and commands are in [docs/vg](docs/vg/README.md). I also ran `--trace-bombs` by hand on Windows on 23 Oct (fuse, E and restart).

### Evidence

| Version | MISMATCH lines | First wrong result |
| --- | --- | --- |
| Current (computed) | 0 of 88 | — |
| B: counter at the destruction point + reset | 0 of 88 | — |
| A: counter at detonation | 10 of 88 | T3: counted 1, verified 0 after the second bomb was removed by the first blast |
| C: like B, no reset | 5 of 79 | T4: after R counted 3, verified 0, every placement rejected |

```
           frame event   bomb  tile counted verified note
current T3   122 removed 25:1   6,9       0        0 hit by another blast
A       T3   122 removed 25:1   6,9       1        0 MISMATCH hit by another blast
A       T3   133 place   25:2   6,9       2        1 MISMATCH
C       T4    35 check      -   2,9       3        0 MISMATCH rejected, capacity full
```

My Windows trace on 23 Oct also had no MISMATCH. The other 5 lines in A come from T2, T4 and T6: the counter drops in the resolve phase, and the bombs are removed later in the same frame.

### Interpretation

A player bomb can leave the world in three ways: its own detonation (fuse or E), being hit by another blast (it is removed without exploding, because this game has no chain reactions), and restart, where `start_round` clears the whole store without going through `apply_entity_destructions`. The computed count does not depend on which way it happened. After the mutation phase the list cleanup drops dead handles and `try_get` rejects stale ones, so the count always matches what is alive.

B shows that a counter can be just as correct here, because every removal during a round passes one central function and restart is handled on its own. The cost is a duty to maintain it: every new way to create or remove a bomb (kicking, chain reactions, an owner dying, a second player) must update it, and a missed path fails silently. A is that mistake. It subtracts in the place that feels natural, the explosion, and misses the T3 bomb. Each such bomb leaks one slot, so after three leaks the player could never place a bomb again (my conclusion from the rule, not a run). A also drops the counter before the bomb is removed, so for part of the frame the number and the world disagree. C forgets that restart skips the central function.

Work: the computed count walks `active_bomb_handles_`, not the whole store. That list holds at most the 3 player bombs here. It is called once per placement request and once per frame for the HUD. I did not time either version.

### Conclusion

For one player and three bombs, counting the live, valid owned bombs is the better choice. It stayed correct on every tested removal path, including restart, without extra bookkeeping, and the work is a walk over at most three handles. A stored counter is worth it when the count is read very often for many owners, for example many AI bombers checking every frame, or when the bomb list gets long. Then I would update it only in the central destruction function plus restart, as in B, and keep the `verified` trace as a debug check.
