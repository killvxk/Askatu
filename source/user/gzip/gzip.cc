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

#include <ask/cmdargs.h>
#include <ask/vthrow.h>
#include <sys/common.h>
#include <sys/endian.h>
#include <z/deflate.h>
#include <z/gzip.h>
#include <stdio.h>
#include <stdlib.h>

static int compr = z::Deflate::FIXED;
static int tostdout = false;
static int keep = false;

static void compress(FILE *f,const std::string &filename) {
	FILE *out = stdout;
	if(!tostdout) {
		std::string name = filename + ".gz";
		out = fopen(name.c_str(),"w");
		if(!out) {
			printe("%s: unable to open for writing",name.c_str());
			return;
		}
	}

	z::GZipHeader header(filename.c_str(),NULL,true);
	header.write(out);

	z::FileDeflateSource src(f);
	z::FileDeflateDrain drain(out);
	z::Deflate deflate;
	if(deflate.compress(&drain,&src,compr) != 0)
		printe("%s: compressing failed",filename.c_str());
	else {
		uint32_t crc32 = src.crc32();
		if(fwrite(&crc32,4,1,out) != 1)
			printe("%s: unable to write CRC32",filename.c_str());
		else {
			uint32_t orgsize = src.count();
			if(fwrite(&orgsize,4,1,out) != 1)
				printe("%s: unable to write size of original file",filename.c_str());
		}
	}

	if(!tostdout) {
		fclose(out);
		if(!keep && unlink(filename.c_str()) < 0)
			printe("Unable to unlink '%s'",filename.c_str());
	}
}

static void usage(const char *name) {
	fprintf(stderr,"Usage: %s [-c] [-k] <file>...\n",name);
	fprintf(stderr,"  -c: write to stdout\n");
	fprintf(stderr,"  -k: keep the original files, don't delete them\n");
	exit(EXIT_FAILURE);
}

int main(int argc,char **argv) {
	ask::cmdargs args(argc,argv,0);
	try {
		args.parse("c l=d k",&tostdout,&compr,&keep);
		if(args.is_help() ||
			(compr != z::Deflate::NONE && compr != z::Deflate::FIXED && compr != z::Deflate::DYN))
			usage(argv[0]);
	}
	catch(const ask::cmdargs_error& e) {
		fprintf(stderr,"Invalid arguments: %s\n",e.what());
		usage(argv[0]);
	}

	for(auto file = args.get_free().begin(); file != args.get_free().end(); ++file) {
		FILE *f = fopen((*file)->c_str(),"r");
		if(f == NULL) {
			printe("Unable to open '%s' for reading",(*file)->c_str());
			continue;
		}

		compress(f,**file);
		fclose(f);
	}
	return 0;
}
