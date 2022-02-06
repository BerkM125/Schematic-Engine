#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include "schematic_engine.h"
#include "schem.h"
using namespace std;

//Level-order traversal utility, mainly used for debugging
void componentTreeHierarchy::traverse(void) {
	componentTreeHierarchy* curr = this;
	queue<componentTreeHierarchy*> nodequeue;
	nodequeue.push(curr);
	while (!nodequeue.empty()) {
		int sz = nodequeue.size();
		for (int n = 0; n < sz; n++) {
			componentTreeHierarchy* c = nodequeue.front();
			cout << c->val << " ";
			nodequeue.pop();

			if (c->left) nodequeue.push(c->left);
			if (c->right) nodequeue.push(c->right);
		}
		cout << endl;
	}
}

//Recursive utility for DFS traversal of the hierarchy
void componentTreeHierarchy::searchHierarchyUtil(componentTreeHierarchy* node, int target, vector<int>& holder) {
	if (!node) return;
	if (node->val == target) {
		holder = node->positionval;
		return;
	}
	if (node->val > target) searchHierarchyUtil(node->left, target, holder);
	if (node->val < target) searchHierarchyUtil(node->right, target, holder);
	return;
}

//Searches through the hierarchy for a matching coordinate value, returns a number corresponding to
//the index(es) of component(s)
vector<int> componentTreeHierarchy::searchHierarchy(int target) {
	vector<int> holderval;
	searchHierarchyUtil(this, target, holderval);
	return holderval;
}

void componentTreeHierarchy::insertComponent(int target, int data) {
	componentTreeHierarchy* node = this;
	while (node->left || node->right) {
		if (node->val == target) {
			if (positionset.find(data) == positionset.end()) {
				node->positionval.push_back(data);
				positionset.insert(data);
			}
			return;
		}
		if (node->val > target) {
			if (node->left) node = node->left;
			else break;
		}
		else {
			if (node->right) node = node->right;
			else break;
		}
	}
	if (node->val > target) {
		node->left = new componentTreeHierarchy;
		node->left->positionval.push_back(data);
		positionset.insert(data);
		node->left->val = target;
	}
	else {
		node->right = new componentTreeHierarchy;
		node->right->positionval.push_back(data);
		positionset.insert(data);
		node->right->val = target;
	}
	return;
}