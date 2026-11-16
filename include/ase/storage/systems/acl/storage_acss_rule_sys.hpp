#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_acss_rule_sys.hpp
 * @brief       StorageAcssRuleSystem - Match the most specific ACL rule for each visible request
 * @description The second lookup of the Section 14.1 access ladder: for a request whose keycard
 *              and realm StorageAcssRslvSystem already accepted (StorageAcssRelmTag), find the
 *              ACL rule that governs its path - location rules by prefix, extension rules by
 *              suffix, the most specific one wins - and turn the result into
 *              StorageStaAcssRslvComponent plus StorageAcssRslvTag, the entry into the gates of
 *              StorageAcssChkSystem. It denies nothing: a path without a rule keeps the PUBLIC
 *              protection of the realm.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @design      DSGN_021
 * @usecase     usecase:100265 - check access clearance
 * @actor       Provider Credential Keeper (PROVCREKEE)
 * @created     2026-11-16
 * @modified    2026-11-16
 * @version     00.00.39.00039 [core]
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

class StorageAcssRuleSystem : public ecs::System {
public:
    const char* name() const override { return "StorageAcssRuleSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
