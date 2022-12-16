/*
 * pp_qfbv_tactic.h
 *
 *      Author: rainoftime
 */

#pragma once

#include "util/params.h"
class ast_manager;
class tactic;

tactic * mk_pp_qfbv_tactic(ast_manager & m, params_ref const & p = params_ref());

/*
  ADD_TACTIC("pp_qfbv",  "A strategy for solving QF_BV problems.", "mk_pp_qfbv_tactic(m, p)")
*/

tactic * mk_pp_qfbv_preamble(ast_manager& m, params_ref const& p);

tactic * mk_pp_qfbv_light_tactic(ast_manager & m, params_ref const & p, tactic* sat);
