#pragma once

/**
 * ASE STORAGE KEYCARD-PAYLOAD SCAN (module-internal)
 *
 * @file        storage_kycd_payload.hpp
 * @brief       Bounded field extraction out of a keycard-authz JSON payload
 * @description Three pure byte scans over a bounded buffer: find a token, read a number
 *              behind '"key":', copy a string behind '"key":"'. No std::string, no strstr,
 *              no allocation, no registry - every function sees only pointer, length and
 *              target buffer, and every scan is bounded by the length of the document it
 *              is handed. The document width itself is the owner's hub row, read by the
 *              caller (NET_CONST_RSN_MEM_READ_PAYLOAD_MAX in StorageEdgeKycdResDrnSystem).
 *
 *              WHY THEY STAND HERE AND NOT IN THE ANONYMOUS NAMESPACE OF THE DRAINER:
 *              pulling fields out of a payload is a different concern than draining a
 *              transport lane and publishing a session. As a unit with a head of its own
 *              the cut between them is named and checkable. This is the build form that
 *              storage_acss_ladder.hpp shows in the same module: declared namespace, own
 *              header, no encapsulation of ECS logic.
 *
 *              THE FUNCTIONS ARE PURE OVER PRIMITIVE TYPES. That is the explicitly allowed
 *              helper form - what is forbidden is a helper that takes the registry and so
 *              hides a view or a hub read behind a function call. These three take none.
 *
 *              A KNOWN FINDING STANDS OUTSIDE THIS MODULE: ase-replication
 *              src/replica/replica_sess_clrn_sys.cpp carries parse_num_field and
 *              parse_str_field with the same meaning. A module never reaches into another;
 *              the shared home is ase-utils (L0), where str_len already lies and which both
 *              include. That is a foundation decision over two modules.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @usecase     usecase:100049 - publish a hub value
 * @created     2026-08-31
 * @modified    2026-11-05
 * @version     1.1.0
 */

#include <cstdint>

#include <ase/utils/strops.hpp>

namespace ase::storage {

// Find the first occurrence of needle in [doc, doc+len). Returns the index of the
// match start, or -1 when absent. Pure byte scan (no std::string, no strstr). A needle
// longer than the document cannot match, so its length is counted at most to len + 1.
inline int32_t find_token(const char* doc, uint32_t len, const char* needle) {
    uint32_t nlen = ase::utils::str_len(needle, len + 1u);
    if (nlen == 0u || nlen > len) return -1;
    for (uint32_t i = 0; i + nlen <= len; ++i) {
        uint32_t j = 0;
        while (j < nlen && doc[i + j] == needle[j]) ++j;
        if (j == nlen) return static_cast<int32_t>(i);
    }
    return -1;
}

// Parse the unsigned integer value of a '"key":<digits>' field into out_val.
// Returns true when the key + a digit run is found. Bounded scan; tolerates
// whitespace between the colon and the first digit. The key was found inside the
// document, so its length is bounded by the document length.
inline bool parse_num_field(const char* doc, uint32_t len, const char* quoted_key,
                            uint32_t& out_val) {
    int32_t at = find_token(doc, len, quoted_key);
    if (at < 0) return false;
    uint32_t i = static_cast<uint32_t>(at) + ase::utils::str_len(quoted_key, len);
    while (i < len && (doc[i] == ' ' || doc[i] == ':')) ++i;
    bool any = false;
    uint32_t v = 0;
    while (i < len && doc[i] >= '0' && doc[i] <= '9') {
        v = v * 10u + static_cast<uint32_t>(doc[i] - '0');
        any = true;
        ++i;
    }
    if (!any) return false;
    out_val = v;
    return true;
}

// Copy the string value of a '"key":"<value>"' field into out (bounded). Returns
// true when the key + a quoted value is found. Bounded scan.
inline bool parse_str_field(const char* doc, uint32_t len, const char* quoted_key,
                            char* out, uint32_t out_size) {
    if (out_size == 0u) return false;
    out[0] = '\0';
    int32_t at = find_token(doc, len, quoted_key);
    if (at < 0) return false;
    uint32_t i = static_cast<uint32_t>(at) + ase::utils::str_len(quoted_key, len);
    while (i < len && (doc[i] == ' ' || doc[i] == ':')) ++i;
    if (i >= len || doc[i] != '"') return false;
    ++i;  // opening quote
    uint32_t o = 0;
    while (i < len && doc[i] != '"' && o + 1u < out_size) {
        out[o++] = doc[i++];
    }
    out[o] = '\0';
    return o > 0u;
}

}  // namespace ase::storage
