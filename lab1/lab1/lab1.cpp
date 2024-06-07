#include <iostream>
#include <queue>
#include <initializer_list>
#include <chrono>
#include <vector>
using namespace std;
using namespace std::chrono;


struct Node
{
	int key;
	Node* left;
	Node* right;

	Node() : key(0), left(nullptr), right(nullptr) {}
	Node(int key) : left(nullptr), right(nullptr)
	{
		this->key = key;
	}
};


class Set
{
	Node* _root;
	Node* copy_tree(Node* root)
	{
		if (root == nullptr)
		{
			return nullptr;
		}
		else
		{
			Node* new_node = new Node(root->key);
			new_node->left = copy_tree(root->left);
			new_node->right = copy_tree(root->right);
			return new_node;
		}
	}
	void _print(const Node* root)
	{
		if (!root) return;
		_print(root->left);
		std::cout << root->key << " ";
		_print(root->right);
	}
	bool _erase(Node*& node, const int key)
	{
		if (node == nullptr) {
			return false;
		}
		if (key < node->key) {
			return _erase(node->left, key);
		}
		else if (key > node->key) {
			return _erase(node->right, key);
		}
		else {
			if (node->left == nullptr) {
				Node* tmp = node->right;
				delete node;
				node = tmp;
				return true;
			}
			else if (node->right == nullptr) {
				Node* tmp = node->left;
				delete node;
				node = tmp;
				return true;
			}
			Node* min_right = node->right;
			while (min_right->left) {
				min_right = min_right->left;
			}

			node->key = min_right->key;
			return _erase(node->right, min_right->key);
		}
	}

	void clear(Node* root)
	{
		if (!root) return;
		clear(root->left);
		clear(root->right);
		delete root;
	}

public:
	Set() : _root(nullptr) {}

	Set(std::initializer_list<int> list) : _root(nullptr)
	{
		for (int x : list)
		{
			insert(x);
		}
	}
	Set(const Set& other) : _root(nullptr) // deep copy constr
	{
		_root = copy_tree(other.get_root());
	}

	~Set()
	{
		clear(_root);
		_root = nullptr;
	}

	void print()
	{
		if (!_root) return;
		_print(_root);
	}

	bool insert(int key)
	{
		if (!_root)
		{
			_root = new Node(key);
			return true;
		}

		Node* tmp = _root;
		while (tmp && tmp->key != key)
		{
			if (key < tmp->key)
			{
				if (tmp->left) tmp = tmp->left;
				else
				{
					tmp->left = new Node(key);
					return true;
				}
			}
			else
			{
				if (tmp->right) tmp = tmp->right;
				else
				{
					tmp->right = new Node(key);
					return true;
				}
			}
		}

		return false;
	}

	bool contains(int key) const
	{
		if (!_root) return false;
		Node* tmp = _root;
		while (tmp)
		{
			if (tmp->key == key) return true;
			if (key < tmp->key)
			{
				tmp = tmp->left;
			}
			else
			{
				tmp = tmp->right;
			}
		}

		return false;
	}

	bool erase(int key)
	{
		return _erase(_root, key);
	}

	Node* get_root() const
	{
		return this->_root;
	}

	Set& operator=(const Set& set) // deep copy
	{
		if (this != &set)
		{
			clear(_root);
			_root = copy_tree(set.get_root());
		}
		return *this;
	}
};


bool operator==(const Set& first, const Set& second)
{
	if (first.get_root() == nullptr && second.get_root() == nullptr) return true;

	std::queue<Node*> que; que.push(second.get_root());
	while (!que.empty())
	{
		Node* current = que.front();
		if (!first.contains(current->key)) return false;
		if (current->left) que.push(current->left);
		if (current->right) que.push(current->right);
		que.pop();
	}

	return true;
}


Set intersection(const Set& first, const Set& second)
{
	if (!first.get_root() || !second.get_root()) return Set();
	Set result;
	Set copy_first(first);

	std::queue<Node*> que; que.push(second.get_root());
	while (!que.empty())
	{
		Node* current = que.front();
		if (!copy_first.insert(current->key)) result.insert(current->key);
		if (current->left) que.push(current->left);
		if (current->right) que.push(current->right);
		que.pop();
	}

	return result;
}


Set difference(const Set& first, const Set& second)
{
	if (!first.get_root()) return Set();
	if (!second.get_root()) return Set(first);
	Set result;
	Set copy_second(second);

	std::queue<Node*> que; que.push(first.get_root());
	while (!que.empty())
	{
		Node* current = que.front();
		if (copy_second.insert(current->key)) result.insert(current->key);
		if (current->left) que.push(current->left);
		if (current->right) que.push(current->right);
		que.pop();
	}

	return result;
}


size_t lcg()
{
	static size_t x = 0;
	x = (1021 * x + 24631) % 116640;
	return x;
}



template<typename Func>
double measure_execution_time(Func&& func) {
	auto start = std::chrono::high_resolution_clock::now();
	func();
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed_time = std::chrono::duration<double, std::milli>(end - start).count();
	return elapsed_time;
}

int main() {
	const int num_trials_fill = 100;
	const int num_trials_search = 1000;
	const int sizes[] = { 1000, 10000, 100000 };

	for (int size : sizes) {
		double total_time_set_fill = 0.0;
		double total_time_set_search = 0.0;
		double total_time_set_insert = 0.0;
		double total_time_set_erase = 0.0;
		double total_time_vector_fill = 0.0;
		double total_time_vector_search = 0.0;
		double total_time_vector_insert = 0.0;
		double total_time_vector_erase = 0.0;

		for (int i = 0; i < num_trials_fill; ++i) {
			// Fill Set
			Set my_set;
			auto set_fill_time = measure_execution_time([&]() {
				for (int j = 0; j < size; ++j) {
					my_set.insert(lcg());
				}
				});
			total_time_set_fill += set_fill_time;

			// Fill vector
			std::vector<int> my_vector;
			auto vector_fill_time = measure_execution_time([&]() {
				for (int j = 0; j < size; ++j) {
					my_vector.push_back(lcg());
				}
				});
			total_time_vector_fill += vector_fill_time;
		}

		double average_time_set_fill = total_time_set_fill / num_trials_fill;
		double average_time_vector_fill = total_time_vector_fill / num_trials_fill;

		cout << "Average fill time for Set with size " << size << ": " << average_time_set_fill << " ms" << endl;
		cout << "Average fill time for vector with size " << size << ": " << average_time_vector_fill << " ms" << endl;

		// Search
		Set my_set;
		std::vector<int> my_vector;
		for (int j = 0; j < size; ++j) {
			int random_number = lcg();
			my_set.insert(random_number);
			my_vector.push_back(random_number);
		}

		for (int i = 0; i < num_trials_search; ++i) {
			int random_number = lcg();

			// Search in Set
			auto set_search_time = measure_execution_time([&]() {
				my_set.contains(random_number);
				});
			total_time_set_search += set_search_time;

			// Search in vector
			auto vector_search_time = measure_execution_time([&]() {
				std::find(my_vector.begin(), my_vector.end(), random_number);
				});
			total_time_vector_search += vector_search_time;
		}

		double average_time_set_search = total_time_set_search / num_trials_search;
		double average_time_vector_search = total_time_vector_search / num_trials_search;

		cout << "Average search time for Set with size " << size << ": " << average_time_set_search << " ms" << endl;
		cout << "Average search time for vector with size " << size << ": " << average_time_vector_search << " ms" << endl;

		// Insertion and deletion
		for (int i = 0; i < num_trials_search; ++i) {
			int random_number_insert = lcg();
			int random_number_erase = lcg();

			// Insertion in Set
			auto set_insert_time = measure_execution_time([&]() {
				my_set.insert(random_number_insert);
				});
			total_time_set_insert += set_insert_time;

			// Deletion in Set
			auto set_erase_time = measure_execution_time([&]() {
				my_set.erase(random_number_erase);
				});
			total_time_set_erase += set_erase_time;

			// Insertion in vector
			auto vector_insert_time = measure_execution_time([&]() {
				my_vector.push_back(random_number_insert);
				});
			total_time_vector_insert += vector_insert_time;

			// Deletion in vector
			auto vector_erase_time = measure_execution_time([&]() {
				my_vector.erase(std::remove(my_vector.begin(), my_vector.end(), random_number_erase), my_vector.end());
				});
			total_time_vector_erase += vector_erase_time;
		}

		double average_time_set_insert = total_time_set_insert / num_trials_search;
		double average_time_set_erase = total_time_set_erase / num_trials_search;
		double average_time_vector_insert = total_time_vector_insert / num_trials_search;
		double average_time_vector_erase = total_time_vector_erase / num_trials_search;

		cout << "Average insert time for Set with size " << size << ": " << average_time_set_insert << " ms" << endl;
		cout << "Average erase time for Set with size " << size << ": " << average_time_set_erase << " ms" << endl;
		cout << "Average insert time for vector with size " << size << ": " << average_time_vector_insert << " ms" << endl;
		cout << "Average erase time for vector with size " << size << ": " << average_time_vector_erase << " ms" << endl;
	}

	Set set1 = { 1, 2, 3, 4, 5 };
	Set set2 = { 4, 5, 6, 7, 8 };

	Set intersection_result = intersection(set1, set2);
	Set difference_result = difference(set1, set2);

	intersection_result.print();
	cout << endl;
	difference_result.print();
	
	cout << endl;
	Set set3 = { 1, 2, 3, 4 ,5 };
	set3.erase(1);
	set3.print();

	return 0;
}