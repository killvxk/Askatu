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
#include <sys/sllist.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "jobs.h"

static sSLList *jobs = NULL;

void jobs_init(void) {
	jobs = sll_create();
	vassert(jobs != NULL,"Unable to create linked list for jobs");
}

tJobId jobs_requestId(void) {
	tJobId id = 1;
	sSLNode *n;
	for(n = sll_begin(jobs); n != NULL; ) {
		sJob *p = (sJob*)n->data;
		if(p->jobId == id) {
			id++;
			n = sll_begin(jobs);
		}
		else
			n = n->next;
	}
	return id;
}

bool jobs_addProc(tJobId jobId,pid_t pid,int argc,char **argv,bool background) {
	sJob *run = (sJob*)malloc(sizeof(sJob));
	if(run) {
		run->background = background;
		run->jobId = jobId;
		run->pid = pid;
		run->removable = false;
		run->command = NULL;
		run->cmd = fopendyn();
		if(run->cmd) {
			for(int i = 0; i < argc; ++i)
				fprintf(run->cmd,"%s ",argv[i]);
			run->command = fgetbuf(run->cmd,NULL);
		}
		if(sll_append(jobs,run))
			return true;
		if(run->cmd)
			fclose(run->cmd);
		free(run);
	}
	return false;
}

sJob *jobs_getXProcOf(tJobId jobId,int i) {
	sSLNode *n;
	sJob *p;
	for(n = sll_begin(jobs); n != NULL; n = n->next) {
		p = (sJob*)n->data;
		if(!p->removable && p->jobId == jobId) {
			if(i-- <= 0)
				return p;
		}
	}
	return NULL;
}

sJob *jobs_findProc(tJobId jobId,pid_t pid) {
	sSLNode *n;
	sJob *p;
	for(n = sll_begin(jobs); n != NULL; n = n->next) {
		p = (sJob*)n->data;
		if(!p->removable && p->pid == pid && (jobId == CMD_ID_ALL || p->jobId == jobId))
			return p;
	}
	return NULL;
}

void jobs_gc(void) {
	sSLNode *n,*p;
	sJob *r;
	p = NULL;
	for(n = sll_begin(jobs); n != NULL; ) {
		r = (sJob*)n->data;
		if(r->removable) {
			sSLNode *next = n->next;
			sll_removeNode(jobs,n,p);
			if(r->cmd)
				fclose(r->cmd);
			free(r);
			n = next;
		}
		else {
			p = n;
			n = n->next;
		}
	}
}

void jobs_print(void) {
	sSLNode *n;
	sJob *p;
	for(n = sll_begin(jobs); n != NULL; n = n->next) {
		p = (sJob*)n->data;
		if(p->background)
			printf("%%%d: (%d) '%s' %c\n",p->jobId,p->pid,p->command,p->removable ? '*' : ' ');
	}
}

bool jobs_remProc(pid_t pid) {
	sJob *run = jobs_findProc(CMD_ID_ALL,pid);
	if(run) {
		sll_removeFirstWith(jobs,run);
		free(run);
	}
	return run != NULL;
}
