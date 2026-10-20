#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_ntfy_syn_sys.hpp
 * @brief       StorageKycdNtfySynSystem - spiegelt die Sternzeilen einer Ausstellungsanfrage
 * @description Die Hub-Haelfte des SYN-Schnitts an der Keycard-Anfrage. Dieses System LIEST die
 *              Hub-Zeile HUB_STG_KYCD_REQ und schreibt ihre Felder in zwei Bruecken-Zeilen auf
 *              derselben Anfrage-Entity; es rechnet nichts. Der Drain daneben rechnet und liest den
 *              Stern nicht.
 *
 *              WARUM DER SCHNITT. Die Regel HUB_IO_MIXED_WITH_MATH trennt das Lesen des Sterns von
 *              der Rechnung; sie nennt ihren Weg selbst — eine server-only Seite liest in eine
 *              `*_inp_*`-Zeile, die rechnende Seite nimmt sie von dort (WRFL_ASE_SYN_PATTERN).
 *
 *              DIE MELDUNG WANDERT MIT DEM LESEN. Fehlt ein Pflichtfeld, meldet es dieses System,
 *              und es entsteht KEINE Bruecke; der Drain sieht die Anfrage gar nicht erst.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @usecase     usecase:100264 - issue customer keycard
 * @created     2026-09-21
 * @modified    2026-10-12
 * @version     00.00.00.00001 [seed]
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
 * @brief StorageKycdNtfySynSystem - the star side of one keycard issuance request
 *
 * @schedule Ingestion - vor StorageKycdNtfyDrnSystem, das die Bruecke liest
 * @reads    Hub: the rows HUB_STG_KYCD_REQ, field by field
 * @writes   StorageInpKycdNtfyIdnComponent + StorageInpKycdNtfyGrntComponent auf der Anfrage
 */
class StorageKycdNtfySynSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdNtfySynSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
