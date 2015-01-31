/**
 * $Id$
 * Copyright (C) 2008 - 2014 Nils Asmussen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <sys/common.h>
#include <stdio.h>

#include "../mem.h"
#include "intexpr.h"
#include "node.h"

sASTNode *ast_createIntExpr(tIntType val) {
	sASTNode *node = (sASTNode*)emalloc(sizeof(sASTNode));
	sIntExpr *expr = (sIntExpr*)emalloc(sizeof(sIntExpr));
	node->data = expr;
	expr->val = val;
	node->type = AST_INT_EXPR;
	return node;
}

sValue *ast_execIntExpr(A_UNUSED sEnv *e,sIntExpr *n) {
	return val_createInt(n->val);
}

void ast_printIntExpr(sIntExpr *s,A_UNUSED uint layer) {
	printf("%d",s->val);
}

void ast_destroyIntExpr(A_UNUSED sIntExpr *n) {
	/* nothing to do */
}
