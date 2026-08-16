#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_cncm_flt_sys.hpp
 * @brief       StorageCncmFltSystem - Realm concealment filter
 * @description Filters realm listings based on concealment tags for non-members
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-05
 * @modified    2026-04-05
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
 * @brief StorageCncmFltSystem - Hides concealed realms from non-members
 *
 * @schedule Integration - run_after StorageAcssChkSystem
 * @reads    StorageStaRelmComponent + the five realm-state tags (Public, Active,
 *           Conceal, Suspended, Archived) - all as View FILTERS, never as
 *           branches inside a loop
 * @writes   StorageRelmVisbTag on realms that may appear in an unauthenticated
 *           listing, removed the moment one condition stops holding
 * @depends  StorageIniSystem / StorageCredAcssRcvSystem classify realms on
 *           creation; this system only DERIVES from that classification
 *
 * SCOPE - read this before extending it
 *   Discoverability is viewer-INDEPENDENT, which is the only reason it may be
 *   cached on the realm entity that every viewer reads. "Is this concealed realm
 *   visible to user X" is a different question with a different answer per
 *   requester, and it is already answered inside StorageAcssChkSystem's ladder
 *   (public/owner exemption, then deny as realm_not_found so concealment leaks
 *   nothing). Moving that here would cache one user's answer for all of them.
 */
class StorageCncmFltSystem : public ecs::System {
public:
    const char* name() const override { return "StorageCncmFltSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
