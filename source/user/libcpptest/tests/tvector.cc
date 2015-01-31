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
#include <sys/test.h>
#include <stdlib.h>
#include <vector>

using namespace std;

/* forward declarations */
static void test_vector(void);
static void test_constr(void);
static void test_assign(void);
static void test_iterators(void);
static void test_insert(void);
static void test_at(void);
static void test_erase(void);
static void test_nonpod(void);

/* our test-module */
sTestModule tModVector = {
	"Vector",
	&test_vector
};

static void test_vector(void) {
	test_constr();
	test_assign();
	test_iterators();
	test_insert();
	test_at();
	test_erase();
	test_nonpod();
}

static void test_constr(void) {
	test_caseStart("Testing constructors");

	vector<int> v;
	test_assertSize(v.size(),0);

	vector<int> v2((size_t)10,4);
	test_assertSize(v2.size(),10);
	for(size_t i = 0; i < v2.size(); i++) {
		test_assertInt(v2[i],4);
		test_assertInt(v2.at(i),4);
	}

	vector<int> v3(v2.begin(),v2.end());
	test_assertSize(v3.size(),v2.size());
	for(size_t i = 0; i < v3.size(); i++)
		test_assertInt(v3[i],v2[i]);

	vector<int> v4(v3);
	test_assertSize(v4.size(),v3.size());
	for(size_t i = 0; i < v4.size(); i++)
		test_assertInt(v4[i],v3[i]);

	test_caseSucceeded();
}

static void test_assign(void) {
	test_caseStart("Testing assign");

	size_t before = heapspace();

	{
		vector<int> v1(5);
		vector<int> v2 = v1;
		test_assertSize(v1.size(),v2.size());
		for(size_t i = 0; i < v1.size(); i++)
			test_assertInt(v1[i],v2[i]);

		vector<int> v3;
		v3.assign(v1.begin() + 1,v1.end());
		test_assertSize(v3.size(),v1.size() - 1);
		test_assertInt(v3[0],v1[1]);
		test_assertInt(v3[1],v1[2]);

		vector<int> v4;
		v4.assign((size_t)8,3);
		test_assertSize(v4.size(),8);
		for(size_t i = 0; i < v4.size(); i++)
			test_assertInt(v4[i],3);
	}

	size_t after = heapspace();
	test_assertSize(after,before);

	test_caseSucceeded();
}

static void test_iterators(void) {
	test_caseStart("Testing iterators");

	vector<int> v1;
	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);
	v1.push_back(4);
	int i = 1;
	for(auto it = v1.begin(); it != v1.end(); it++)
		test_assertInt(*it,i++);
	i = 4;
	for(auto it = v1.rbegin(); it != v1.rend(); it++)
		test_assertInt(*it,i--);

	test_caseSucceeded();
}

static void test_insert(void) {
	test_caseStart("Testing insert");

	size_t before = heapspace();

	{
		vector<int>::iterator it;
		vector<int> v1;
		it = v1.insert(v1.end(),4);
		test_assertInt(*it,4);
		test_assertSize(v1.size(),1);
		test_assertInt(v1[0],4);
		it = v1.insert(v1.end(),5);
		test_assertInt(*it,5);
		test_assertSize(v1.size(),2);
		test_assertInt(v1[0],4);
		test_assertInt(v1[1],5);
		it = v1.insert(v1.begin(),1);
		test_assertInt(*it,1);
		test_assertSize(v1.size(),3);
		test_assertInt(v1[0],1);
		test_assertInt(v1[1],4);
		test_assertInt(v1[2],5);
		it = v1.insert(v1.begin() + 1,2);
		test_assertInt(*it,2);
		test_assertSize(v1.size(),4);
		test_assertInt(v1[0],1);
		test_assertInt(v1[1],2);
		test_assertInt(v1[2],4);
		test_assertInt(v1[3],5);
		it = v1.insert(v1.begin() + 2,3);
		test_assertInt(*it,3);
		test_assertSize(v1.size(),5);
		test_assertInt(v1[0],1);
		test_assertInt(v1[1],2);
		test_assertInt(v1[2],3);
		test_assertInt(v1[3],4);
		test_assertInt(v1[4],5);

		v1.insert(v1.begin() + 1,3u,8);
		test_assertSize(v1.size(),8);
		test_assertInt(v1[0],1);
		test_assertInt(v1[1],8);
		test_assertInt(v1[2],8);
		test_assertInt(v1[3],8);
		test_assertInt(v1[4],2);
		test_assertInt(v1[5],3);
		test_assertInt(v1[6],4);
		test_assertInt(v1[7],5);

		vector<int> v2(2u,6);
		v1.insert(v1.begin() + 1,v2.begin(),v2.end());
		test_assertSize(v1.size(),10);
		test_assertInt(v1[0],1);
		test_assertInt(v1[1],6);
		test_assertInt(v1[2],6);
		test_assertInt(v1[3],8);
		test_assertInt(v1[4],8);
		test_assertInt(v1[5],8);
		test_assertInt(v1[6],2);
		test_assertInt(v1[7],3);
		test_assertInt(v1[8],4);
		test_assertInt(v1[9],5);
	}

	size_t after = heapspace();
	test_assertSize(after,before);

	test_caseSucceeded();
}

static void test_at(void) {
	test_caseStart("Testing at");

	vector<int> v1;
	v1.push_back(1);
	v1.push_back(2);
	v1.push_back(3);
	test_assertInt(v1.at(0),1);
	test_assertInt(v1.at(1),2);
	test_assertInt(v1.at(2),3);
	try {
		v1.at(3);
		test_assertFalse(false);
	}
	catch(out_of_range &e) {
		test_assertTrue(true);
	}
	try {
		v1.at(-1);
		test_assertFalse(false);
	}
	catch(out_of_range &e) {
		test_assertTrue(true);
	}

	test_caseSucceeded();
}

static void test_erase(void) {
	test_caseStart("Testing erase");

	size_t before = heapspace();

	{
		vector<int>::iterator it;
		vector<int> v1;
		v1.push_back(1);
		v1.push_back(2);
		v1.push_back(3);
		v1.push_back(4);
		v1.push_back(5);

		it = v1.erase(v1.end() - 1);
		test_assertTrue(it == v1.end());
		test_assertSize(v1.size(),4);
		test_assertInt(v1[0],1);
		test_assertInt(v1[1],2);
		test_assertInt(v1[2],3);
		test_assertInt(v1[3],4);
		it = v1.erase(v1.begin() + 1);
		test_assertInt(*it,3);
		test_assertSize(v1.size(),3);
		test_assertInt(v1[0],1);
		test_assertInt(v1[1],3);
		test_assertInt(v1[2],4);
		it = v1.erase(v1.begin() + 1,v1.end());
		test_assertTrue(it == v1.end());
		test_assertSize(v1.size(),1);
		test_assertInt(v1[0],1);
		it = v1.erase(v1.begin(),v1.end());
		test_assertTrue(it == v1.end());
		test_assertSize(v1.size(),0);
	}

	size_t after = heapspace();
	test_assertSize(after,before);

	test_caseSucceeded();
}

static unsigned counter = 0;

struct NonPOD {
	NonPOD() : x(0) {
	}
	NonPOD(int _x) : x(_x) {
		attach();
	}
	NonPOD(const NonPOD &a) : x(a.x) {
		attach();
	}
	NonPOD &operator=(const NonPOD &a) {
		if(&a != this) {
			detach();
			x = a.x;
			attach();
		}
		return *this;
	}
	~NonPOD() {
		detach();
	}

private:
	void attach() {
		if(x)
			counter++;
	}
	void detach() {
		if(x)
			counter--;
		x = 0;
	}

	int x;
};

static void test_nonpod(void) {
	test_caseStart("Testing non-POD");

	size_t before = heapspace();

	{
		vector<NonPOD> v1;
		v1.push_back(NonPOD(1));
		v1.push_back(NonPOD(2));
		v1.push_back(NonPOD(3));
		v1.push_back(NonPOD(4));
		v1.push_back(NonPOD(5));
		test_assertUInt(counter,5);

		v1.insert(v1.begin() + 1,NonPOD(6));
		test_assertUInt(counter,6);

		v1.insert(v1.end() - 1,NonPOD(7));
		test_assertUInt(counter,7);

		v1.erase(v1.begin());
		test_assertUInt(counter,6);

		v1.erase(v1.end() - 1);
		test_assertUInt(counter,5);

		v1.erase(v1.begin(),v1.end());
		test_assertUInt(counter,0);
	}

	size_t after = heapspace();
	test_assertSize(after,before);

	test_caseSucceeded();
}
