/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_acss_rslv_sys.cpp
 * @brief       StorageAcssRslvSystem - Resolve realm and ACL rule for each pending request
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-08-29
 * @modified    2026-08-29
 * @version     1.0.0
 *
 * TRENNUNG 2026-08-29 — NACHSCHLAGEN UND ENTSCHEIDEN WAREN EINE tick()
 *
 * Die Zugriffsleiter nach ARCH Section 14.1 stellt zwei verschiedene Arten von Frage, und sie
 * standen in derselben Funktion:
 *
 *   NACHSCHLAGEN (hier)  Welches Revier ist gemeint, wem gehoert es, ist es oeffentlich, ist es
 *                        verschleiert, und welche ACL-Regel regiert genau diesen Pfad? Das sind
 *                        Index-Zugriffe, Pfad-Vorfahren und ein Spezifitaets-Vergleich.
 *   ENTSCHEIDEN          Darf DIESER Anrufer? Gitter, Freigabestufe, Codewort, Recht, Etikett,
 *                        Kenntnisnahme, Kontingent - StorageAcssChkSystem.
 *
 * WAS DIE TRENNUNG WERT IST: eine Aenderung am Regelabgleich - etwa an der Frage, welche von
 * zwei ueberlappenden Regeln gewinnt - war vorher eine Aenderung an derselben Datei, in der auch
 * die Freigabestufen-Pruefung steht. Das ist die teuerste Nachbarschaft, die es hier geben kann.
 *
 * DIE ERSTEN BEIDEN SPROSSEN BLEIBEN HIER, weil sie keine Erlaubnis erteilen, sondern eine
 * Adresse aufloesen: ohne gueltige Keycard und ohne sichtbares Revier gibt es nichts zu
 * entscheiden. Beide Ausgaenge sind Abweisungen mit Pruefspur, und die Verschleierung leckt
 * dabei nichts - sie weist als realm_not_found ab, nie als access_denied.
 *
 * KEIN VORAB-GRANT. Der oeffentliche Raum und die Eigentuemer-Macht sind INNERHALB der Leiter
 * modelliert (PUBLIC-Schutzregel, Eigentuemer-Vorgabe als wirksame Keycard) - sie ueberspringen
 * kein Tor, sie machen die Tore fuer diesen Fall gegenstandslos.
 *
 * CAUSAL CHAIN (CHAIN_STORAGE_ACSS_RSLV: Request → Resolution)
 *
 *   [HTTP-Route legt Anfrage + Keycard-Daten an]
 *          │
 *          │ StorageReqAcssComponent + StorageReqCredComponent
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAcssRslvSystem                         │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    StorageReqAcssComponent (Revier, Projekt, Pfad, Aktion)  │
 *   │    StorageReqCredComponent (Keycard-Identitaet)             │
 *   │    StorageStaRelmComponent + StorageRelmIdnComponent        │
 *   │    StorageRelmGlobTag / StorageRelmCncmTag (Klassifikation) │
 *   │    StorageAcssRuleComponent + StorageRuleIdnComponent       │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    StorageStaAcssRslvComponent (Schutzstufe, Regel, Etikett)│
 *   │    StorageAcssRslvTag (die Leiter darf ansetzen)            │
 *   │    StorageAcssOwnrTag (Eigentuemer-Vorgabe gilt)            │
 *   │    StorageAcssDenyTag + Pruefspur bei Sprosse 1 und 2       │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ StorageAcssRslvTag
 *          ▼
 *   StorageAcssChkSystem - die Tore
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (from ase-storage internal):
 *   StorageReqAcssComponent  → realm, project, path and action of the pending request
 *   StorageReqCredComponent  → the keycard identity the HTTP route validated
 *   StorageStaRelmComponent  → the realm record the request names, reached in O(1)
 *   StorageAcssRuleComponent → the ACL rules of that realm, location and extension buckets
 *
 * WRITES (to ase-storage internal):
 *   StorageStaAcssRslvComponent → protection level, matched rule and its workflow label
 *   StorageAcssRslvTag          → the gate ladder may run on this request
 *   StorageAcssOwnrTag          → the owner preset applies for the whole ladder
 *   StorageAcssDenyTag          → rung 1 and 2 denials, each with an audit record
 *
 * ECS SYSTEM IMPLEMENTATION COMPLIANCE
 *
 * [ ] Layer dependencies checked (only depend on lower layers)
 * [ ] Existing functions checked (ase-math, ase-utils, ase-containers)
 * [ ] Abbreviations defined in types.hpp or documentation
 * [ ] types.hpp created with all constants and enums
 * [ ] STATELESS? No member variables?
 * [ ] Views created on demand, not stored?
 * [ ] NO direct calls to other systems?
 * [ ] Communication only via Components?
 * [ ] Helpers in anonymous namespace (NOT static!)?
 * [ ] Math functions from ase-math (Layer 0)?
 * [ ] NO file-level static/constexpr?
 * [ ] Registered in Module with correct Schedule?
 * [ ] Filename matches convention?
 * [ ] Class name derived correctly from filename?
 * [ ] Using Deferred Deletion Pattern? (Tag + Batch Destroy)
 * [ ] NO destroy() on other entities during iteration?
 * [ ] Cleanup System in Schedule::Conclusion?
 * [ ] NO local arrays/vectors for collection?
 * [ ] 1 File = 1 System?
 * [ ] Folder structure matches convention?
 * [ ] components/, systems/, src/ have IDENTICAL subfolder structure?
 * [ ] Layer dependencies respected (no upward dependencies)?
 * [ ] NO inline nlohmann::json + .dump() in broadcast systems?
 * [ ] Serializer functions in anonymous namespace?
 * [ ] *NetBctReqSystem + *NetBctSndSystem pattern?
 * [ ] Math functions from ase-math? (lerp, clamp, noise)
 * [ ] Containers from ase-containers? (RingBuffer)
 * [ ] Types from ase-types? (Result, Option)
 * [ ] Utils from ase-utils? (UUID, hash)
 * [ ] No duplicate functionality across modules?
 * [ ] ONLY primitive types: int, float, uint32_t, bool, etc.
 * [ ] ONLY ase-math for math (NO std::min, std::max, std::clamp!)
 * [ ] ONLY ase-containers for containers (NO std::vector, std::map, std::unordered_map!)
 * [ ] ONLY ase-types for Result/Option (NO std::optional, std::expected!)
 * [ ] std:: FORBIDDEN except: <cstdint>, <cmath> basics, <cassert>
 * [ ] CAUSAL CHAIN documented (Input → Processing → Output)
 * [ ] HUB Pattern documented (READS/WRITES)
 * [ ] hub::get() for reads
 * [ ] hub::set() for writes
 * [ ] Method order: on_start → tick → on_stop
 * [ ] ALL THREE METHODS implemented
 * [ ] on_start/on_stop: log::debug with system name
 * [ ] log::warn() if value EXISTS but invalid (e.g., health < 0, temp > 1000)
 * [ ] log::error() for EVERY NOT_FOUND check (see ase-log/log.hpp ERR::CAT::*)
 * [ ] Unused params: (void)dt; or commented parameter name
 * [ ] NO switch/case statements? (use Tag-filtered Views!)
 * [ ] NO if-else chains for type dispatch? (use separate Systems per type!)
 * [ ] NO instanceof/dynamic_cast checks? (use Tags for entity classification!)
 * [ ] NO factory patterns with type enums? (use Component composition!)
 * [ ] NO inheritance hierarchies? (use Component composition!)
 * [ ] NO virtual dispatch for game logic? (only ecs::System base class allowed!)
 * [ ] NO singleton patterns? (use Manager Tags on entities!)
 * [ ] NO state machines with switch? (use Tag-based state + separate Systems!)
 * [ ] ALL behavior driven by Component DATA, not hardcoded logic?
 * [ ] NO hardcoded entity types? (types defined by Component composition!)
 * [ ] NO hardcoded processing order? (order via Schedule + run_after!)
 * [ ] NO hardcoded value ranges? (ranges in types.hpp constants!)
 * [ ] NO hardcoded special cases? (special cases = Tags + dedicated Systems!)
 * [ ] Formulas use Component fields, not magic numbers?
 * [ ] New behavior = new Component + new System, NOT if-else in existing code?
 * [ ] NO `find_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO `check_*()`/`has_*()`/`is_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO `get_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO struct in namespace {}? (use Component)
 * [ ] NO collect-then-process? (use single-pass)
 * [ ] NO View/Query in Helper? (only pure math)
 * [ ] NO `bool has_*` for type categories in Components? (use Tags!)
 * [ ] NO `bool is_*` for type categories in Components? (use Tags!)
 * [ ] NO `uint8_t *_type` field with if-chain dispatch? (use Tag-filtered Views!)
 * [ ] Type determined by Tag composition, not boolean field?
 * [ ] N-item support via Entity-per-Item + Tags, not type booleans?
 * [ ] Tag-filtered Views per type, not if-chain in single loop?
 * [ ] NO Entity-per-Character pattern when loading strings?
 * [ ] String loading uses char[N] fixed arrays or Pointer Pattern?
 * [ ] String hashing via entt::hashed_string for lookup keys?
 * [ ] String data stored as single attribute, not per-character entities?
 * [ ] NO std::shared_ptr in Components? (use Flyweight Pattern!)
 * [ ] NO void* in Components? (use Flyweight Pattern!)
 * [ ] NO static std::unordered_map for resource storage? (use ResourceManager via ctx!)
 * [ ] External resources (shared_ptr, handles) accessed via registry.ctx().get<ResourceManager&>()?
 * [ ] ResourceManager registered in on_start() via registry.ctx().emplace<ResourceManager&>()?
 * [ ] Components store ONLY uint32_t IDs referencing external resources?
 */

// INCLUDES - ONLY THESE ARE ALLOWED!
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/storage/systems/acl/storage_acss_rslv_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_req_cred_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_relm_idn_comp.hpp>
#include <ase/storage/components/state/storage_rule_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_acss_rslv_comp.hpp>
#include <ase/storage/components/tag/storage_acss_grnt_tag.hpp>
#include <ase/storage/components/tag/storage_acss_deny_tag.hpp>
#include <ase/storage/components/tag/storage_acss_rslv_tag.hpp>
#include <ase/storage/components/tag/storage_acss_ownr_tag.hpp>
#include <ase/storage/components/tag/storage_relm_cncm_tag.hpp>
#include <ase/storage/components/tag/storage_relm_glob_tag.hpp>
// Die Pruefspur-Zeile wird seit dem 2026-08-31 in DIESEM Rumpf gestellt, nicht mehr von einem
// Helfer mit `ecs::Registry&` im Argument. Sie kam bis dahin transitiv ueber storage_acss_ladder;
// wer einen Typ selbst benutzt, fuehrt seine Kante selbst.
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/storage_acss_ladder.hpp>
#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO View/Query operations in helpers! Only pure math!
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAcssRslvSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssRslv] Started");
}

void StorageAcssRslvSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;
    }
    auto& mgr = **mgr_ptr;
    uint64_t now = mgr.get_wall_time_seconds();

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER: der Erzeuger (StorageAcssIdxSystem) hat den ctx-Halter noch nicht angelegt.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageAcssRslvSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // Tag-filtered classification views, built ONCE. A request names its realm by entity
    // id, so the classification is a membership test on the filtered view — O(1), and the
    // tag stays where the ECS rules want it: in the View filter, never in an all_of<Tag>
    // runtime check. Both were per-request scans over every realm before (WS-K.2c).
    auto pub_view = registry.view<StorageStaRelmComponent, StorageRelmGlobTag>();
    auto cnc_view = registry.view<StorageStaRelmComponent, StorageRelmCncmTag>();

    // Both halves of the request in ONE view: what is asked (req) and what the
    // caller brings (cred). A request without credentials never reaches the ladder -
    // the view drops it, which is the same denial the empty user_id check gives.
    // Already-resolved requests are excluded: the resolution runs exactly once.
    auto req_view = registry.view<StorageReqAcssComponent, StorageReqCredComponent>(
        entt::exclude<StorageAcssGrntTag, StorageAcssDenyTag, StorageAcssRslvTag>);
    for (auto entity : req_view) {
        auto& req  = req_view.get<StorageReqAcssComponent>(entity);
        auto& cred = req_view.get<StorageReqCredComponent>(entity);

        // ── Step 1: KEYCARD VALID ─ authenticated identity present
        // user_id is set by the HTTP route from the keycard JWT (validated by
        // StorageKycdVldSystem); an empty user_id means no valid keycard reached here.
        if (cred.user_id[0] == '\0') {
            registry.emplace<StorageAcssDenyTag>(entity);
            // DIE PRUEFSPUR-ZEILE IST EINE ENTITY, und sie wird hier gestellt statt in einem
            // Helfer: eine freie Funktion mit `ecs::Registry&` im Argument verbirgt ECS-Arbeit
            // hinter einem Namen. `user_id` bleibt LEER — genau das ist die Aussage dieser
            // Ablehnung: es kam keine gueltige Ausweiskarte an, es gibt also keine Identitaet,
            // die man eintragen koennte. Eine erfundene waere schlimmer als eine leere.
            auto aud_ent = registry.create();
            auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
            aud.relm_ref = req.relm_ref;
            aud.proj_ref = req.proj_ref;
            ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, "");
            ase::utils::str_copy(aud.path, MAX_PATH_LEN, req.path);
            aud.timestamp = now;
            auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
            outc.action = req.action;
            outc.result = AUD_DENIED;
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "not_authenticated");
            registry.emplace<StorageAudtPendTag>(aud_ent);
            continue;
        }

        // ── Step 2: REALM MEMBERSHIP + CONCEALMENT ─ resolve the target realm core data
        char     target_id[MAX_REALM_ID] = {};
        bool     realm_found = false;
        bool     public_realm = false;
        bool     owner_preset = false;
        // req.relm_ref IS the realm's entity id, so the realm is reached directly. The
        // former version scanned EVERY realm and compared each entity id against the one
        // it already held — a linear search for a lookup the registry answers in O(1),
        // and it ran once per request (WS-K.2c).
        const auto relm_ent = static_cast<ecs::Entity>(req.relm_ref);
        // The requester's identity, hashed ONCE per request. Every identity test below
        // is a 32-bit equality against this value: identity is a lookup, and a lookup
        // compares hashes, never characters (WRFL_ASE_STRING_HANDLING Section 3).
        const uint32_t user_hash = entt::hashed_string(cred.user_id).value();
        if (auto* rc = registry.try_get<StorageStaRelmComponent>(relm_ent)) {
            realm_found = true;
            ase::utils::str_copy(target_id, MAX_REALM_ID, rc->id);
            // Identity lives beside the record. A realm without it cannot be classified
            // at all, so its absence is reported rather than silently treated as private.
            auto* rc_idn = registry.try_get<StorageRelmIdnComponent>(relm_ent);
            if (rc_idn == nullptr) {
                log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssRslvSystem",
                           req.relm_ref, "StorageRelmIdnComponent");
            } else {
                // Public 'ase' realm by id is the in-ladder PUBLIC protection source
                public_realm = rc_idn->id_hash == ACSS_REALM_PUBLIC_HASH;
                // Direct owner of this realm → owner keycard preset (ARCH :819)
                owner_preset = rc_idn->owner_hash == user_hash;
            }
        }
        // Public realm classification via the tag on THAT realm (a realm carrying the
        // StorageRelmGlobTag is public regardless of its id naming).
        if (realm_found && !public_realm) {
            public_realm = pub_view.contains(relm_ent);
        }
        // Parent-realm ownership: owner of "org/adg" governs "org/adg/projects/x".
        // The condition the scan tested - rc.id is a prefix of target_id AND the next
        // character is '/' - is exactly "rc.id is a path ANCESTOR of target_id", and the
        // ancestors of a path can be read off the path itself. Enumerating them costs
        // path depth and hits the index once per step, where the scan cost one pass over
        // every realm per request (WS-K.2c).
        if (realm_found && !owner_preset) {
            const uint32_t target_len = ase::utils::str_len(target_id, MAX_REALM_ID);
            char ancestor[MAX_REALM_ID] = {};
            for (uint32_t cut = 1u; cut < target_len && !owner_preset; ++cut) {
                if (target_id[cut] != '/') { continue; }
                for (uint32_t i = 0; i < cut; ++i) { ancestor[i] = target_id[i]; }
                ancestor[cut] = '\0';
                const uint32_t ancestor_hash = entt::hashed_string(ancestor).value();
                const uint32_t cand = idx.get_realm(static_cast<uint64_t>(ancestor_hash));
                if (cand == INVALID_ENTITY) { continue; }
                auto* rc_idn =
                    registry.try_get<StorageRelmIdnComponent>(static_cast<ecs::Entity>(cand));
                if (rc_idn == nullptr) { continue; }
                // Identity is the hash. The bucket key and the stored id_hash are the
                // same number, so agreeing on it IS the confirmation - there is nothing
                // a character comparison would add beyond the cost of walking the string.
                if (rc_idn->id_hash != ancestor_hash) { continue; }
                if (rc_idn->owner_hash == user_hash) {
                    owner_preset = true;
                }
            }
        }
        // Concealment via Tag-filtered View: a concealed realm is invisible to non-owners.
        // Public realms are never concealed; the owner-preset always sees its own realm.
        bool concealed = false;
        if (realm_found && !public_realm && !owner_preset) {
            concealed = cnc_view.contains(relm_ent);
        }
        if (!realm_found || concealed) {
            // Concealment leaks nothing: deny as realm_not_found, never access_denied.
            registry.emplace<StorageAcssDenyTag>(entity);
            // DER GRUND STEHT AUCH IN DER PRUEFSPUR ALS `realm_not_found`, und das ist Absicht,
            // keine Ungenauigkeit: eine verborgene Revier-Zeile darf sich auch hier nicht durch
            // einen abweichenden Grund verraten. Wer die Spur liest, sieht dasselbe wie der
            // Fragende — sonst waere die Verbergung an der einen Stelle dicht und an der anderen
            // offen.
            auto aud_ent = registry.create();
            auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
            aud.relm_ref = req.relm_ref;
            aud.proj_ref = req.proj_ref;
            ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, cred.user_id);
            ase::utils::str_copy(aud.path, MAX_PATH_LEN, req.path);
            aud.timestamp = now;
            auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
            outc.action = req.action;
            outc.result = AUD_DENIED;
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "realm_not_found");
            registry.emplace<StorageAudtPendTag>(aud_ent);
            continue;
        }

        // ── Step 2 (cont.): match the ACL rule for this path ─ clearance/label/codeword src.
        // The public realm contributes an implicit PUBLIC protection rule (level 0, no
        // codewords) so engine defaults stay readable by every authenticated user — this is
        // the in-ladder replacement of the old pre-ladder "ase_shared" grant.
        uint8_t  required_protection = PROTECTION_PUBLIC;
        uint32_t matched_rule = INVALID_ENTITY;
        uint32_t rule_label_hash = 0;
        // The request's own prefixes and extensions, hashed ONCE. Three separate gates
        // in the ladder ask "does this path lie under X" - the ACL rules here, the lattice
        // share and the need-to-know scope there - and all read these same numbers.
        uint32_t pfx_hash[MAX_PATH_LEN + 1] = {};
        uint32_t ext_hash[ACSS_MAX_PATH_PARTS] = {};
        uint32_t ext_len[ACSS_MAX_PATH_PARTS] = {};
        const uint32_t path_len = ase::utils::str_len(req.path, MAX_PATH_LEN);
        path_prefix_hashes(req.path, path_len, pfx_hash);
        const uint32_t ext_n = path_extension_hashes(req.path, path_len, ext_hash,
                                                     ext_len, ACSS_MAX_PATH_PARTS);
        {
            // Pattern semantics (types.hpp ACSS_MATCH_SUFFIX_BONUS): an extension rule
            // ("*.sig" governs companion artifacts that sit BESIDE binaries, where no
            // location can reach them) carries StorageAcssRuleSufxTag; every other rule
            // is a location rule. The MOST SPECIFIC rule wins (extension over location,
            // longer literal over shorter) — never first-iteration order, which is
            // storage-order dependent and would let a broad location rule swallow the
            // "*.sig" codeword requirement.
            //
            // The request's own locations and extensions are hashed ONCE, above; each
            // rule is then one 32-bit equality against them. The two kinds are matched by
            // two different computations, so they are two loops over two Tag-separated
            // buckets - never one loop that reads a discriminator and branches.
            uint32_t best_score = 0;
            const uint32_t loc_rule_count = idx.get_location_rule_count(req.relm_ref);
            for (uint32_t rule_index = 0; rule_index < loc_rule_count; ++rule_index) {
                const uint32_t acl_id = idx.get_location_rule(req.relm_ref, rule_index);
                if (acl_id == INVALID_ENTITY) { continue; }
                const auto acl_ent = static_cast<ecs::Entity>(acl_id);
                auto* rule_ptr = registry.try_get<StorageAcssRuleComponent>(acl_ent);
                if (rule_ptr == nullptr) { continue; }
                auto& rule = *rule_ptr;
                // rule.relm_ref == req.relm_ref holds by construction of the bucket.
                if (rule.proj_ref != req.proj_ref && rule.proj_ref != 0) { continue; }
                auto* rule_idn = registry.try_get<StorageRuleIdnComponent>(acl_ent);
                if (rule_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssRslvSystem",
                               acl_id, "StorageRuleIdnComponent");
                    continue;
                }
                // The path must be at least as long as the pattern and must begin with
                // it - the same condition the character comparison enforced, now one
                // 32-bit equality against the prefix hash of exactly that length.
                if (rule_idn->match_len < 1u) { continue; }
                if (rule_idn->match_len > path_len) { continue; }
                if (pfx_hash[rule_idn->match_len] != rule_idn->match_hash) { continue; }
                const uint32_t score = 1u + rule_idn->match_len;
                if (score <= best_score) { continue; }
                best_score = score;
                required_protection = rule.protection_level;
                matched_rule = acl_id;
                rule_label_hash = rule_idn->label_hash;
            }

            const uint32_t ext_rule_count = idx.get_extension_rule_count(req.relm_ref);
            for (uint32_t rule_index = 0; rule_index < ext_rule_count; ++rule_index) {
                const uint32_t acl_id = idx.get_extension_rule(req.relm_ref, rule_index);
                if (acl_id == INVALID_ENTITY) { continue; }
                const auto acl_ent = static_cast<ecs::Entity>(acl_id);
                auto* rule_ptr = registry.try_get<StorageAcssRuleComponent>(acl_ent);
                if (rule_ptr == nullptr) { continue; }
                auto& rule = *rule_ptr;
                if (rule.proj_ref != req.proj_ref && rule.proj_ref != 0) { continue; }
                auto* rule_idn = registry.try_get<StorageRuleIdnComponent>(acl_ent);
                if (rule_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssRslvSystem",
                               acl_id, "StorageRuleIdnComponent");
                    continue;
                }
                if (rule_idn->match_len < 1u) { continue; }
                bool match = false;
                for (uint32_t i = 0; i < ext_n; ++i) {
                    if (ext_hash[i] == rule_idn->match_hash &&
                        ext_len[i] == rule_idn->match_len) { match = true; break; }
                }
                if (!match) { continue; }
                // Score parity with the character version: it scored on the FULL pattern
                // length, wildcard included, which is one more than the literal.
                const uint32_t score = ACSS_MATCH_SUFFIX_BONUS + rule_idn->match_len + 1u;
                if (score <= best_score) { continue; }
                best_score = score;
                required_protection = rule.protection_level;
                matched_rule = acl_id;
                rule_label_hash = rule_idn->label_hash;
            }
        }

        /**
         * DIE AUFLOESUNG WIRD ZUM ZUSTAND. Drei Zahlen genuegen der Torleiter; alles andere
         * leitet sie aus Dingen ab, die ohnehin auf der Anfrage stehen. Die Eigentuemer-Vorgabe
         * ist eine MARKE, kein Feld: sie ist eine Klassifikation, und Klassifikationen gehoeren
         * nach WRFL_ASE_TAGGED_VIEWS in den View-Filter.
         */
        auto& rslv = registry.emplace<StorageStaAcssRslvComponent>(entity);
        rslv.matched_rule = matched_rule;
        rslv.rule_label_hash = rule_label_hash;
        rslv.required_protection = required_protection;
        if (owner_preset) {
            registry.emplace<StorageAcssOwnrTag>(entity);
        }
        registry.emplace<StorageAcssRslvTag>(entity);
    }
}

void StorageAcssRslvSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssRslv] Stopped");
}

}  // namespace ase::storage
