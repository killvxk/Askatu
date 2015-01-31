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

#pragma once

#include <sys/common.h>
#include <sys/sllist.h>

#include "../exec/env.h"
#include "node.h"

typedef struct {
	sSLList *list;
} sExprList;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Creates a expression-list-node
 *
 * @return the created node
 */
sASTNode *ast_createExprList(void);

/**
 * Executes the given node(-tree)
 *
 * @param e the environment
 * @param n the node
 * @return the value
 */
sValue *ast_execExprList(sEnv *e,sExprList *n);

/**
 * Adds the given expression to the list
 *
 * @param l the list
 * @param e the expression
 * @return the list
 */
sASTNode *ast_addExpr(sASTNode *l,sASTNode *e);

/**
 * Prints this expression-list
 *
 * @param s the list
 * @param layer the layer
 */
void ast_printExprList(sExprList *s,uint layer);

/**
 * Destroys the given expression-list (should be called from ast_destroy() only!)
 *
 * @param n the list
 */
void ast_destroyExprList(sExprList *n);

#if defined(__cplusplus)
}
#endif
