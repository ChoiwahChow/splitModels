/*
 * Invariant.cpp
 *
 *  Created on: May 1, 2021
 *      Author:
 */

#include <algorithm>
#include <sstream>
#include <iostream>
#include "Invariant.h"

struct VecComparator
{
	int total_invariant_length = Invariant::invariant_size;

	VecComparator(int length = Invariant::invariant_size) {
		total_invariant_length = length;
	}

	inline bool operator()(const int* vec1, const int* vec2) const
	{
		// compare element-by-element, return when there is a difference between the two, true if vec1[x] < vec2[x], false otherwise
		for (int idx = 0; idx < total_invariant_length; ++idx)
			if (vec1[idx] < vec2[idx])
				return true;
			else if (vec1[idx] > vec2[idx])
				return false;
		return false;
	}
};


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



void Invariant::calc_invariant_vec(int domain_size, int num_binop, std::vector<int**>& all_mt, std::vector<int**>& all_inv_vec, std::vector<int*>& pre_calced)
{
	for (int idx = 0; idx < num_binop; ++idx) {
		calc_invariant_vec(domain_size, all_mt[idx], all_inv_vec[idx]);
	}
	int binpos = num_binop;
	int col = 0;
	for (unsigned int idx = 0; idx < pre_calced.size(); ++idx) {
		for( int jdx = 0; jdx < domain_size; ++jdx) {
			all_inv_vec[binpos][jdx][idx%invariant_size] = pre_calced[idx][jdx];
		}
		col++;
		if (col == invariant_size) {
			col = 0;
			binpos++;
		}
	}
}


void Invariant::sort_invariant_vec(int domain_size, int num_binop, int** combo_inv_vec)
{
	std::sort(combo_inv_vec, combo_inv_vec+domain_size, VecComparator(num_binop * invariant_size));
}


void Invariant::calc_relation_invariant_vec(int domain_size, int** mt, int** inv_vec)
{
	for (int idx = 0; idx < domain_size; ++idx) {
		std::fill(inv_vec[idx], inv_vec[idx]+invariant_size, 0);
	}

	int i_no = 0;
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
	 * For each domain element x, number of y such that R(y, x)
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
	/* Invariant 4: transitivity
	 * For each domain element x, the number of y and z pairs such that R(x, y) & R(y, z) & R(x, z)
	 */
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < domain_size; ++jdx) {
			if (mt[el][jdx] == 1) {
				for (int kdx = 0; kdx < domain_size; ++kdx) {
					if (mt[jdx][kdx] == 1)
						inv_vec[el][i_no] += 1;
				}
			}
		}
	}
}


void Invariant::calc_invariant_vec(int domain_size, int** mt, int** inv_vec)
{
	int elementSquared[domain_size];

	for (int idx = 0; idx < domain_size; ++idx) {
		elementSquared[idx] = mt[idx][idx];
	}

	for (int idx = 0; idx < domain_size; ++idx) {
		std::fill(inv_vec[idx], inv_vec[idx]+invariant_size, 0);
	}

	int i_no = 0;
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
		inv_vec[el][i_no] = find_num_unique_el(domain_size, mt[el]);

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
}


void Invariant::hash_key(int domain_size, int num_binop, int** combo_in_vec, std::string& key)
{
	std::stringstream ss;
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < num_binop*invariant_size; ++jdx)
			ss << combo_in_vec[el][jdx] << ",";
	}
	key = ss.str();
}


void Invariant::hash_key(int domain_size, int** inv_vec, std::string& key)
{
	std::stringstream ss;
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < invariant_size; ++jdx)
			ss << inv_vec[el][jdx] << ",";
		ss << "|";
	}
	key = ss.str();
}

void Invariant::calc_cross_invariant_vec(int domain_size, int num_binop, std::vector<int**> all_mt, std::vector<int**> all_inv_vec)
{
	calc_2_invariant_vec(domain_size, all_mt[0], all_mt[1], all_inv_vec[0]);
}


void Invariant::sort_cross_invariant_vec(int domain_size,  int** combo_inv_vec)
{
	std::sort(combo_inv_vec, combo_inv_vec+domain_size, VecComparator(Invariant::invariant_size_2));
}


void Invariant::cross_hash_key(int domain_size, int** combo_in_vec, std::string& key)
{
	std::stringstream ss;
	for (int el = 0; el < domain_size; ++el) {
		for (int jdx = 0; jdx < invariant_size_2; ++jdx)
			ss << combo_in_vec[el][jdx] << ",";
	}
	key = ss.str();
	std::cerr << key << std::endl;
}


void Invariant::calc_2_invariant_vec(int domain_size, int** mt1, int** mt2, int** inv_vec)
{
	for (int idx = 0; idx < domain_size; ++idx) {
		std::fill(inv_vec[idx], inv_vec[idx]+invariant_size_2, 0);
	}

	int i_no = 0;
	/* Invariant 1:
	 * x * (y + z) = (y * x) + (z * z).
     */
	for (int xdx = 0; xdx < domain_size; ++xdx) {
		for (int ydx = 0; ydx < domain_size; ++ydx) {
			for (int zdx = 0; zdx < domain_size; ++zdx) {
				if (mt2[xdx][mt1[ydx][zdx]] == mt1[mt2[ydx][xdx]][mt2[zdx][zdx]]) {
					inv_vec[xdx][i_no] += 1;
					break;
				}
			}
		}
	}

	++i_no;
	/* Invariant 2:
	 * ((y + ( x * y )) + z) * z = ((z + x) * z) * z.
     */
	for (int xdx = 0; xdx < domain_size; ++xdx) {
		for (int ydx = 0; ydx < domain_size; ++ydx) {
			for (int zdx = 0; zdx < domain_size; ++zdx) {
				if (mt2[mt1[ydx][mt2[xdx][ydx]]][zdx] == mt2[mt2[mt1[zdx][xdx]][zdx]][zdx]) {
					inv_vec[xdx][i_no] += 1;
					break;
				}
			}
		}
	}

	++i_no;
	/* Invariant 3:
	 * x + y = y * x.
     */
	for (int xdx = 0; xdx < domain_size; ++xdx) {
		for (int ydx = 0; ydx < domain_size; ++ydx) {
			if (mt1[xdx][ydx] == mt2[ydx][xdx]) {
				inv_vec[xdx][i_no] += 1;
			}
		}
	}
}


