/*
 * pp_qfbv_light_tactic.cpp
 *
 * @Author: rainoftime
 *
 */


#include "tactic/tactical.h"
#include "tactic/core/simplify_tactic.h"
#include "tactic/core/propagate_values_tactic.h"
#include "tactic/core/solve_eqs_tactic.h"
#include "tactic/core/elim_uncnstr_tactic.h"
#include "tactic/bv/bit_blaster_tactic.h"
#include "tactic/bv/bv1_blaster_tactic.h"
#include "tactic/bv/max_bv_sharing_tactic.h"
#include "tactic/bv/bv_size_reduction_tactic.h"
#include "tactic/aig/aig_tactic.h"
#include "sat/tactic/sat_tactic.h"
#include "sat/sat_solver/inc_sat_solver.h"
#include "ackermannization/ackermannize_bv_tactic.h"
#include "tactic/smtlogics/smt_tactic.h"


static tactic * mk_pp_qfbv_light_preamble(ast_manager& m, params_ref const& p) {

    params_ref solve_eq_p;
    // conservative guassian elimination.
    solve_eq_p.set_uint("solve_eqs_max_occs", 2);

    params_ref simp2_p = p;
    simp2_p.set_bool("som", true);
    simp2_p.set_bool("pull_cheap_ite", true);
    simp2_p.set_bool("push_ite_bv", false);
    simp2_p.set_bool("local_ctx", true);
    simp2_p.set_uint("local_ctx_limit", 10000000);
    simp2_p.set_bool("flat", true); // required by som
    simp2_p.set_bool("hoist_mul", false); // required by som


    return
        and_then(
            mk_simplify_tactic(m),
            mk_propagate_values_tactic(m), // [Lin] Propagate values using equalities of the form (= t v) where v is a value, and atoms t and (not t)
            using_params(mk_solve_eqs_tactic(m), solve_eq_p), // [Lin] solving equations and performing gaussian elimination.
            mk_elim_uncnstr_tactic(m), //  [Lin] Eliminated unconstrained variables.
            //if_no_proofs(if_no_unsat_cores(mk_bv_size_reduction_tactic(m))), // [Lin] for example, -2 <= x <= 2 Then, x can be replaced by (concat m_util.mk_numeral(numeral(sign), 5) k) where k is a fresh bit-vector constant of size 3.
            using_params(mk_simplify_tactic(m), simp2_p), // [Lin] local contextual simplifications are performed. These simplifications are potentially very expensive. So, a threshold on the maximal number of nodes to be visited is used (the default value is 10million). Local context simplification contain rules such as (x != 0 or  y = x+1) ->  (x != 0 or y = 1)

            //using_params(mk_simplify_tactic(m), hoist_p), // [Lin] a_b + a_c -> (b+c)*a to minimize the number of multipliers
            mk_max_bv_sharing_tactic(m)
            // [Lin] a + (b + c), a + (b + d) -> (a+b)+c, (a+b)+d. minimize the number of adders and multipliers by applying associativity and commutativity
            //if_no_proofs(if_no_unsat_cores(mk_ackermannize_bv_tactic(m,p)))
            );
}


static tactic * main_p(tactic* t) {
    params_ref p;
    p.set_bool("elim_and", true);
    p.set_bool("push_ite_bv", true);
    p.set_bool("blast_distinct", true);
    return using_params(t, p);
}


static tactic * mk_pp_qfbv_light_tactic(ast_manager& m, params_ref const & p, tactic* sat) {
    tactic* preamble_st = mk_pp_qfbv_light_preamble(m, p);
    tactic * st = main_p(and_then(preamble_st,mk_bit_blaster_tactic(m),sat));
    st->updt_params(p);
    return st;
}

tactic * mk_pp_sat_tactic(ast_manager& m, params_ref const & p, tactic* sat) {

    params_ref main_p;
    main_p.set_bool("elim_and", true);
    main_p.set_bool("push_ite_bv", true);
    //main_p.set_bool("blast_distinct", true);

    //tactic * st = using_params(and_then(mk_simplify_tactic(m), mk_bit_blaster_tactic(m), sat), main_p);
    tactic * st = using_params(and_then(mk_simplify_tactic(m), sat), main_p);
    st->updt_params(p);
    return st;
}



// Interface
tactic * mk_pp_sat_tactic(ast_manager & m, params_ref const & p) {

    tactic * new_sat = cond(mk_produce_proofs_probe(),
                            and_then(mk_simplify_tactic(m), mk_smt_tactic(m, p)),
                            mk_psat_tactic(m, p));
    return mk_pp_sat_tactic(m, p, new_sat);
}



tactic * mk_pp_qfbv_light_tactic(ast_manager & m, params_ref const & p) {
    //	return mk_qfbv_sls_tactic(m, p);
    tactic * new_sat = cond(mk_produce_proofs_probe(),
                            and_then(mk_simplify_tactic(m), mk_smt_tactic(m, p)),
                            mk_psat_tactic(m, p));

    return mk_pp_qfbv_light_tactic(m, p, new_sat);

}


