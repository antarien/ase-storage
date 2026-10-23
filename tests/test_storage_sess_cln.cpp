/**
 * ASE Storage — Session Hub-Family Retirement Tests (doctest, Backlog #111)
 *
 * @file        test_storage_sess_cln.cpp
 * @brief       Unit tests for StorageKycdSessClnSystem
 * @module      ase-storage
 * @layer       3 (Module)
 * @usecase     usecase:100265 - check access clearance
 *
 * Drives the REAL StorageKycdSessClnSystem against the exact key set
 * StorageKycdLnkSystem publishes (storage_kycd_lnk_sys.cpp plus the
 * SES_USER_ID_HI/_LO and SES_REALM_ID_HI/_LO halves). No main() here —
 * test_storage_wflw.cpp owns DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN for this
 * executable.
 *
 * The defect class under test: NetworkHubSyncSystem PASS 2 retires only the
 * keys ase-network wrote itself, so SES_CLEARANCE, the realm halves and the
 * user-hash halves outlive the session unless this module retires them.
 * HubSessLifeClnSystem withdraws the life row HUB_SESS_LIFE only once the
 * owner's hub-value count reaches ZERO — so a single surviving key pins the
 * row forever. That is why the decisive assertion in these cases is
 * hub::count(owner) == 0 and not merely "the keys I named are gone".
 *
 * The session rows are built with the hub functions their real producers use:
 * hub::row_adopt for the life row (NetworkHubSyncSystem PASS 1) and hub::row_tag
 * for the mark HUB_SESS_LIFE_GONE (HubNetClaiSyncSystem PASS 4). ase-storage
 * reaches neither module, so the test plays both edges through the hub.
 */

#include <doctest/doctest.h>

#include <ase/storage/systems/keycard/storage_kycd_sess_cln_sys.hpp>
#include <ase/storage/components/tag/storage_kycd_vld_tag.hpp>
#include <ase/hub/api.hpp>
#include <ase/ecs/app.hpp>
#include <entt/core/hashed_string.hpp>
#include <cstdint>

using namespace ase::ecs;
using namespace ase::storage;
using namespace entt::literals;

namespace {

namespace hub = ase::hub;

// A realm hash above 2^24, where one float32 hub value would round it (D0692).
constexpr uint32_t REALM_HASH = 0x40000001u;

/** Publish exactly what StorageKycdLnkSystem publishes for a linked session.
 *  Kept in one place so a key added to the producer without adding it to the
 *  retirer fails the count assertion below instead of leaking silently. */
void publish_session_family(Registry& reg, uint32_t owner, uint32_t user_hash) {
    hub::set(reg, owner, "SES_IS_AUTHENTICATED"_hs, 1.0f);
    hub::set(reg, owner, "SES_CLEARANCE"_hs, 5.0f);
    hub::set(reg, owner, "SES_EXP_AT"_hs, 1785000000.0f);
    hub::set(reg, owner, "SES_REALM_ID_HI"_hs, static_cast<float>(REALM_HASH >> 16));
    hub::set(reg, owner, "SES_REALM_ID_LO"_hs, static_cast<float>(REALM_HASH & 0xFFFFu));
    hub::set(reg, owner, "SES_USER_ID_HI"_hs, static_cast<float>(user_hash >> 16));
    hub::set(reg, owner, "SES_USER_ID_LO"_hs, static_cast<float>(user_hash & 0xFFFFu));
}

/** A session entity as the live server holds it: keycard linked and a life
 *  row HUB_SESS_LIFE without the mark HUB_SESS_LIFE_GONE. */
entt::entity make_live_session(Registry& reg, uint32_t user_hash) {
    auto e = reg.create();
    reg.emplace<StorageKycdVldTag>(e);
    hub::row_adopt(reg, e, "HUB_SESS_LIFE"_hs);
    publish_session_family(reg, static_cast<uint32_t>(e), user_hash);
    return e;
}

/** The same entity after its connection ended: the life row carries the mark
 *  HUB_SESS_LIFE_GONE, and NetworkHubSyncSystem PASS 2 has already retired the
 *  two SES_* keys it wrote itself. */
void kill_session(Registry& reg, entt::entity e) {
    hub::row_tag(reg, e, "HUB_SESS_LIFE_GONE"_hs);
    uint32_t owner = static_cast<uint32_t>(e);
    hub::remove(reg, owner, "SES_IS_AUTHENTICATED"_hs);
    hub::remove(reg, owner, "SES_EXP_AT"_hs);
}

}  // namespace

TEST_CASE("StorageKycdSessClnSystem retires the whole SES_* family of a dead session") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    auto e = make_live_session(reg, 0xDEADBEEFu);
    uint32_t owner = static_cast<uint32_t>(e);
    REQUIRE(hub::count(reg, owner) == 7);

    kill_session(reg, e);
    // What NetworkHubSyncSystem leaves behind on its own: five keys and a pinned life row.
    REQUIRE(hub::count(reg, owner) == 5);

    sys.tick(reg, 1.0f);

    CHECK(hub::exists(reg, owner, "SES_CLEARANCE"_hs) == false);
    CHECK(hub::exists(reg, owner, "SES_REALM_ID_HI"_hs) == false);
    CHECK(hub::exists(reg, owner, "SES_REALM_ID_LO"_hs) == false);
    CHECK(hub::exists(reg, owner, "SES_USER_ID_HI"_hs) == false);
    CHECK(hub::exists(reg, owner, "SES_USER_ID_LO"_hs) == false);

    // The condition HubSessLifeClnSystem waits for before it may withdraw the
    // life row. Anything above zero means the row is pinned for the life of the
    // process — that is the leak, not the individual key.
    CHECK(hub::count(reg, owner) == 0);

    // A disconnected session holds no valid keycard.
    auto vld_view = reg.view<StorageKycdVldTag>();
    CHECK(vld_view.contains(e) == false);
}

TEST_CASE("StorageKycdSessClnSystem leaves a LIVE session untouched") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    auto live = make_live_session(reg, 0x00ABCDEFu);
    uint32_t owner = static_cast<uint32_t>(live);

    sys.tick(reg, 1.0f);

    // The mark HUB_SESS_LIFE_GONE is the filter; while it is absent the
    // session is alive and nothing may be retired.
    CHECK(hub::count(reg, owner) == 7);
    // DIE ANWESENHEIT WIRD GETRENNT ZUGESICHERT. hub::get liefert bei einem nie geschriebenen
    // Schluessel seinen Vorgabewert; ohne die zweite Zeile saehe "Freigabestufe falsch" genauso
    // aus wie "die Sitzungsfamilie wurde nie veroeffentlicht" — und genau das ist hier der
    // Unterschied zwischen einer falschen Berechtigung und einer fehlenden Sitzung.
    // Sie steht UNMITTELBAR hinter dem Lesen: das Fenster der Regel ist fuenf Zeilen, und ein
    // Begruendungsblock DAZWISCHEN wuerde die Pruefung daraus schieben.
    CHECK(hub::get(reg, owner, "SES_CLEARANCE"_hs) == 5.0f);
    CHECK(hub::is_measured(reg, owner, "SES_CLEARANCE"_hs));
    auto vld_view = reg.view<StorageKycdVldTag>();
    CHECK(vld_view.contains(live) == true);
}

TEST_CASE("StorageKycdSessClnSystem retires only the dead session, not its neighbour") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    auto dead = make_live_session(reg, 0x11112222u);
    auto live = make_live_session(reg, 0x33334444u);
    uint32_t dead_owner = static_cast<uint32_t>(dead);
    uint32_t live_owner = static_cast<uint32_t>(live);
    kill_session(reg, dead);

    sys.tick(reg, 1.0f);

    CHECK(hub::count(reg, dead_owner) == 0);
    CHECK(hub::count(reg, live_owner) == 7);
    auto vld_view = reg.view<StorageKycdVldTag>();
    CHECK(vld_view.contains(live) == true);
}

TEST_CASE("StorageKycdSessClnSystem ignores a dead session that never linked a keycard") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    // ws-ticket session: NetworkHubSyncSystem published SES_*, ase-storage
    // never did, so StorageKycdVldTag was never set and this module owns
    // nothing here.
    auto e = reg.create();
    hub::row_adopt(reg, e, "HUB_SESS_LIFE"_hs);
    hub::row_tag(reg, e, "HUB_SESS_LIFE_GONE"_hs);
    uint32_t owner = static_cast<uint32_t>(e);
    hub::set(reg, owner, "SES_EXP_AT"_hs, 1785000000.0f);

    sys.tick(reg, 1.0f);

    CHECK(hub::exists(reg, owner, "SES_EXP_AT"_hs) == true);
}

TEST_CASE("StorageKycdSessClnSystem is idempotent and survives an empty registry") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    // No life row at all — must not crash and must not invent work.
    sys.tick(reg, 1.0f);
    CHECK(hub::total_count(reg) == 0);

    auto e = make_live_session(reg, 0x0000FFFFu);
    uint32_t owner = static_cast<uint32_t>(e);
    kill_session(reg, e);

    sys.tick(reg, 1.0f);
    sys.tick(reg, 1.0f);  // second pass: the session holds no StorageKycdVldTag any more

    CHECK(hub::count(reg, owner) == 0);
}

TEST_CASE("SES_USER_ID halves survive the float32 hub value that one whole hash would not") {
    Registry reg;

    // An FNV-1a32 hash routinely exceeds 2^24, where float32 stops being able
    // to represent consecutive integers — the reason the value is split at all
    // (ARCH_ASE_HUB_API.md, same pattern as SES_KYCD_NTF_USER_ID_HI/_LO).
    uint32_t user_hash = entt::hashed_string{"6992dde284692622bc147084"}.value();
    REQUIRE(user_hash > (1u << 24));

    auto e = reg.create();
    uint32_t owner = static_cast<uint32_t>(e);
    publish_session_family(reg, owner, user_hash);

    // BEIDE HAELFTEN BEKOMMEN IHRE EIGENE ZUSICHERUNG, und hier traegt die Trennung besonders:
    // eine fehlende Haelfte liefert ihren Vorgabewert, und der Zusammenbau unten ergaebe daraus
    // eine gueltig AUSSEHENDE Kennung mit halbem Inhalt. Die Bereichspruefungen (< 2^16) koennen
    // das nicht auffangen — ein Vorgabewert liegt ebenfalls unter der Grenze.
    uint32_t hi = static_cast<uint32_t>(hub::get(reg, owner, "SES_USER_ID_HI"_hs));
    CHECK(hub::is_measured(reg, owner, "SES_USER_ID_HI"_hs));
    uint32_t lo = static_cast<uint32_t>(hub::get(reg, owner, "SES_USER_ID_LO"_hs));
    CHECK(hub::is_measured(reg, owner, "SES_USER_ID_LO"_hs));

    // Each half is below 2^16 and therefore exact in a float32 hub value.
    CHECK(hi < (1u << 16));
    CHECK(lo < (1u << 16));
    // useSessionStore.ts performs exactly this reassembly browser-side.
    CHECK(((hi << 16) | lo) == user_hash);

    // A single float32 value loses the low bits outright — this is what a bare
    // SES_USER_ID would deliver if anyone published it.
    CHECK(static_cast<uint32_t>(static_cast<float>(user_hash)) != user_hash);
}
