#include "kotte/entity_store.hpp"

#include <cstdio>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>

namespace
{
    void require(bool condition, std::string_view message){
        if(!condition){
            throw std::runtime_error{std::string{message}};
        }
    }

    void stale_handle_cannot_access_a_reused_slot(){
        kotte::EntityStore store;
        store.reserve(1);

        const kotte::EntityHandle crate_handle = store.create({kotte::EntityKind::crate});
        require(store.contains(crate_handle), "A newly created handle must resolve.");
        require(store.live_count() == 1, "Creating an entity must increase the live count.");

        const kotte::EntityHandle wrong_generation{
            crate_handle.slot,
            static_cast<std::uint32_t>(crate_handle.generation + 1)
        };
        require(!store.contains(wrong_generation), "An occupied slot must reject the wrong generation.");
        const kotte::EntityStore& const_store = store;
        require(store.try_get(wrong_generation) == nullptr,
            "Mutable lookup must reject the wrong generation.");
        require(const_store.try_get(wrong_generation) == nullptr,
            "Const lookup must reject the wrong generation.");
        require(store.try_get({}) == nullptr,
            "Mutable lookup must reject an out-of-range handle.");
        require(!store.contains({}), "A default out-of-range handle must be rejected.");

        require(store.destroy(crate_handle), "A live handle must be destroyable.");
        require(!store.contains(crate_handle), "A destroyed handle must stop resolving immediately.");
        require(!store.contains(wrong_generation), "An empty slot must not resolve even with its current generation.");
        require(!store.destroy(crate_handle), "Repeated destruction must be harmless.");
        require(store.try_get(wrong_generation) == nullptr,
            "Mutable lookup must reject an empty slot at its current generation.");

        const kotte::EntityHandle enemy_handle = store.create({kotte::EntityKind::enemy});
        require(enemy_handle.slot == crate_handle.slot, "Creation should reuse the available slot.");
        require(enemy_handle.generation != crate_handle.generation, "Slot reuse must advance the generation.");
        require(!store.contains(crate_handle), "A stale handle must not resolve to the replacement entity.");
        require(store.try_get(crate_handle) == nullptr,
            "Mutable lookup must reject the previous occupant's handle.");
        require(!store.destroy(crate_handle),
            "Stale destruction must not remove a replacement entity.");
        require(store.contains(enemy_handle),
            "The replacement must survive stale destruction.");

        const kotte::Entity* enemy = store.try_get(enemy_handle);
        require(enemy != nullptr && enemy->kind == kotte::EntityKind::enemy,
            "The replacement handle must resolve to the replacement entity.");
        require(store.live_count() == 1, "Slot reuse must preserve the correct live count.");
        require(store.slot_count() == 1, "Slot reuse must not grow storage.");
    }

    void handles_survive_storage_reallocation(){
        kotte::EntityStore store;
        store.reserve(1);

        const kotte::EntityHandle player_handle = store.create({kotte::EntityKind::player});
        const kotte::EntityHandle crate_handle = store.create({kotte::EntityKind::crate});

        require(store.contains(player_handle), "Growing slot storage must not invalidate an existing handle.");
        require(store.contains(crate_handle), "The handle created after storage growth must resolve.");
    }

    void relationships_do_not_keep_stale_owners_alive(){
        kotte::EntityStore store;

        const kotte::EntityHandle player_handle = store.create({kotte::EntityKind::player});
        const kotte::EntityHandle bomb_handle = store.create({
            .kind = kotte::EntityKind::bomb,
            .bomb_state = kotte::BombState{
                .owner = player_handle,
                .fuse_remaining = 2.0f,
                .blast_range = 3
            }
        });

        require(store.destroy(player_handle), "The relationship must not own the player lifetime.");
        const kotte::EntityHandle replacement = store.create({kotte::EntityKind::enemy});

        const kotte::Entity* bomb = store.try_get(bomb_handle);
        if(bomb == nullptr || !bomb->bomb_state.has_value()){
            throw std::runtime_error{"The bomb must retain its relationship value."};
        }

        const kotte::EntityHandle retained_owner = bomb->bomb_state->owner;
        require(retained_owner == player_handle, "The bomb must still identify its original owner.");
        require(!store.contains(retained_owner), "A relationship must validate its handle before use.");
        require(replacement.slot == player_handle.slot, "The test must exercise owner-slot reuse.");
    }

    void clear_invalidates_every_old_handle(){
        kotte::EntityStore store;

        const kotte::EntityHandle player_handle = store.create({kotte::EntityKind::player});
        const kotte::EntityHandle crate_handle = store.create({kotte::EntityKind::crate});
        const kotte::EntityHandle enemy_handle = store.create({kotte::EntityKind::enemy});
        require(store.destroy(crate_handle), "Setup: one slot is already free before the clear.");

        store.clear();
        require(store.live_count() == 0, "Clearing must leave no live entities.");
        require(store.slot_count() == 3, "Clearing must keep the slots for reuse.");
        require(!store.contains(player_handle), "Clearing must invalidate the player handle.");
        require(!store.contains(enemy_handle), "Clearing must invalidate the enemy handle.");
        require(!store.destroy(enemy_handle), "Destroying an old handle after clearing must be harmless.");

        const kotte::EntityHandle first = store.create({kotte::EntityKind::player});
        const kotte::EntityHandle second = store.create({kotte::EntityKind::crate});
        const kotte::EntityHandle third = store.create({kotte::EntityKind::enemy});
        require(store.slot_count() == 3, "New entities must reuse the cleared slots.");
        require(store.live_count() == 3, "The store must count the new entities.");

        for(const kotte::EntityHandle old_handle : {player_handle, crate_handle, enemy_handle}){
            require(!store.contains(old_handle), "An old handle must not resolve to a new entity.");
            require(old_handle != first && old_handle != second && old_handle != third,
                "A new handle must never repeat an old one.");
        }
    }

    void a_new_store_can_repeat_old_handles(){
        // Why restart clears the store instead of replacing it.
        kotte::EntityStore old_store;
        const kotte::EntityHandle old_player = old_store.create({kotte::EntityKind::player});

        kotte::EntityStore new_store;
        const kotte::EntityHandle new_enemy = new_store.create({kotte::EntityKind::enemy});

        require(old_player == new_enemy, "A fresh store issues the same first handle again.");
        require(new_store.contains(old_player), "So an old handle would resolve to a new entity.");
    }
}

int main(){
    try{
        stale_handle_cannot_access_a_reused_slot();
        handles_survive_storage_reallocation();
        relationships_do_not_keep_stale_owners_alive();
        clear_invalidates_every_old_handle();
        a_new_store_can_repeat_old_handles();
        std::println("EntityStore tests passed.");
        return 0;
    } catch(const std::exception& error){
        std::println(stderr, "EntityStore tests failed: {}", error.what());
        return 1;
    }
}
