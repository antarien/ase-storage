#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_ntfy_drn_sys.hpp
 * @brief       StorageKycdNtfyDrnSystem - Bridges SDK Hub keycard-notify signals into Storage components
 * @description Reads SES_KYCD_NTF_* Hub values from request entities tagged with
 *              hub::HubStgKycdPendTag and emplaces StorageReqKycdComponent on the
 *              same entity.  This replaces the direct component emplace that used
 *              to live in the SDK — SDK is cross-server (Engine/Replica/World) and
 *              can not link ase::storage, so the hand-off crosses the module
 *              boundary via Hub keys + a Hub-owned discovery tag.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-04-16
 * @modified    2026-04-16
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
 * @brief StorageKycdNtfyDrnSystem - Bridges SDK Hub keycard-notify signals
 *
 * @schedule Ingestion - runs before StorageKycdReqDrnSystem so the downstream
 *           collect-then-destroy pass sees StorageReqKycdComponent.
 * @reads    hub::HubStgKycdPendTag + SES_KYCD_NTF_{USER_ID,CLRN,REALM_ID,EXP_AT} (Hub)
 * @writes   StorageReqKycdComponent on the same request entity (leaves HubStgKycdPendTag
 *           in place for StorageKycdReqDrnSystem consumption)
 */
class StorageKycdNtfyDrnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdNtfyDrnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
