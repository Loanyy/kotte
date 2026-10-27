# Bomberman World — Bomberman World

**Student:** Okan Ozcan

## How to play

- **Controls:** WASD or arrow keys move. Space places a bomb on your tile. E detonates all your bombs. F starts the shield. R restarts the round. Q or Escape quits.
- **Objective and rules:** Kill all three enemies with bomb blasts to win. A blast or touching an enemy kills you. If the same frame kills you and the last enemy, you lose. After a win or a loss the world stops until you press R. You can have 3 bombs at once; a bomb is solid, but you can walk off a bomb you are still standing on. The cyan pickup adds 1 blast range (3 to 4, max 5); bombs already placed keep their old range. E detonates every bomb you own at once; the fuse (2 s) still works if you do not press E. F gives a 3 s shield once per round: blasts and enemy contact do not kill you, but walls, crates, bombs and enemies still block you. Pressing F again does not extend it. The player turns light white and the HUD shows the time left, then `shield: used`.
- **Configurations / demo setup:** Default is the small gameplay map (15 × 11 tiles, 3 enemies). `kotte.exe --stress` opens the Week 6 stress world (200 × 120 tiles, 4570 entities, 294 enemies). `kotte.exe --trace-bombs` prints one `TRACE` line to the console for every bomb event (see the investigation below). Both flags can be combined. The seed is fixed, so every round starts with the same world.

## Features and validation

Most checks below were scripted: a small test program drives the real `run_frame` with fake key presses at a fixed 1/60 s step on Linux (a headless stand-in replaces the raylib window calls). Checks marked *Windows* I did by hand in the Release build.

| Feature | Check, observed result, and expected behaviour |
| --- | --- |
| 1. Player damage and death | Stood on my own bomb until the fuse ended: `YOU LOSE (blast)`. Walked into a parked enemy: `YOU LOSE (enemy)` within 10 frames. *Windows (18 Oct):* both deaths seen. Both match the rules. |
| 2. Round states and objective | One blast killed the last three enemies with me in range: Lost, not Won, as intended. Without me in range: Won. Then I held move, Space, E and F for 60 frames: the player did not move and no bomb or shield started. In Lost, enemies and a live fuse stayed frozen for 180 frames. All as expected. |
| 3. Safe restart | Pressed R with the shield on, one bomb live and a blast effect on screen: new round with 24 entities, 3 enemies, no bombs or effects, shield ready, and old bomb and player handles no longer resolve. Pressed E and R in the same frame: no late blast. *Windows (23 Oct):* restart with an active bomb correct. Matches. |
| 4. Solid bombs with owner escape | Placed a bomb and walked left: off the bomb after 5 frames, then walking back stopped at the bomb's edge. An enemy walking into a bomb turned. A second bomb on the same tile was rejected. After the blast I walked across the old tile. All as expected. |
| 5. Blast-range pickup | Took the pickup while bomb A was live: A kept range 3 and the next bomb had range 4. The pickup is collected once, a blast does not destroy it, and R brings it back with range 3. Matches. |
| #6 Multiple bombs (3) | Placed 3 bombs, the 4th was rejected, then the first fuse freed one slot and the next press was accepted (trace T1: `counted` and `verified` equal on every line). A blast that destroyed my second bomb returned both slots in the same frame. Five fill-and-empty cycles in a row all behaved the same. |
| #13 Remote-detonated bombs | E with 3 bombs: 3 blasts in the same frame, capacity back to 0/3 and a new bomb could be placed the next frame. E in the same frame as the fuse ending: one blast and one trace line (`fuse also ended`). E in Won or Lost: no blast. *Windows (23 Oct):* E with 0, 1 and 3 bombs correct. |
| #8 Shield | With the shield on, my own bomb (fuse and E) and an enemy push did not kill me; the enemy still blocked me. After it ended, the next blast gave `YOU LOSE (blast)` and the next contact `YOU LOSE (enemy)`. A blast in the frame the timer reaches 0 kills; one frame earlier it does not. Second F: no extension. At 1/60 s the shield lasted 181 frames, not 180, because float steps leave about 2e-6 s; I accepted the extra 17 ms. |

**Known issues:**
- Crate collision boxes (from the base code) reach 12 px into the tile above. A blast that passes the tile directly above a crate destroys that crate and stops there; for example a bomb at tile (2, 1) destroys the crate at (4, 2) and its ray ends at (4, 1). Walking along that row also touches the crate a little early. I left the base sizes unchanged.
- Enemy contact is counted when a move between the player and an enemy is blocked. Standing still next to an enemy that walks away is not a hit.

## Sources and assistance

This project builds on the shared Kotte foundations (`2026-week-06-reference`, commit `19e2866`, by Ulf Benjaminsson, MIT licence in `LICENSE.txt`). It uses raylib 6.0 (zlib licence, `vendor/raylib/LICENSE`) and cpp_prngs (`vendor/cpp_prngs/LICENSE.md`). The game draws simple shapes; the Bombman art in `assets/` came with Kotte (CC0) and is not used.

AI assistance: I used Claude Code, an AI coding agent, through the whole project. I set the scope, the extensions and the working rules, and it planned the days with me, wrote most of the code changes, wrote the scripted checks and the trace scenarios, and drafted this README, `DEVLOG.md` and `ROADMAP.md`. I read the diffs, asked for changes, and did every Windows build and hand-played check myself. The scripted Linux results above come from those AI-written test programs. I am still studying parts of the code for the presentation.

## Architectural investigation — VG only

### Decision

Should the three-bomb limit be computed from the live bombs, or kept in a separate counter? Now `count_owned_bombs` (`application.cpp`) walks `active_bomb_handles_` and counts the handles that still resolve to a bomb owned by the player. `apply_bomb_placements` uses it for the capacity check and the HUD shows it. A plausible alternative is a `player_bomb_count_` member: +1 when a bomb is placed, −1 when it goes away, read in O(1).

### Method

`--trace-bombs` prints a line for every round start, placement check, placement, detonation (fuse or remote), removal and restart. `counted` is the value the capacity check uses; `verified` is an independent scan of the whole `EntityStore` (`EntityStore::count_if`), and a difference prints `MISMATCH`. I ran six scenarios on commit `0c360ca` (seed `0x4b4f545445`, gameplay layout, fixed 1/60 s step, scripted keys on Linux): T1 fill to 3, reject a 4th, let a fuse free a slot and place again; T2 three bombs and E; T3 one blast removes my other bomb; T4 R with three live bombs, then fill to 3 again and reject a 4th; T5 E in the frame the fuse ends; T6 the same with the shield on. To test the alternative without committing it, I made three throwaway builds where `count_owned_bombs` returns a counter: **A** −1 when the bomb's own detonation is resolved; **B** −1 in `apply_entity_destructions` for every removed own bomb, reset in `start_round`; **C** like B but without the reset. Finally I timed `count_owned_bombs` and a full store scan (`-O2`, Linux). The raw traces, patches and scripts are kept with my project notes.

### Evidence

| Build | MISMATCH lines (same six scenarios) | What went wrong |
| --- | --- | --- |
| Current (computed) | 0 | — |
| A: −1 on own detonation | 10 | T3: the bomb destroyed by another blast was never subtracted |
| B: −1 on any removal, reset on R | 0 | — |
| C: like B, no reset | 5 | T4: after R, every placement was rejected |

```
         frame event        bomb   tile counted verified note
current  T3 122 removed      25:1    6,9       0        0 hit by another blast
A        T3 122 removed      25:1    6,9       1        0 MISMATCH hit by another blast
A        T3 133 place        25:2    6,9       2        1 MISMATCH
C        T4  35 check           -    2,9       3        0 MISMATCH rejected, capacity full
```

Earlier runs agree: on 21 Oct the real game binary with `--trace-bombs` (Linux) and on 23 Oct my own Windows run had no MISMATCH, and a deliberately broken version (list size without cleanup) did show MISMATCH. Cost: `count_owned_bombs` took 7–9 ns per call with 3 bombs, both in the 27-entity and the 4573-entity world; the full store scan took 14–20 ns and 2.4–2.6 µs. I did not time a counter.

### Interpretation

A bomb can leave the world in three ways: its own detonation (fuse or E), being hit by another blast (it is removed without exploding, because there are no chain reactions), and restart, where `start_round` clears the whole store. The computed count does not care which way it happened. After the mutation phase the list cleanup drops dead handles and `try_get` rejects stale ones, so the count is always taken from what is really alive. A counter is only right if every one of those paths updates it. Variant A is the natural mistake, "subtract when the bomb explodes": it misses the edge case in T3 and leaks one slot each time. After three leaks the player could never place a bomb again, and nothing would show it except the trace. Variant C forgets restart. Variant B proves a counter can work, but only because all removals go through one function and restart was remembered. Every future removal path (kicking, chain reactions, an owner dying) would be another place to keep it in sync. A also shows a smaller problem: its counter dropped in the resolve phase, before the bombs were actually removed, so for part of the frame the number and the world disagreed.

Cost: the computed count walks the short active-bomb list, not the store, which is why its time stayed the same in the stress world while the full scan grew. With at most 3 bombs and a few calls per frame that is well under a microsecond in a 16.7 ms frame, so a counter could only save very little here.

### Conclusion

For this game, with one player, three bombs and a short list of active bombs, computing the count from the live bombs is the better choice. It was correct in every tested removal path, including restart, without extra bookkeeping, and its measured cost is a few nanoseconds per call. A counter becomes worth it when the count is read very often for many owners, for example hundreds of AI bombers checking capacity every frame. Then it should be updated in the single destruction function plus restart (variant B), and the `verified` trace should stay as a debug check.
