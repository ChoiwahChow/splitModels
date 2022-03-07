/*
 * Invariant.cpp
 *
 *  Created on: May 1, 2021
 *      Author:
 */

#include <algorithm>
#include <sstream>
#include <iostream>
#include "Interpretation.h"
#include "Invariant.h"

struct CompareInvariantVec
{
	inline bool operator()(const std::vector<int>& vec1, const std::vector<int>& vec2) const
	{
		// compare element-by-element, return when there is a difference between the two, true if vec1[x] < vec2[x], false otherwise
		for (size_t idx = 0; idx < vec1.size(); ++idx) {
			if (vec1[idx] < vec2[idx])
				return true;
			else if (vec1[idx] > vec2[idx])
				return false;
		}
		return false;
	}
};


int Invariant::find_num_unique_el(const std::vector<int>& vec)
{
	/* find number of unique elements in a vector/array */
	int s = vec.size();
	int buf[s];
	std::fill(buf, buf+s, 0);
	int sum = 0;
	for (int idx = 0; idx < s; ++idx) {
		if (buf[vec[idx]] == 0) {
			buf[vec[idx]] = 1;
			++sum;
		}
	}
	return sum;
}


int Invariant::find_num_unique_el(int s, const int* vec)
{
	/* find number of unique elements in a vector/array */
	int buf[s];
	std::fill(buf, buf+s, 0);
	int sum = 0;
	for (int idx = 0; idx < s; ++idx) {
		if (buf[vec[idx]] == 0) {
			buf[vec[idx]] = 1;
			++sum;
		}
	}
	return sum;
}


int Invariant::count_non_zero(int s, const int vec[])
{
	/* find number of non zero elements in a vector */
	int sum = 0;
	for (int idx = 0; idx < s; idx++)
		if (vec[idx] > 0)
			++sum;
	return sum;
}


int Invariant::calc_ternary_invariant_vec(const std::vector<int>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos)
{
	/*
	 * Special case:
	 * Calculate the invariant vector for a ternary function, which is coded as the 2d-vector mt.
	 * But only the first row is used because we don't actually use the multiplication table.
	 * Only one invariant is calculated: the number of time a domain element appears in the (real)
	 * multiplication table. This is already done when the model is parsed, and the are results
	 * stored on the first row of the "multiplication" table.
	 */
	int domain_size = (int)inv_vec.size();
	for (int el = 0; el < domain_size; ++el) {
		inv_vec[el][inv_pos] = 0;
	}

	int i_no = inv_pos;
	/* Invariant 1:
	 * For each domain element x, number of times it appears in the multiplication table
	 */
	for (int el = 0; el < domain_size; el++) {
		inv_vec[el][i_no] = mt[el];
	}
	return i_no - inv_pos + 1;
}


int Invariant::calc_unary_invariant_vec(const std::vector<int>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos)
{
	/*
	 * Calculate the invariant vector for a unary function
	 * The unary function is coded as the 2-d vector mt.
	 * But only the first row is used because it is a unary function.
	 * The invariants for each element is a row in inv_vec (1-d array of domain_size pointers)
	 * That is, each column in inv_vec is the same invariant for each element.
	 */
	int domain_size = inv_vec.size();
	for (int idx = 0; idx < domain_size; ++idx) {
		for (int jdx = 0; jdx < unary_inv_count; jdx++)
			inv_vec[idx][inv_pos+jdx] = 0;
	}

	int i_no = inv_pos;
	/* Invariant 1: idempotency
	 * For each domain element x, 1 if x' = x
	 */
	for (int el = 0; el < domain_size; el++) {
		if (mt[el] == el)
			inv_vec[el][i_no] = 1;
	}

	++i_no;
	/* Invariant 2: unary of unary equals self
	 * For each domain element x, 1 if x'' = x & x' != x, 0 otherwise
	 */
	for (int el = 0; el < domain_size; ++el) {
		if (mt[el] != el && mt[mt[el]] == el)
			inv_vec[el][i_no] = 1;
	}

	++i_no;
	/* Invariant 3: range
	 * For each domain element x, number of y such that y' = x
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			if (mt[jdx] == el)
				inv_vec[el][i_no] += 1;
		}
	}

	++i_no;
	/* Invariant 4: range of range
	 * For each domain element x, number of y such that y'' = x
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			if (mt[mt[jdx]] == el)
				inv_vec[el][i_no] += 1;
		}
	}
	return i_no - inv_pos + 1;
}


int Invariant::calc_binary_relation_invariant_vec(const std::vector<std::vector<int>>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos)
{
	/*
	 * Calculate the invariant vector for a binary relation
	 * The binary relation is coded as the 2-d vector mt.
	 * The invariants for each element is a row in inv_vec (1-d vector of domain_size vectors)
	 * That is, each column in inv_vec is the same invariant for each element.
	 */
	int domain_size = inv_vec.size();
	for (int idx = 0; idx < domain_size; ++idx) {
		for (int jdx = 0; jdx < binary_relation_inv_count; jdx++)
			inv_vec[idx][inv_pos+jdx] = 0;
	}

	int i_no = inv_pos;
	/* Invariant 1: relatedness
	 * For each domain element x, number of y such that R(x, y)
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx)
			if (mt[el][jdx] == 1)
				inv_vec[el][i_no] += 1;
	}

	++i_no;
	/* Invariant 2: inverse relatedness
	 * For each domain element x, number of R(y, x)
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx)
			if (mt[jdx][el] == 1)
				inv_vec[el][i_no] += 1;
	}

	++i_no;
	/* Invariant 3: reflexivity
	 * For each domain element x, R(x, x)
	 */
	for (int el = 0; el < domain_size; ++el) {
		if (mt[el][el] == 1)
			inv_vec[el][i_no] += 1;
	}

	++i_no;
	/* Invariant 4:
	 * For each domain element x, the number of y such that R(x, y) & R(y, x)
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			if (mt[el][jdx] == 1 && mt[jdx][el] == 1) {
				inv_vec[el][i_no] += 1;
			}
		}
	}
	return i_no - inv_pos + 1;
}


int Invariant::calc_binary_invariant_vec(const std::vector<std::vector<int>>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos)
{
	/*
	 * Calculate the invariant vector for a binary function
	 * The binary function is coded as the 2-d vector mt.
	 * The invariants for each element is a row in inv_vec (1-d array of domain_size vectors)
	 * That is, each column in inv_vec is the same invariant for each element.
	 */
	int domain_size = inv_vec.size();
	for (int idx = 0; idx < domain_size; ++idx) {
		for (int jdx = 0; jdx < binary_inv_count; jdx++)
			inv_vec[idx][inv_pos+jdx] = 0;
	}

	int elementSquared[domain_size];

	for (int idx = 0; idx < domain_size; ++idx) {
		elementSquared[idx] = mt[idx][idx];
	}

	int i_no = inv_pos;
	/* Invariant 1: min exponent
	 * what is the smallest exponent n that recurs, that is, s^n = s^k such that n > k >= 1.
	 * n is clearly <= domain size + 1
	 * Note that s^n = (..(s*s)*s)*s)... for n of the s.
     */
	for (int el = 0; el < domain_size; ++el) {
		int new_power = el;
		int ElPowers[domain_size];
		std::fill(ElPowers, ElPowers+domain_size, 0);
		ElPowers[el] = 1;
		for (int pow = 2; pow <= domain_size+2; ++pow) {
			new_power = mt[new_power][el];
			if (ElPowers[new_power] == 1) {
				inv_vec[el][i_no] = pow;
				break;
			}
			ElPowers[new_power] = 1;
		}
	}

	++i_no;
	/* Invariant 4: number of inverses
	 * For each element x, number of elements y such that x = (xy)x
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx)
			if (mt[mt[el][jdx]][el] == el)
				inv_vec[el][i_no] += 1;
	}

	++i_no;
	/* Invariant 5: distinct row elements
	 * Number of distinct elements on a row
	 */
	for (int el = 0; el < domain_size; ++el)
		inv_vec[el][i_no] = find_num_unique_el(mt[el]);

	++i_no;
	/* Invariant 6: distinct column elements
	 * Number of distinct elements on a column
	 */
	int column[domain_size];
	for (int el = 0; el < domain_size; ++el) {
		for (int row = 0; row < domain_size; ++row)
			column[row] = mt[row][el];
		inv_vec[el][i_no] = find_num_unique_el(domain_size, column);
	}

	++i_no;
	/* Invariant 7: idempotent
	 * Am I idempotent? 1 or 0
	 */
	for (int el = 0; el < domain_size; ++el)
		if (elementSquared[el] == el)
			inv_vec[el][i_no] = 1;

	++i_no;
	/* Invariant 12: double centralizer
	 * How many elements of which the square I commute with?
	 * For each x, number of y such that x(yy) = (yy)x
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			if (mt[el][elementSquared[jdx]] == mt[elementSquared[jdx]][el])
				inv_vec[el][i_no] += 1;
		}
	}

	++i_no;
	/* Invariant 13: square root
	 * number of element of which I am a square root
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx)
			if (elementSquared[jdx] == el) {
				inv_vec[el][i_no] += 1;
				// inv_vec[jdx][i_no] += 1;
			}
	}

	++i_no;
	/* Invariant 14: associatizer, for each domain element x,
	 * Number of elements y satisfying x(xy) = (xx)y
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			if (mt[el][mt[el][jdx]] == mt[elementSquared[el]][jdx])
				inv_vec[el][i_no] += 1;
		}
	}

	++i_no;
	/* Invariant 15: product of commutators, for each domain element s
	 * Number of products xy = yx = s
	 */
	for (int idx = 0; idx < domain_size; ++idx) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			int s = mt[idx][jdx];
			if (s == mt[jdx][idx])
				inv_vec[s][i_no] += 1;
		}
	}

	++i_no;
	/* Invariant 17: ordering
	 * For each s,
	 * number of t such that exist two elements x,y in M such that s=xy and t=yx
	 */

	// vec_list[s][t] == 1 means exists x, y such that s=xy and t=yx
	int vec_list[domain_size][domain_size];
	for (int idx = 0; idx < domain_size; ++idx)
		std::fill(vec_list[idx], vec_list[idx]+domain_size, 0);

	for (int idx = 0; idx < domain_size; ++idx) {
		for (int jdx = 0; jdx < domain_size; ++jdx)
			vec_list[mt[idx][jdx]][mt[jdx][idx]] = 1;
	}
	for (int el = 0; el < domain_size; ++el)
		inv_vec[el][i_no] = count_non_zero(domain_size, vec_list[el]);

	return i_no - inv_pos + 1;
}


int Invariant::calc_binary_invariant_vec2(const std::vector<std::vector<int>>& mt, std::vector<std::vector<int>>& inv_vec, int inv_pos)
{
	/*
	 * Let (A,*) be a model. Let a in A.

		R(a):={(x,y) | x * y = a }.

		The elements of R(a) can be seen as arrows in a direct graph x->y.

		P1(a) := {x | exists y in A such that (x,y) in R(a)} %This is called the first projection.

		P2(a) := {y | exists x in A such that (x,y) in R(a)} %This is called the second projection.

		I(a):={(x,y) in R(a) | x = a} %Initial vertex a
		T(a):={(x,y) in R(a) | y = a} %terminal vertex a

		M(a):= {x in A | a->x->a}
		M2(a):={(x,y) in R(a) | a ->x->y->a}
		M3(a):={(x,y,z) | a->x->y->z->a}
		...
		Tr(a):={(x,y,z) | x->y->z->x} %Triangle on R(a).
		Sq(a):={(x,y,z,w) | x->y->z->w->x and no subset is a triangle} %Squares on R(a).
		etc.

		S(a) :={(x,y) in R(a) | (y,x) in R(a) }.
		The elements in S(a) can be denoted as {x,y} or x-y.

		A(a) :={{x,y} in S(a) | a in {x,y}}.
		TrA(a):={{x,y,z} | x-y-z-x}.
		SqA(a):={{x,y,z,w} |x-y-z-w and no subset belongs to the previous}
		Etc.
	 */

	/*
	 * Calculate the invariant vector for a binary function
	 * The binary function is coded as the 2-d vector mt.
	 * The invariants for each element is a row in inv_vec (1-d array of domain_size vectors)
	 * That is, each column in inv_vec is the same invariant for each element.
	 */
	int domain_size = inv_vec.size();
	for (int idx = 0; idx < domain_size; ++idx) {
		for (int jdx = 0; jdx < binary_inv_count; jdx++)
			inv_vec[idx][inv_pos+jdx] = 0;
	}

	int i_no = inv_pos;
	/* P1(a): First projection. For each element a,
	 * Number of x such that x * y = a for some y
	 * This is the same as number of rows that a appears in the multiplication table
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int idx = 0; idx < domain_size; ++idx) {
			for (int jdx = 0; jdx < domain_size; ++jdx) {
				if (mt[idx][jdx] == el) {
					inv_vec[el][i_no] += 1;
					break;
				}
			}
		}
	}

	++i_no;
	/* P2(a): Second projection. For each element a,
	 * Number of y such that x * y = a for some x
	 * This is the same as number of columns that a appears in the multiplication table
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int idx = 0; idx < domain_size; ++idx) {  // column
			for (int jdx = 0; jdx < domain_size; ++jdx) {   // row
				if (mt[jdx][idx] == el) {
					inv_vec[el][i_no] += 1;
					break;
				}
			}
		}
	}

	++i_no;
	/* M(a): For each element a,
	 * Number of x such that a * x = a for some x
	 * This is the same as the number of right identities for a
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int idx = 0; idx < domain_size; ++idx) {
			if (mt[el][idx] == el)
				inv_vec[el][i_no] += 1;
		}
	}

	return i_no - inv_pos + 1;
}

void Invariant::add_space(std::vector<std::vector<int>>& all_inv_vec, int num_space)
{
	for (size_t el=0; el < all_inv_vec.size(); el++) {
		for (int idx=0; idx < num_space; ++idx)
			all_inv_vec[el].push_back(0);
	}
}

void Invariant::calc_invariant_vec(int domain_size, int num_ops, std::vector<std::vector<std::vector<int>>>& all_mt,
		std::vector<std::vector<int>>& all_inv_vec, std::vector<int>& op_type, std::vector<std::string>& op_sym)
{
	/*
	 * Calculate the invariant vector for each domain element based on the multiplication table
	 */
	bool allocate_space = all_inv_vec.size() == 0;
	if (allocate_space) {
		for (int el=0; el<domain_size; el++)
			all_inv_vec.push_back(std::vector<int>());
	}
	int inv_pos = 0;
	int count = 0;
	for (int op = 0; op < num_ops; ++op) {
		if (op_type[op] == InterpretationType::unary_function) {
			if (allocate_space)
				add_space(all_inv_vec, unary_inv_count);
			count = calc_unary_invariant_vec(all_mt[op][0], all_inv_vec, inv_pos);
			inv_pos += count;
		}
		else if (op_type[op] == InterpretationType::ternary_function) {
			if (allocate_space)
				add_space(all_inv_vec, ternary_inv_count);
			count = calc_ternary_invariant_vec(all_mt[op][0], all_inv_vec, inv_pos);
			inv_pos += count;
		}
		else if( op_type[op] == InterpretationType::function) { // function
			if (allocate_space)
				add_space(all_inv_vec, binary_inv_count);
			count = calc_binary_invariant_vec(all_mt[op], all_inv_vec, inv_pos);
			inv_pos += count;
		}
		else {
			if (allocate_space)
				add_space(all_inv_vec, binary_relation_inv_count);
			count = calc_binary_relation_invariant_vec(all_mt[op], all_inv_vec, inv_pos);
			inv_pos += count;
		}
	}
	/* debug print
	std::cout << all_inv_vec[0][0] << " ";
	for( int x=11; x <13; x++)
		std::cout << all_inv_vec[0][x] << " ";
	std::cout << std::endl;
	std::cout << all_inv_vec[1][0] << " ";
	for( int x=11; x <13; x++)
		std::cout << all_inv_vec[1][x] << " ";
	std::cout << std::endl;
	*/
}


void Invariant::hash_key(const std::vector<std::vector<int>>& all_inv_vec, std::string& key)
{
	std::stringstream ss;
	for (size_t el = 0; el < all_inv_vec.size(); ++el) {
		for (size_t jdx = 0; jdx < all_inv_vec[el].size(); ++jdx)
			ss << all_inv_vec[el][jdx] << ",";
	}
	key = ss.str();
}


void Invariant::sort_invariant_vec(std::vector<std::vector<int>>& all_inv_vec)
{
	std::sort(all_inv_vec.begin(), all_inv_vec.end(), CompareInvariantVec());
}
