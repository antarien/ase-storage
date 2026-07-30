#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_sess_cln_sys.hpp
 * @brief       StorageKycdSessClnSystem - Retires the SES_* family of a dead session
 * @description StorageKycdLnkSystem publishes SES_IS_AUTHENTICATED, SES_CLEARANCE,
 *              SES_EXP_AT, SES_REALM_ID and SES_USER_ID_HI/_LO with owner = client
 *              entity. NetworkHubSyncSystem removes only the three keys it wrote
 *              itself when the client disconnects, so clearance, realm and the user
 *              hash outlive every session that ever authenticated. This system is
 *              the missing counterpart: the writer retires its own family, the same
 *              contract remove_keycard_ntf_family already applies to SES_KYCD_NTF_*.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    action/maintenance/cleanup
 * @schedule    Preservation
 * @created     2026-07-28
 * @modified    2026-07-28
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

class StorageKycdSessClnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdSessClnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
