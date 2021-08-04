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

enum NodeType { value = 0, op = 1, unary_op = 2 };

class TreeNode {
protected:
	TreeNode* left;
	TreeNode* right;
public:
	TreeNode(): left(nullptr), right(nullptr) {};
	virtual ~TreeNode();

	virtual void to_string(std::ostream& out, bool paran = true) const;
	virtual NodeType getNodeType() = 0;
	virtual int eval_node(std::vector<int>& labels) = 0;

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
	virtual int eval_node(std::vector<int>& labels);
};


class OpNode: public TreeNode { // handles both unary and binary operations
private:
	std::vector<std::vector<int>>*    op;
public:
	std::string        sym;
	OpNode(): op(nullptr), sym("*") {};
	virtual ~OpNode();

	virtual NodeType getNodeType() { return NodeType::op;}
	virtual void to_string(std::ostream& out, bool paran = true) const;
	std::vector<std::vector<int>>* get_op() {return op;};
	void  set_op(std::vector<std::vector<int>>* op) {this->op = op;};
	void  set_sym(std::string& sym) {this->sym = sym;};
	virtual int eval_node(std::vector<int>& labels);
};


class UnaryOpNode: public TreeNode {
private:
	std::vector<int>*  op;
	std::string        sym;
public:
	UnaryOpNode(): op(nullptr), sym("'") {};
	virtual ~UnaryOpNode();

	virtual NodeType getNodeType() { return NodeType::unary_op;}
	virtual void to_string(std::ostream& out, bool paran = true) const;
	std::vector<int>* get_op() {return op;};
	void  set_op(std::vector<int>* op) {this->op = op;};
	void  set_sym(std::string& sym) {this->sym = sym;};
	virtual int eval_node(std::vector<int>& labels);
};


// TODO: change to contain an OpNode
// Allocated space for order 64 only
class Tree {
private:
	static const int default_max_depth = 4;   // starts from 0 for the root
	static const int default_num_labels = 2;
	std::vector<std::vector<int>>*         op;
	std::string   sym;
	std::vector<OpNode>         all_op_nodes;
	std::vector<UnaryOpNode>    all_unary_op_nodes;
	std::vector<ValueNode>      all_value_nodes;
	std::vector<int>            label_in_use;
	TreeNode*  left;
	TreeNode*  right;
	unsigned int depth;
	unsigned int num_labels;
	size_t op_node_size;
	size_t unary_op_nodes_size;
	size_t value_node_size;
	size_t avail_op_node_pos;
	size_t avail_value_node_pos;
	size_t avail_unary_op_node_pos;

	std::vector<int> invariants;

	static int find_next_1(std::vector<int> vec, unsigned int start_pos);

	ValueNode* build_value_node();
	OpNode*    build_op_node(std::vector<std::vector<int>>& mt, std::string& op_sym);
	UnaryOpNode* build_unary_op_node(std::vector<std::vector<int>>& mt, std::string& op_sym);
	TreeNode* build_node(unsigned int level, std::vector<std::vector<std::vector<int>>>& all_mt,
			std::vector<int>& all_bin_function_mt, std::vector<std::string>& op_sym, std::vector<int>& all_unary_function_mt,
			std::vector<std::string>& unary_function_op_sym, bool top_is_op = false);
	void build_tree_structure(std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt,
			std::vector<std::string>& op_sym, std::vector<int>& all_bin_relation_mt, std::vector<std::string>& bin_relation_op_sym,
			std::vector<int>& all_unary_function_mt, std::vector<std::string>& unary_function_op_sym);

public:
	Tree(): op(nullptr), sym("="), left(nullptr), right(nullptr), depth(default_max_depth), num_labels(default_num_labels),
			op_node_size(14), unary_op_nodes_size(16), value_node_size(16), avail_op_node_pos(0), avail_value_node_pos(0),
			avail_unary_op_node_pos(0) {};

	Tree(unsigned int depth, unsigned int num_labels);
	virtual ~Tree();
	void initialize(std::vector<std::vector<std::vector<int>>>& all_mt, std::vector<int>& all_bin_function_mt, std::vector<std::string>& op_sym,
			std::vector<int>& all_bin_relation_mt, std::vector<std::string>& bin_relation_op_sym, std::vector<int>& all_unary_function_mt,
			std::vector<std::string>& unary_function_op_sym, unsigned int seed);
	void calc_invariant_vector(unsigned int domain_size);
	std::vector<int> get_invariants() {return invariants;};
	void print_invariants(unsigned int domain_size) const;
};

#endif /* TREENODE_H_ */
