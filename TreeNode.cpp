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

UnaryOpNode::~UnaryOpNode() {

}

void UnaryOpNode::to_string(std::ostream& out, bool paran) const
{
	out << "(";
	left->to_string(out);
	out << ")";
	out << sym << " ";
}

Tree::Tree(unsigned int depth, unsigned int num_labels):
		op(nullptr), sym("="), left(nullptr), right(nullptr), depth(depth),
		num_labels(num_labels), op_node_size(int(pow(2.0, depth))), unary_op_nodes_size(op_node_size+1),
		value_node_size(op_node_size+1), avail_op_node_pos(0), avail_value_node_pos(0), avail_unary_op_node_pos(0)
{
}

void Tree::initialize(std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt,
		std::vector<std::string>& op_sym, std::vector<int>& all_bin_relation_mt, std::vector<std::string>& bin_relation_op_sym,
		std::vector<int>& all_unary_function_mt, std::vector<std::string>& unary_function_op_sym, unsigned int seed)
{
	all_op_nodes = std::vector<OpNode>(op_node_size);
	all_value_nodes = std::vector<ValueNode>(value_node_size);
	all_unary_op_nodes = std::vector<UnaryOpNode>(unary_op_nodes_size);
	srand(seed);
	build_tree_structure(all_mt, all_bin_function_mt, op_sym, all_bin_relation_mt,
			bin_relation_op_sym,all_unary_function_mt, unary_function_op_sym);
	int domain_size = all_mt[0][0].size();
	invariants.resize(domain_size, 0); // TODO: check, perhaps domain_size is needed.
}

ValueNode* Tree::build_value_node()
{
	ValueNode* v_node = &all_value_nodes[avail_value_node_pos++];
	int label_to_use = rand()%num_labels;
	label_in_use[label_to_use] = 1;
	v_node->set_value(label_to_use);
	return v_node;
}

OpNode* Tree::build_op_node(std::vector<std::vector<int>>& mt, std::string& op_sym)
{
	OpNode* op_node = &all_op_nodes[avail_op_node_pos++];
	op_node->set_op(&mt);
	op_node->set_sym(op_sym);
	return op_node;
}

UnaryOpNode* Tree::build_unary_op_node(std::vector<std::vector<int>>& mt, std::string& op_sym)
{
	UnaryOpNode* op_node = &all_unary_op_nodes[avail_unary_op_node_pos++];
	op_node->set_op(&(mt[0]));
	op_node->set_sym(op_sym);
	return op_node;
}


TreeNode* Tree::build_node(unsigned int level, std::vector<std::vector<std::vector<int>>>& all_mt,
		std::vector<int>& all_bin_function_mt, std::vector<std::string>& op_sym,
		std::vector<int>& all_unary_function_mt, std::vector<std::string>& unary_function_op_sym, bool top_is_op)
{
	bool is_value_node = !top_is_op && (level == depth || rand() % 2 == 0);
	if (is_value_node) {
		return build_value_node();
	}
	else {
		bool unary = false;
		if (all_unary_function_mt.size() > 0) {
			unary = rand() % 4 == 0;
		}
		if (unary) {
			int which_op = rand() % unary_function_op_sym.size();
			UnaryOpNode* node = build_unary_op_node(all_mt[all_unary_function_mt[which_op]], unary_function_op_sym[which_op]);
			node->left = build_node(level+1, all_mt, all_bin_function_mt, op_sym, all_unary_function_mt, unary_function_op_sym);
			return node;
		}
		else {
			int which_op = rand() % op_sym.size();
			OpNode* node = build_op_node(all_mt[all_bin_function_mt[which_op]], op_sym[which_op]);
			node->left = build_node(level+1, all_mt, all_bin_function_mt, op_sym, all_unary_function_mt, unary_function_op_sym);
			node->right = build_node(level+1, all_mt, all_bin_function_mt, op_sym, all_unary_function_mt, unary_function_op_sym);
			return node;
		}
	}
}

void Tree::build_tree_structure(std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt,
		std::vector<std::string>& op_sym, std::vector<int>& all_bin_relation_mt, std::vector<std::string>& bin_relation_op_sym,
		std::vector<int>& all_unary_function_mt, std::vector<std::string>& unary_function_op_sym)
{
	label_in_use = std::vector<int>(num_labels, 0);
	left = build_node(1, all_mt, all_bin_function_mt, op_sym, all_unary_function_mt, unary_function_op_sym, true);
	right = build_node(1, all_mt, all_bin_function_mt, op_sym, all_unary_function_mt, unary_function_op_sym);
	if (all_bin_relation_mt.size() > 0) {
		if (rand() % 2 == 0) {
			int x = rand() % all_bin_relation_mt.size();
			sym = bin_relation_op_sym[x];
			op = &all_mt[all_bin_relation_mt[x]];
		}
	}
	/*  debug print
	left->to_string(std::cerr, false);
	std::cerr << " " << sym << " ";
	right->to_string(std::cerr, false);
	std::cerr << std::endl;
	*/
}

int Tree::find_next_1(std::vector<int> vec, unsigned int start_pos)
{
	for(unsigned int idx=start_pos; idx<vec.size(); idx++) {
		if (vec[idx] > 0)
			return idx;
	}
	return -1;
}

void Tree::calc_invariant_vector(unsigned int domain_size)
{
	// TODO:: check that domain_size >= num_labels
	std::vector<int> values(domain_size, 0);
	int num_label_in_use = std::accumulate(label_in_use.begin(), label_in_use.end(), decltype(label_in_use)::value_type(0));
	std::fill(invariants.begin(), invariants.end(), 0);
	int pos0 = find_next_1(label_in_use, 0);
	if (num_label_in_use == 1) {
		for(unsigned int el = 0; el < domain_size; el++) {
			values[pos0] = el;
			int left_value = left->eval_node(values);
			int right_value = right->eval_node(values);
			if ((op != nullptr && (*op)[left_value][right_value]) ||
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
				if ((op != nullptr && (*op)[left_value][right_value]) ||
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
					if ((op != nullptr && (*op)[left_value][right_value]) ||
							(op == nullptr && left_value == right_value)) {
						invariants[el] +=1;
					}
				}
			}
		}
	}
	//print_invariants(domain_size);
}

int ValueNode::eval_node(std::vector<int>& labels)
{
	return labels[value];
}

int OpNode::eval_node(std::vector<int>& values)
{
	int left_value  = left->eval_node(values);
	int right_value = right->eval_node(values);
	return (*op)[left_value][right_value];
}

int UnaryOpNode::eval_node(std::vector<int>& values)
{
	int left_value  = left->eval_node(values);
	return (*op)[left_value];
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

