#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_idn_idx_sys.hpp
 * @brief       StorageIdnIdxSystem - indexes the identities the frame resolves by key
 * @description Realms by their id hash, network clients by their published NET_CLAI_ID
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-08-16
 * @modified    2026-08-16
 * @version     1.0.0
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants → types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::storage {

/**
 * @brief StorageIdnIdxSystem - entity lookups by published identifier
 *
 * WHY A SECOND INDEX BUILDER
 *   One rebuild cannot serve both stages of the frame, and the frame order is
 *   Reception → Ingestion → Integration. Realms are created in Reception and read from
 *   Ingestion onwards; keycards are minted DURING Ingestion and must reach the
 *   Integration ladder in the SAME frame. A single early build would hide a freshly
 *   minted keycard for one tick - its owner's first access would be refused - and a
 *   single late build leaves the Ingestion readers with nothing to read.
 *
 *   So the relations are split by the stage that can honestly produce them, and every
 *   relation has exactly ONE writer: this system owns realms and clients,
 *   StorageAcssIdxSystem owns rules, links, codewords, tasks and edges.
 *
 * @schedule Ingestion - Runs first, before the keycard drain and link systems
 * @reads    StorageRelmIdnComponent, hub::HubNetClaiRdyTag
 * @writes   StorageAcssIndexResourceManager (registry.ctx(), not a Component)
 * @depends  StorageAcssIdxSystem::on_start registers the manager in ctx()
 */
class StorageIdnIdxSystem : public ecs::System {
public:
    const char* name() const override { return "StorageIdnIdxSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
