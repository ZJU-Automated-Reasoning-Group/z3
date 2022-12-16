/*
 * pp_qfbv_light_tactic.h
 *
 * @Author: rainoftime
 */

#pragma once

#include "util/params.h"
class ast_manager;
class tactic;

tactic * mk_pp_sat_tactic(ast_manager & m, params_ref const & p = params_ref());

tactic * mk_pp_qfbv_light_tactic(ast_manager & m, params_ref const & p = params_ref());

/*
  ADD_TACTIC("pp_qfbv_light",  "A lightweight strategy for solving QF_BV problems.", "mk_pp_qfbv_light_tactic(m, p)")

  ADD_TACTIC("pp_sat",  "A strategy for solving SAT problems.", "mk_pp_sat_tactic(m, p)")
*/

tactic * mk_pp_qfbv_light_preamble(ast_manager& m, params_ref const& p);

tactic * mk_pp_sat_tactic(ast_manager & m, params_ref const & p, tactic* sat);

tactic * mk_pp_qfbv_light_tactic(ast_manager & m, params_ref const & p, tactic* sat);
