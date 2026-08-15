#pragma once

/**
 * ASE MODULE INCLUDE HEADER (SSOT)
 *
 * @file        storage.hpp
 * @brief       Single entry point for ase-storage
 * @description Include this header to use the module.
 *              Exports ALL components, ALL systems, and types.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @created     2026-04-06
 * @modified    2026-08-15
 * @version     1.0.0
 *
 * USAGE:
 *   #include <ase/storage/storage.hpp>
 *   app.add_module<ase::storage::StorageModule>();
 *
 * ECS MODULE/PLUGIN INCLUDE COMPLIANCE
 *
 * [ ] types.hpp included FIRST (SSOT for constants)
 * [ ] Module/Plugin definition included ({Module}Module or {Plugin}Plugin)
 * [ ] ALL data components exported
 * [ ] ALL state components exported
 * [ ] ALL tag components exported
 * [ ] ALL input components exported (L4 plugins: Hub sync targets)
 * [ ] ALL systems exported
 * [ ] Components grouped by category with section comments
 * [ ] Systems grouped by category with section comments
 * [ ] No circular dependencies
 * [ ] No duplicate includes
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
#include <ase/storage/components/tag/storage_tag_kycd_pst_pend.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_pst_done.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_rev_pst.hpp>
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

// Request Components
#include <ase/storage/components/request/storage_req_kycd_comp.hpp>

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
#include <ase/storage/components/state/storage_buf_kycd_comp.hpp>
#include <ase/storage/components/state/storage_buf_kycd_acc_comp.hpp>
#include <ase/storage/components/state/storage_buf_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_sta_vote_comp.hpp>
#include <ase/storage/components/state/storage_vote_cnt_comp.hpp>
#include <ase/storage/components/state/storage_bllt_vote_comp.hpp>
#include <ase/storage/components/state/storage_sta_task_comp.hpp>
