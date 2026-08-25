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
#include <ase/storage/storage_acss_index_resource_manager.hpp>

// Module Definition
#include <ase/storage/storage_module.hpp>

// Tags
#include <ase/storage/components/tag/storage_mgr_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_vld_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_exp_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_rjct_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_rev_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_pend_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_pst_pend_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_pst_done_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_rev_pst_tag.hpp>
#include <ase/storage/components/tag/storage_acss_grnt_tag.hpp>
#include <ase/storage/components/tag/storage_acss_deny_tag.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/components/tag/storage_vote_pend_tag.hpp>

// Review-Marken (bis 2026-08-20 unter dem ungedeckten Stamm `cur`)
#include <ase/storage/components/tag/storage_rvw_pend_tag.hpp>
#include <ase/storage/components/tag/storage_rvw_acpt_tag.hpp>
#include <ase/storage/components/tag/storage_rvw_rjct_tag.hpp>
#include <ase/storage/components/tag/storage_rvw_rvse_tag.hpp>
#include <ase/storage/components/tag/storage_relm_usr_tag.hpp>
#include <ase/storage/components/tag/storage_relm_org_tag.hpp>
#include <ase/storage/components/tag/storage_relm_glob_tag.hpp>
#include <ase/storage/components/tag/storage_relm_actv_tag.hpp>
#include <ase/storage/components/tag/storage_relm_susp_tag.hpp>
#include <ase/storage/components/tag/storage_relm_arcv_tag.hpp>
#include <ase/storage/components/tag/storage_relm_cncm_tag.hpp>

// Request Components
#include <ase/storage/components/request/storage_req_kycd_comp.hpp>

// State Components
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_relm_quot_comp.hpp>
#include <ase/storage/components/state/storage_relm_idn_comp.hpp>
#include <ase/storage/components/state/storage_rule_idn_comp.hpp>
#include <ase/storage/components/state/storage_kycd_idn_comp.hpp>
#include <ase/storage/components/state/storage_lnk_idn_comp.hpp>
#include <ase/storage/components/state/storage_task_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_sess_comp.hpp>
#include <ase/storage/components/state/storage_sta_tkn_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_kycd_grnt_comp.hpp>
#include <ase/storage/components/state/storage_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_acss_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_lat_lnk_comp.hpp>
#include <ase/storage/components/state/storage_lnk_cnst_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/state/storage_buf_kycd_comp.hpp>
#include <ase/storage/components/state/storage_buf_kycd_acc_comp.hpp>
#include <ase/storage/components/state/storage_buf_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_req_cred_comp.hpp>
#include <ase/storage/components/state/storage_sta_vote_comp.hpp>
#include <ase/storage/components/state/storage_vote_cnt_comp.hpp>
#include <ase/storage/components/state/storage_bllt_vote_comp.hpp>
#include <ase/storage/components/state/storage_sta_task_comp.hpp>
#include <ase/storage/components/state/storage_task_desc_comp.hpp>
#include <ase/storage/components/state/storage_req_cur_prm_comp.hpp>
#include <ase/storage/components/state/storage_cur_asmt_comp.hpp>
