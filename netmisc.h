#ifndef __netmisc_H
#define __netmisc_H
/*
 * Copyright (c) 1999-2000 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#ifdef HAVE_CVS_IDENT
#ident "$Id: netmisc.h,v 1.31 2007/06/02 03:42:13 steve Exp $"
#endif

# include  "netlist.h"

/*
 * Search for a symbol using the "start" scope as the starting
 * point. If the path includes a scope part, then locate the
 * scope first.
 *
 * The return value is the scope where the symbol was found.
 * If the symbol was not found, return 0. The output arguments
 * get 0 except for the pointer to the object that represents
 * the located symbol.
 *
 * The ex1 and ex2 output arguments are extended results. If the
 * symbol is a parameter (par!=0) then ex1 is the msb expression and
 * ex2 is the lsb expression for the range. If there is no range, then
 * these values are set to 0.
 */
extern NetScope* symbol_search(Design*des,
			       NetScope*start, pform_name_t path,
			       NetNet*&net,       /* net/reg */
			       const NetExpr*&par,/* parameter */
			       NetEvent*&eve,     /* named event */
			       const NetExpr*&ex1, const NetExpr*&ex2);

inline NetScope* symbol_search(Design*des,
			       NetScope*start, const pform_name_t&path,
			       NetNet*&net,       /* net/reg */
			       const NetExpr*&par,/* parameter */
			       NetEvent*&eve      /* named event */)
{
      const NetExpr*ex1, *ex2;
      return symbol_search(des, start, path, net, par, eve, ex1, ex2);
}

/*
 * This function transforms an expression by padding the high bits
 * with V0 until the expression has the desired width. This may mean
 * not transforming the expression at all, if it is already wide
 * enough.
 */
extern NetExpr*pad_to_width(NetExpr*expr, unsigned wid);
extern NetNet*pad_to_width(Design*des, NetNet*n, unsigned w);

extern NetNet*pad_to_width_signed(Design*des, NetNet*n, unsigned w);

/*
 * This function transforms an expression by cropping the high bits
 * off with a part select. The result has the width w passed in. This
 * function does not pad, use pad_to_width if padding is desired.
 */
extern NetNet*crop_to_width(Design*des, NetNet*n, unsigned w);

/*
 * This function takes as input a NetNet signal and adds a constant
 * value to it. If the val is 0, then simply return sig. Otherwise,
 * return a new NetNet value that is the output of an addition.
 */
extern NetNet*add_to_net(Design*des, NetNet*sig, long val);

/*
 * These functions make various sorts of expressions, given operands
 * of certain type. The order of the operands is preserved in cases
 * where order matters.
 *
 * make_add_expr
 *   Make a NetEBAdd expression with <expr> the first argument and
 *   <val> the second. This may get turned into a subtract if <val> is
 *   less then zero. If val is exactly zero, then return <expr> as is.
 *
 * make_sub_expr
 *   Make a NetEBAdd(subtract) node that subtracts the given
 *   expression from the integer value.
 */
extern NetExpr*make_add_expr(NetExpr*expr, long val);
extern NetExpr*make_sub_expr(long val, NetExpr*expr);

/*
 * In some cases the lval is accessible as a pointer to the head of
 * a list of NetAssign_ objects. This function returns the width of
 * the l-value represented by this list.
 */
extern unsigned count_lval_width(const class NetAssign_*first);

/*
 * This function elaborates an expression, and tries to evaluate it
 * right away. If the expression can be evaluated, this returns a
 * constant expression. If it cannot be evaluated, it returns whatever
 * it can. If the expression cannot be elaborated, return 0.
 *
 * The expr_width is the width of the context where the expression is
 * being elaborated, or -1 if the expression is self-determined width.
 *
 * Also, the prune_width is the maximum width of the result, and it
 * passed to the eval_tree method of the expression to limit constant
 * results if possible.
 */
class PExpr;
extern NetExpr* elab_and_eval(Design*des, NetScope*scope,
			      const PExpr*pe, int expr_wid,
			      int prune_width =-1);

/*
 * Get the long integer value for the passed in expression, if
 * possible. If it is not possible (the expression is not evaluated
 * down to a constant) then return false and leave value unchanged.
 */
bool eval_as_long(long&value, NetExpr*expr);

extern std::list<hname_t> eval_scope_path(Design*des, NetScope*scope,
					  const pform_name_t&path);

#endif
