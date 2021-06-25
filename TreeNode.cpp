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

TreeNode::~TreeNode() {
	// TODO Auto-generated destructor stub
}

void TreeNode::to_string(std::ostream& out)
{
	left->to_string(out);
	right->to_string(out);
}

ValueNode::~ValueNode() {
	// TODO Auto-generated destructor stub
}

OpNode::~OpNode() {

}

void OpNode::to_string(std::ostream& out)
{
	out << "[op, left: ";
	left->to_string(out);
	out << ", right: ";
	right->to_string(out);
	out << "]";
}

void Tree::clear_memory()
{
	this->avail_op_node_pos = 0;
	this->avail_value_node_pos = 0;
}

Tree::Tree(unsigned int op_node_size, unsigned int value_node_size, int** mt, unsigned int num_labels):
		left(nullptr), right(nullptr), depth(default_max_depth), num_labels(num_labels), op_node_size(op_node_size),
		value_node_size(value_node_size), avail_op_node_pos(0), avail_value_node_pos(0), mt(mt)
{
}

Tree::Tree(int** mt, unsigned int depth, unsigned int num_labels):
		left(nullptr), right(nullptr), depth(depth), num_labels(num_labels),
		op_node_size(int(pow(2.0, depth))), value_node_size(op_node_size*2),
		avail_op_node_pos(0), avail_value_node_pos(0), mt(mt)
{
}

void Tree::initialize(unsigned int seed)
{
	all_op_nodes = std::vector<OpNode>(op_node_size);
	all_value_nodes = std::vector<ValueNode>(value_node_size);
	srand(seed);
	build_tree_structure();
}

ValueNode* Tree::build_value_node()
{
	ValueNode* v_node = &all_value_nodes[avail_value_node_pos++];
	int label_to_use = rand()%num_labels;
	label_in_use[label_to_use] = 1;
	v_node->set_value(label_to_use);
	return v_node;
}

OpNode* Tree::build_op_node()
{
	OpNode* v_node = &all_op_nodes[avail_op_node_pos++];
	v_node->set_op(mt);
	return v_node;
}

void Tree::build_node(TreeNode** node, unsigned int level, bool top_is_op)
{
	bool is_value_node = !top_is_op && (level == depth || rand() % 2 == 0);
	if (is_value_node) {
		*node = build_value_node();
	}
	else {
		*node = build_op_node();
		build_node(&(*node)->left, level+1);
		build_node(&(*node)->right, level+1);
	}
}

void Tree::build_tree_structure()
{
	label_in_use = std::vector<int>(num_labels, 0);
	build_node(&left, 1, true);
	build_node(&right, 1);
	/*
	left->to_string(std::cerr);
	std::cerr << " | ";
	right->to_string(std::cerr);
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
			if (left_value == right_value) {
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
				if (left_value == right_value) {
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
					if (left_value == right_value) {
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

