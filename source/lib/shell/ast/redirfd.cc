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
#include "node.h"
#include "redirfd.h"

sASTNode *ast_createRedirFd(uchar type) {
	sASTNode *node = (sASTNode*)emalloc(sizeof(sASTNode));
	sRedirFd *expr = (sRedirFd*)emalloc(sizeof(sRedirFd));
	node->data = expr;
	expr->type = type;
	node->type = AST_REDIR_FD;
	return node;
}

void ast_printRedirFd(sRedirFd *s,A_UNUSED uint layer) {
	switch(s->type) {
		case REDIR_ERR2OUT:
			printf(" 2>&1");
			break;
		case REDIR_OUT2ERR:
			printf(" 1>&2");
			break;
	}
}

void ast_destroyRedirFd(A_UNUSED sRedirFd *n) {
	/* nothing to do */
}
