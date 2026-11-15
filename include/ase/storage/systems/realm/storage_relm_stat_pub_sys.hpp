#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_relm_stat_pub_sys.hpp
 * @brief       StorageRelmStatPubSystem - announces the tier and the default protection of every realm on the star
 * @description Reads StorageStaRelmComponent of every realm entity and writes its tier to
 *              STG_RELM_TIER and its default protection to STG_RELM_PROT, both under the owner
 *              entt::hashed_string(relm.id), the owner StorageQuotChkSystem publishes the quota rows
 *              under and GET /api/realms/:realmId reads them from. Hub write only, no calculation:
 *              the numbers are set at realm registration and are passed on unchanged.
 *
 * @design      DSGN_021
 * @usecase     usecase:100049 - publish a hub value
 * @actor       Protocol Channel Steward (PROTCHASTE)
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @parity      server_only
 * @schedule    Observation
 * @created     2026-11-07
 * @modified    2026-11-07
 * @version     00.00.00.00000 [seed]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
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
 * @brief StorageRelmStatPubSystem - tier and default protection of every realm, announced per tick
 *
 * @schedule Observation - beside StorageQuotChkSystem, which announces the quota rows of the same owner
 * @reads    StorageStaRelmComponent (every realm entity)
 * @writes   Hub: STG_RELM_TIER, STG_RELM_PROT under hashed_string(relm.id)
 */
class StorageRelmStatPubSystem : public ecs::System {
public:
    const char* name() const override { return "StorageRelmStatPubSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
