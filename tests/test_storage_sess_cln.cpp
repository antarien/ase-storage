/**
 * ASE Storage — Session Hub-Family Retirement Tests (doctest, Backlog #111)
 *
 * @file        test_storage_sess_cln.cpp
 * @brief       Unit tests for StorageKycdSessClnSystem
 * @module      ase-storage
 * @layer       3 (Module)
 *
 * Drives the REAL StorageKycdSessClnSystem against the exact key set
 * StorageKycdLnkSystem publishes (storage_kycd_lnk_sys.cpp:243-256 plus the
 * SES_USER_ID_HI/_LO halves). No main() here — test_storage_wflw.cpp owns
 * DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN for this executable.
 *
 * The defect under test (measured 2026-07-28): NetworkHubSyncSystem PASS 2
 * retires only NET_CLAI_ID / SES_IS_AUTHENTICATED / SES_EXP_AT, so
 * SES_CLEARANCE, SES_REALM_ID and the user-hash halves outlived every session.
 * HubSessLifeClnSystem releases HubSessLifeTag only once the owner's hub-value
 * count reaches ZERO — so a single surviving key pins the tag forever. That is
 * why the decisive assertion in these cases is hub::count(owner) == 0 and not
 * merely "the keys I named are gone".
 */

#include <doctest/doctest.h>

#include <ase/storage/systems/keycard/storage_kycd_sess_cln_sys.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
#include <ase/hub/api.hpp>
#include <ase/ecs/app.hpp>
#include <entt/core/hashed_string.hpp>
#include <cstdint>

using namespace ase::ecs;
using namespace ase::storage;
using namespace entt::literals;

namespace {

namespace hub = ase::hub;

/** Publish exactly what StorageKycdLnkSystem publishes for a linked session.
 *  Kept in one place so a key added to the producer without adding it to the
 *  retirer fails the count assertion below instead of leaking silently. */
void publish_session_family(Registry& reg, uint32_t owner, uint32_t user_hash) {
    hub::set(reg, owner, "SES_IS_AUTHENTICATED"_hs, 1.0f);
    hub::set(reg, owner, "SES_CLEARANCE"_hs, 5.0f);
    hub::set(reg, owner, "SES_EXP_AT"_hs, 1785000000.0f);
    hub::set(reg, owner, "SES_REALM_ID"_hs, 123456.0f);
    hub::set(reg, owner, "SES_USER_ID_HI"_hs, static_cast<float>(user_hash >> 16));
    hub::set(reg, owner, "SES_USER_ID_LO"_hs, static_cast<float>(user_hash & 0xFFFFu));
}

/** A session entity as the live server holds it: keycard linked, identity
 *  mirror present, session-lifecycle tag present. */
entt::entity make_live_session(Registry& reg, uint32_t user_hash) {
    auto e = reg.create();
    reg.emplace<StorageKycdVldTag>(e);
    reg.emplace<hub::HubSessLifeTag>(e);
    reg.emplace<hub::HubStaClaiIdntComponent>(e);
    publish_session_family(reg, static_cast<uint32_t>(e), user_hash);
    return e;
}

/** The same entity after NetworkHubSyncSystem PASS 2 ran: the identity mirror
 *  is gone, HubSessLifeTag deliberately stays, and PASS 2 has already retired
 *  the two SES_* keys it wrote itself. */
void kill_session(Registry& reg, entt::entity e) {
    reg.remove<hub::HubStaClaiIdntComponent>(e);
    uint32_t owner = static_cast<uint32_t>(e);
    hub::remove(reg, owner, "SES_IS_AUTHENTICATED"_hs);
    hub::remove(reg, owner, "SES_EXP_AT"_hs);
}

}  // anonymous namespace

TEST_CASE("StorageKycdSessClnSystem retires the whole SES_* family of a dead session") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    auto e = make_live_session(reg, 0xDEADBEEFu);
    uint32_t owner = static_cast<uint32_t>(e);
    REQUIRE(hub::count(reg, owner) == 6);

    kill_session(reg, e);
    // What NetworkHubSyncSystem leaves behind today: four keys and a pinned tag.
    REQUIRE(hub::count(reg, owner) == 4);

    sys.tick(reg, 1.0f);

    CHECK(hub::exists(reg, owner, "SES_CLEARANCE"_hs) == false);
    CHECK(hub::exists(reg, owner, "SES_REALM_ID"_hs) == false);
    CHECK(hub::exists(reg, owner, "SES_USER_ID_HI"_hs) == false);
    CHECK(hub::exists(reg, owner, "SES_USER_ID_LO"_hs) == false);

    // The condition HubSessLifeClnSystem waits for before it may release
    // HubSessLifeTag. Anything above zero means the tag is pinned for the life
    // of the process — that is the leak, not the individual key.
    CHECK(hub::count(reg, owner) == 0);

    // A disconnected session holds no valid keycard.
    CHECK(reg.all_of<StorageKycdVldTag>(e) == false);
}

TEST_CASE("StorageKycdSessClnSystem leaves a LIVE session untouched") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    auto live = make_live_session(reg, 0x00ABCDEFu);
    uint32_t owner = static_cast<uint32_t>(live);

    sys.tick(reg, 1.0f);

    // The identity mirror is the exclude filter; while it is present the
    // session is alive and nothing may be retired.
    CHECK(hub::count(reg, owner) == 6);
    CHECK(hub::get(reg, owner, "SES_CLEARANCE"_hs) == 5.0f);
    CHECK(reg.all_of<StorageKycdVldTag>(live) == true);
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
    CHECK(hub::count(reg, live_owner) == 6);
    CHECK(reg.all_of<StorageKycdVldTag>(live) == true);
}

TEST_CASE("StorageKycdSessClnSystem ignores a dead session that never linked a keycard") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    // ws-ticket session: NetworkHubSyncSystem published SES_*, ase-storage
    // never did, so StorageKycdVldTag was never set and this module owns
    // nothing here.
    auto e = reg.create();
    reg.emplace<hub::HubSessLifeTag>(e);
    uint32_t owner = static_cast<uint32_t>(e);
    hub::set(reg, owner, "NET_CLAI_ID"_hs, 7.0f);

    sys.tick(reg, 1.0f);

    CHECK(hub::exists(reg, owner, "NET_CLAI_ID"_hs) == true);
}

TEST_CASE("StorageKycdSessClnSystem is idempotent and survives an empty registry") {
    Registry reg;
    StorageKycdSessClnSystem sys;

    // Empty view — must not crash and must not invent work.
    sys.tick(reg, 1.0f);
    CHECK(hub::total_count(reg) == 0);

    auto e = make_live_session(reg, 0x0000FFFFu);
    uint32_t owner = static_cast<uint32_t>(e);
    kill_session(reg, e);

    sys.tick(reg, 1.0f);
    sys.tick(reg, 1.0f);  // second pass: the view no longer matches

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

    uint32_t hi = static_cast<uint32_t>(hub::get(reg, owner, "SES_USER_ID_HI"_hs));
    uint32_t lo = static_cast<uint32_t>(hub::get(reg, owner, "SES_USER_ID_LO"_hs));

    // Each half is below 2^16 and therefore exact in a float32 hub value.
    CHECK(hi < (1u << 16));
    CHECK(lo < (1u << 16));
    // useSessionStore.ts performs exactly this reassembly browser-side.
    CHECK(((hi << 16) | lo) == user_hash);

    // The single-value form the browser read until 2026-07-28 loses the low
    // bits outright — this is what a bare SES_USER_ID would have delivered had
    // anyone ever published it.
    CHECK(static_cast<uint32_t>(static_cast<float>(user_hash)) != user_hash);
}
