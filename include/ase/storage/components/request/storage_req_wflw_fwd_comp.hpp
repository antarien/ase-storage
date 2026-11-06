#pragma once

/**
 * ASE ECS COMPONENT (REQUEST)
 *
 * @file        storage_req_wflw_fwd_comp.hpp
 * @brief       StorageReqWflwFwdComponent - one Replica-forwarded operator command awaiting its reply
 * @description One entity per forward frame the Replica relayed on LANE_WFLW (Entity-per-Item).
 *              StorageEdgeWflwFwdRcvSystem fills it and marks what the command is: a filed
 *              promote (StorageWflwPromTag), a status read (StorageWflwStatTag) or a rejected
 *              frame (StorageWflwRjctTag). StorageEdgeWflwRplySystem answers each one on the
 *              reply frame and destroys the entity in the same tick.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    communication/request
 * @parity      server_only
 * @usecase     usecase:100317 - relay operator command
 *
 * PARITY DECISION
 *   The entity names one ase-cli connection of the Replica and lives between two server-only
 *   transport systems of the dist tier inside one tick. The browser has neither the connection
 *   nor the lane, so nothing of it is computed on the client.
 * @created     2026-11-05
 * @modified    2026-11-05
 * @version     1.0.0
 *
 * ECS COMPONENT COMPLIANCE
 *
 * [ ] DATA fields ONLY - No methods
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization (= 0, = 0.0f, = false, = {})
 * [ ] No magic numbers in defaults (use types.hpp constants)
 * [ ] Entity references initialized to = 0 (systems set values)
 * [ ] Single responsibility (one data category)
 * [ ] No God-Component (unrelated fields)
 * [ ] Large data in registry.ctx()? (component has only lookup ID!)
 * [ ] Tag structs end with Tag suffix - N/A (not a tag)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name derived from filename (snake_case to PascalCase)
 * [ ] 1 File = 1 Component
 * [ ] File in correct category subfolder
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Strings < 64 bytes use char[N] fixed arrays
 * [ ] Strings 64-256 bytes use appropriately sized char[N]
 * [ ] Strings > 256 bytes use registry.ctx() with lookup ID?
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 */

#include <cstdint>

// MAX_PATH_LEN, MAX_LABEL_LEN, MAX_REASON_LEN - the module's slot widths, NUL included
#include <ase/storage/types.hpp>

namespace ase::storage {

/**
 * @brief StorageReqWflwFwdComponent - one forwarded operator command awaiting its reply
 *
 * Carries what the reply needs: the Replica connection to answer, the asset path the
 * command names, the requested target label of a promote and, for a rejected frame, the
 * reason the reply states. The kind of command is the tag on the same entity.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageReqWflwFwdComponent {
    uint32_t cli_conn = 0;                 // Replica-side ase-cli connection the reply is relayed to
    char path[MAX_PATH_LEN] = {};          // Realm-relative asset path the command names
    char target_label[MAX_LABEL_LEN] = {}; // Requested workflow label of a promote
    char reason[MAX_REASON_LEN] = {};      // Why a rejected frame was refused (reply error text)
};

}  // namespace ase::storage
