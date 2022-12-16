/*
 * pp_qfaufbv_tactic.h
 *
 * @Author: rainoftime
 */

#pragma once

#include "util/params.h"
class ast_manager;
class tactic;

tactic * mk_pp_qfaufbv_tactic(ast_manager & m, params_ref const & p = params_ref());

tactic * mk_pp_qfaufbv_light_tactic(ast_manager & m, params_ref const & p = params_ref());

tactic * mk_pp_qfaufbv_layered_tactic(ast_manager & m, params_ref const & p = params_ref());

/*
  ADD_TACTIC("pp_qfaufbv",  "A strategy for solving QF_AUFBV problems.", "mk_pp_qfaufbv_tactic(m, p)")

  ADD_TACTIC("pp_qfaufbv_light",  "A lightweight strategy for solving QF_AUFBV problems.", "mk_pp_qfaufbv_light_tactic(m, p)")
*/

tactic * mk_pp_qfaufbv_preamble(ast_manager& m, params_ref const& p);

tactic * mk_pp_qfaufbv_tactic(ast_manager & m, params_ref const & p, tactic* sat, tactic* smt);

tactic * mk_pp_qfaufbv_light_tactic(ast_manager & m, params_ref const & p, tactic* sat, tactic* smt);



