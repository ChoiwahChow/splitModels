/*
 * TreeNode.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: uabcoimbra
 */

#include <math.h>
#include <numeric>
#include <sstream>
#include "TreeNode.h"

constexpr const char ValueNode::labels[];

TreeNode::~TreeNode() {
	// TODO Auto-generated destructor stub
}

void TreeNode::to_string(std::ostream& out, bool paran) const
{
}

ValueNode::~ValueNode() {
	// TODO Auto-generated destructor stub
}


void ValueNode::to_string(std::ostream& out, bool paran) const
{
	out << ValueNode::labels[value];
};

OpNode::~OpNode() {

}

void OpNode::to_string(std::ostream& out, bool paran) const
{
	if (paran)
		out << "(";
	left->to_string(out);
	out << " " << sym << " ";
	right->to_string(out);
	if (paran)
		out << ")";
}

void Tree::clear_memory()
{
	this->avail_op_node_pos = 0;
	this->avail_value_node_pos = 0;
}

Tree::Tree(unsigned int depth, unsigned int num_labels):
		op(nullptr), sym("="), left(nullptr), right(nullptr), depth(depth),
		num_labels(num_labels), op_node_size(int(pow(2.0, depth))), value_node_size(op_node_size*2),
		avail_op_node_pos(0), avail_value_node_pos(0)
{
}

void Tree::initialize(std::vector<int**>& all_mt, std::vector<std::string>& op_sym,
		std::vector<int**>& all_bin_relation_mt, std::vector<std::string>& bin_relation_op_sym, unsigned int seed)
{
	all_op_nodes = std::vector<OpNode>(op_node_size);
	all_value_nodes = std::vector<ValueNode>(value_node_size);
	srand(seed);
	build_tree_structure(all_mt, op_sym, all_bin_relation_mt, bin_relation_op_sym);
}

ValueNode* Tree::build_value_node()
{
	ValueNode* v_node = &all_value_nodes[avail_value_node_pos++];
	int label_to_use = rand()%num_labels;
	label_in_use[label_to_use] = 1;
	v_node->set_value(label_to_use);
	return v_node;
}

OpNode* Tree::build_op_node(int** mt, std::string& op_sym)
{
	OpNode* v_node = &all_op_nodes[avail_op_node_pos++];
	v_node->set_op(mt);
	v_node->set_sym(op_sym);
	return v_node;
}

void Tree::build_node(TreeNode** node, unsigned int level, std::vector<int**>& all_mt, std::vector<std::string>& op_sym, bool top_is_op)
{
	bool is_value_node = !top_is_op && (level == depth || rand() % 2 == 0);
	if (is_value_node) {
		*node = build_value_node();
	}
	else {
		int which_op = rand() % op_sym.size();
		*node = build_op_node(all_mt[which_op], op_sym[which_op]);
		build_node(&(*node)->left, level+1, all_mt, op_sym);
		build_node(&(*node)->right, level+1, all_mt, op_sym);
	}
}

void Tree::build_tree_structure(std::vector<int**>& all_mt, std::vector<std::string>& op_sym,
		std::vector<int**>& all_bin_relation_mt, std::vector<std::string>& bin_relation_op_sym)
{
	label_in_use = std::vector<int>(num_labels, 0);
	build_node(&left, 1, all_mt, op_sym, true);
	build_node(&right, 1, all_mt, op_sym);
	if (all_bin_relation_mt.size() > 0) {
		if (rand() % 2 == 0) {
			sym = bin_relation_op_sym[0];
			op = all_bin_relation_mt[0];
		}
	}
/*
	left->to_string(std::cerr, false);
	std::cerr << " " << sym << " ";
	right->to_string(std::cerr, false);
	std::cerr << std::endl;
*/
}

int Tree::find_next_1(std::vector<int> vec, unsigned int start_pos) {
	for(unsigned int idx=start_pos; idx<vec.size(); idx++) {
		if (vec[idx] > 0)
			return idx;
	}
	return -1;
}

void Tree::calc_invariant_vector(unsigned int domain_size)
{
	// check that domain_size >= num_labels

	std::vector<int> values(domain_size, 0);
	int num_label_in_use = std::accumulate(label_in_use.begin(), label_in_use.end(), decltype(label_in_use)::value_type(0));
	std::fill(invariants, invariants+domain_size, 0);
	int pos0 = find_next_1(label_in_use, 0);
	if (num_label_in_use == 1) {
		for(unsigned int el = 0; el < domain_size; el++) {
			values[pos0] = el;
			int left_value = left->eval_node(values);
			int right_value = right->eval_node(values);
			if ((op != nullptr && op[left_value][right_value]) ||
					(op == nullptr && left_value == right_value)) {
				invariants[el] +=1;
			}
		}
	}
	else if (num_label_in_use == 2) {
		int pos1 = find_next_1(label_in_use, pos0+1);
		for(unsigned int el = 0; el < domain_size; el++) {
			values[pos0] = el;
			for (unsigned int jdx = 0; jdx < domain_size; jdx++) {
				values[pos1] = jdx;
				int left_value = left->eval_node(values);
				int right_value = right->eval_node(values);
				if ((op != nullptr && op[left_value][right_value]) ||
						(op == nullptr && left_value == right_value)) {
					invariants[el] +=1;
				}
			}
		}
	}
	else if (num_label_in_use == 3) {
		int pos1 = find_next_1(label_in_use, pos0+1);
		int pos2 = find_next_1(label_in_use, pos1+1);
		for(unsigned int el = 0; el < domain_size; el++) {
			values[pos0] = el;
			for (unsigned int jdx = 0; jdx < domain_size; jdx++) {
				values[pos1] = jdx;
				for (unsigned int kdx = 0; kdx < domain_size; kdx++) {
					values[pos2] = kdx;
					int left_value = left->eval_node(values);
					int right_value = right->eval_node(values);
					if ((op != nullptr && op[left_value][right_value]) ||
							(op == nullptr && left_value == right_value)) {
						invariants[el] +=1;
					}
				}
			}
		}
	}
	//std::cerr << "Number labels " << num_label_in_use << " invariants ";
	//print_invariants(domain_size);
}

int OpNode::eval_node(std::vector<int> values)
{
	int left_value  = left->eval_node(values);
	int right_value = right->eval_node(values);
	return op[left_value][right_value];
}

void Tree::print_invariants(unsigned int domain_size) const
{
	for( unsigned int i=0; i<domain_size; i++) {
		std::cerr << invariants[i] << ", ";
	}
	std::cerr << std::endl;
}


Tree::~Tree() {

}

