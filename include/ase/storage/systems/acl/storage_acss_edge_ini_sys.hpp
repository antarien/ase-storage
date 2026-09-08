#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_acss_edge_ini_sys.hpp
 * @brief       StorageAcssEdgeIniSystem - Seeds the edge realm's ACL rules
 * @description Seeds the A/ACS rules of the edge_binaries realm as Entity-per-Item
 *              pairs (StorageAcssRuleComponent + StorageAcssCwrdComponent) across all
 *              four codeword axes - BINARY, SIG, SBOM, METADATA - plus the workflow
 *              drill asset at draft, and publishes the drill's initial stage and its
 *              customer-public serving verdict to the Hub.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
 * @parity      server_only
 * @created     2026-08-31
 * @modified    2026-08-31
 * @version     1.0.0
 *
 * WARUM DIE PARITAET HIER DEKLARIERT STEHT UND NICHT ERKANNT WIRD
 *   Das System SCHREIBT in den Hub (STG_WFLW_STAGE, STG_WFLW_PUB). Der Browser haelt
 *   einen LESE-Spiegel des Hubs, keine Schreibseite - die Autorenschaft liegt beim
 *   Server, also ist das System server_only. Das ist genau die erste der drei
 *   Unmoeglichkeiten, die der Generator selbst prueft.
 *
 *   Er sieht sie hier nur nicht, und der Grund ist messbar: sein Kommentar-Filter
 *   liest das `/` + `*` im ACL-Glob `release/` + Wildcard als Anfang eines
 *   Blockkommentars und schneidet bis zum naechsten `*` + `/` - das ist das
 *   auskommentierte Parameter-Paar in der tick()-Signatur. Beide hub::set-Aufrufe
 *   liegen dazwischen und verschwinden vor der Erkennung (gemessen 2026-08-31:
 *   4 Hub-Rufe roh, 0 nach dem Filter).
 *
 *   Die Deklaration UEBERSTIMMT die Erkennung also nicht - sie sagt, was der Filter
 *   nicht sehen kann. Faellt der Filterfehler weg, ist sie redundant, nie falsch.
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
 * @brief StorageAcssEdgeIniSystem - Seeds the edge realm's ACL rules
 *
 * Six rules, each an Entity-per-Item pair of an ACL rule and the codeword it
 * requires: the release tree (BINARY), *.sig signatures (SIG), *.spdx.json bills of
 * materials (SBOM), the two release manifests (METADATA), and the per-asset workflow
 * drill rule at draft. Each rule
 * also carries a StorageRuleIdnComponent whose match_hash is what
 * StorageAcssRslvSystem's ladder compares - a suffix rule additionally carries
 * StorageAcssRuleSufxTag and matches on the pattern WITHOUT its leading '*'.
 *
 * The realm the rules are scoped to is found through StorageRelmEdgeTag, whose only
 * producer is StorageEdgeIniSystem; that tag is the identity, so no scan over all
 * realms comparing id strings happens here.
 *
 * Dedication: the ACL RULES and nothing else. The realm belongs to
 * StorageEdgeIniSystem, the workflow transition graph to StorageWflwEdgeIniSystem.
 *
 * @schedule Initialization - Runs once at boot, after StorageEdgeIniSystem
 * @reads    StorageStaRelmComponent + StorageRelmEdgeTag (the realm to scope to)
 * @writes   StorageAcssRuleComponent, StorageRuleIdnComponent,
 *           StorageAcssRuleSufxTag, StorageAcssCwrdComponent,
 *           Hub STG_WFLW_STAGE + STG_WFLW_PUB (drill asset)
 * @depends  StorageEdgeIniSystem (creates the realm entity and its edge tag)
 */
class StorageAcssEdgeIniSystem : public ecs::System {
public:
    const char* name() const override { return "StorageAcssEdgeIniSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
