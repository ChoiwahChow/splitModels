/*
 * TreeNode.h
 *
 *  Created on: Jun 13, 2021
 *      Author: uabcoimbra
 */

#ifndef TREENODE_H_
#define TREENODE_H_

#include <iostream>
#include <ostream>
#include <time.h>
#include <vector>

enum NodeType { value = 0, op = 1 };

class TreeNode {
protected:
	TreeNode* left;
	TreeNode* right;
public:
	TreeNode(): left(nullptr), right(nullptr) {};
	virtual ~TreeNode();

	void set_left(TreeNode* left) {this->left = left;};
	void set_right(TreeNode* right) {this->right = right;};

	TreeNode* get_left() const {return left;};
	TreeNode* get_right() const {return right;};

	virtual void to_string(std::ostream& out, bool paran = true) const;
	virtual NodeType getNodeType() = 0;
	virtual int eval_node(std::vector<int> labels) = 0;

	friend class Tree;
};

class ValueNode: public TreeNode {
private:
	int  value;
public:
	static constexpr const char labels[] = {'x', 'y', 'z', 'u', 'v', 'w'};
	ValueNode(): value(0) {};
	virtual ~ValueNode();

	virtual NodeType getNodeType() { return NodeType::value;}
	virtual void to_string(std::ostream& out, bool paran = true) const;
	int  get_value() {return value;};
	void set_value(int value) {this->value = value;};
	virtual int eval_node(std::vector<int> labels) {return labels[value];};
};


class OpNode: public TreeNode {
private:
	int**         op;
	std::string   sym;
public:
	OpNode(): op(nullptr), sym("*") {};
	virtual ~OpNode();

	virtual NodeType getNodeType() { return NodeType::op;}
	virtual void to_string(std::ostream& out, bool paran = true) const;
	int** get_op() {return op;};
	void  set_op(int** op) {this->op = op;};
	void  set_sym(std::string& sym) {this->sym = sym;};
	virtual int eval_node(std::vector<int> labels);
};


// TODO: change to contain an OpNode
// Allocated space for order 64 only
class Tree {
private:
	static const int default_max_depth = 4;   // starts from 1
	static const int default_num_labels = 2;
	static const int max_domain_size = 64;
	int**         op;
	std::string   sym;
	std::vector<OpNode>    all_op_nodes;
	std::vector<ValueNode> all_value_nodes;
	std::vector<int>       label_in_use;
	TreeNode*  left;
	TreeNode*  right;
	unsigned int depth;
	unsigned int num_labels;
	unsigned int op_node_size;
	unsigned int value_node_size;
	unsigned int avail_op_node_pos;
	unsigned int avail_value_node_pos;

	int invariants[max_domain_size];

	static int find_next_1(std::vector<int> vec, unsigned int start_pos);

public:
	Tree(): op(nullptr), sym("="), left(nullptr), right(nullptr), depth(default_max_depth), num_labels(default_num_labels),
			op_node_size(20), value_node_size(50), avail_op_node_pos(0), avail_value_node_pos(0) {};

	Tree(unsigned int depth, unsigned int num_labels);
	virtual ~Tree();


	void clear_memory();
	ValueNode* build_value_node();
	OpNode*    build_op_node(int** mt, std::string& op_sym);
	void build_node(TreeNode** node, unsigned int level, std::vector<int**>& all_mt, std::vector<std::string>& op_sym, bool top_is_op = false);
	void build_tree_structure(std::vector<int**>& all_mt, std::vector<std::string>& op_sym,
			std::vector<int**>& all_bin_relation_mt, std::vector<std::string>& bin_relation_op_sym);
	void initialize(std::vector<int**>& all_mt, std::vector<std::string>& op_sym,
			std::vector<int**>& all_bin_relation_mt, std::vector<std::string>& bin_relation_op_sym, unsigned int seed);
	void calc_invariant_vector(unsigned int domain_size);
	int* get_invariants() {return invariants;};
	void print_invariants(unsigned int domain_size) const;
};

#endif /* TREENODE_H_ */
