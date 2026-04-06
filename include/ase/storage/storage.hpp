#pragma once

/**
 * ASE MODULE UMBRELLA HEADER
 *
 * @file        storage.hpp
 * @brief       Single include for all ase-storage types, components, and tags
 * @module      ase-storage
 * @layer       3 (Modules)
 */

// Types (SSOT)
#include <ase/storage/types.hpp>

// Resource Manager
#include <ase/storage/storage_resource_manager.hpp>

// Module Definition
#include <ase/storage/storage_module.hpp>

// Tags
#include <ase/storage/components/tag/storage_tag_mgr.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_exp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_rjct.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_rev.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_pend.hpp>
#include <ase/storage/components/tag/storage_tag_acss_grant.hpp>
#include <ase/storage/components/tag/storage_tag_acss_deny.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/components/tag/storage_tag_vot_pend.hpp>
#include <ase/storage/components/tag/storage_tag_relm_personal.hpp>
#include <ase/storage/components/tag/storage_tag_relm_org.hpp>
#include <ase/storage/components/tag/storage_tag_relm_public.hpp>
#include <ase/storage/components/tag/storage_tag_relm_active.hpp>
#include <ase/storage/components/tag/storage_tag_relm_suspended.hpp>
#include <ase/storage/components/tag/storage_tag_relm_archived.hpp>
#include <ase/storage/components/tag/storage_tag_relm_conceal.hpp>

// State Components
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_tkn_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_acss_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_lat_lnk_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_sta_vote_comp.hpp>
#include <ase/storage/components/state/storage_vote_cnt_comp.hpp>
#include <ase/storage/components/state/storage_bllt_vote_comp.hpp>
#include <ase/storage/components/state/storage_sta_task_comp.hpp>
