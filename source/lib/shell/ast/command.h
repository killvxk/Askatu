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
#include "../exec/jobs.h"
#include "node.h"

typedef struct {
	bool runInBG;
	bool retOutput;
	sSLList *subList;
} sCommand;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Creates a command
 *
 * @return the created node
 */
sASTNode *ast_createCommand(void);

/**
 * Executes the given node(-tree)
 *
 * @param e the environment
 * @param n the node
 * @return the value
 */
sValue *ast_execCommand(sEnv *e,sCommand *n);

/**
 * Termiates processes of the given command.
 *
 * @param cmd the command id
 */
void ast_termProcsOfJob(tJobId cmd);

/**
 * Catch all zombies that are around
 */
void ast_catchZombies(void);

/**
 * Sets whether this command should return the output
 *
 * @param c the command
 * @param retOutput whether to return the output
 */
void ast_setRetOutput(sASTNode *c,bool retOutput);

/**
 * Sets whether this command should run in bg
 *
 * @param c the command
 * @param runInBG whether to run in background
 */
void ast_setRunInBG(sASTNode *c,bool runInBG);

/**
 * Adds <sub> to the command <c>.
 *
 * @param c the command
 * @param sub the sub-command
 */
void ast_addSubCmd(sASTNode *c,sASTNode *sub);

/**
 * Prints this command
 *
 * @param s the command
 * @param layer the layer
 */
void ast_printCommand(sCommand *s,uint layer);

/**
 * Destroys the given command (should be called from ast_destroy() only!)
 *
 * @param n the command
 */
void ast_destroyCommand(sCommand *n);

#if defined(__cplusplus)
}
#endif
