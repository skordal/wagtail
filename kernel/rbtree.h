// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_RB_TREE_H
#define WAGTAIL_RB_TREE_H

namespace wagtail
{
	// Red-Black tree structure with key type I and node content type T, implemented
	// using algorithms and guidance from "Introduction to Algorithms" by Cormen, et al.
	// Turns out, it is nigh impossible to get a different perspective on RB trees on
	// the Internet, as everyone else also seems to use this book to learn about RB trees.
	// TODO: Write a method for deleting items.
	template<typename K, typename T> class rbtree final
	{
		public:
			~rbtree() {}

			// Inserts a value into the tree:
			void insert(const K & key, const T & data)
			{
				node * current = root;
				node * parent = nullptr;

				while(current != nullptr)
				{
					parent = current;
					if(key < current->key)
						current = current->left;
					else
						current = current->right;
				}

				node * new_node = new node(key, data, parent);

				if(parent == nullptr)
					root = new_node;
				else if(key < parent->key)
					parent->left = new_node;
				else
					parent->right = new_node;

				insertion_fixup(new_node);
			}

			// Gets the value of a node. Returns false if a value corresponding to the
			// specified key was not found:
			bool get_value(const K & key, T & value_out) const
			{
				node * n = find_node(key);
				if(n == nullptr)
					return false;
				
				value_out = n->content;
				return true;
			}

			// Checks if a key exists in the tree:
			bool key_exists(const K & key) const
			{
				return find_node(key) != nullptr;
			}

		private:
			// Structure for tree nodes.
			// TODO: possibly make this a class with accessors and private fields.
			struct node
			{
				node(const K & key, const T & content, node * parent)
					: key(key), content(content), parent(parent) {}

				K key;
				T content;

				enum { RED, BLACK} color = RED;

				node * left = nullptr, * right = nullptr;
				node * parent;
			};

			// Finds the node with the specified key, or nullptr if the node does not
			// exist:
			node * find_node(const K & key) const
			{
				node * current = root;
				while(current != nullptr)
				{
					if(key == current->key)
						return current;
					else if(key < current->key)
						current = current->left;
					else
						current = current->right;
				}

				return nullptr;
			}

			// Fixes the tree up after insertion. The parameter n is the newly inserted node.
			void insertion_fixup(node * n)
			{
				node * y;

				if(n->parent != nullptr && n->parent->color == node::BLACK)
					return;

				while(n != root && n->parent->parent != nullptr && n->parent->color == node::RED)
				{
					if(n->parent == n->parent->parent->left)
					{
						y = n->parent->parent->right;

						if(y->color == node::RED)
						{
							n->parent->color = node::BLACK;
							if(y != nullptr)
								y->color = node::BLACK;
							if(y != nullptr && y->parent != nullptr && y->parent->parent != nullptr)
								y->parent->parent->color = node::RED;
							n = n->parent->parent;
						} else {
							if(n == n->parent->right)
							{
								n = n->parent;
								left_rotate(n);
							}

							n->parent->color = node::BLACK;
							n->parent->parent->color = node::RED;
							right_rotate(n->parent->parent);
						}
					} else {
						y = n->parent->parent->left;

						if(y != nullptr && y->color == node::RED)
						{
							n->parent->color = node::BLACK;
							if(y != nullptr)
								y->color = node::BLACK;
							if(y != nullptr && y->parent != nullptr && y->parent->parent != nullptr)
								y->parent->parent->color = node::RED;
							n = n->parent->parent;
						} else {
							if(n == n->parent->left)
							{
								n = n->parent;
								right_rotate(n);
							}

							n->parent->color = node::BLACK;
							n->parent->parent->color = node::RED;
							left_rotate(n->parent->parent);
						}
					}
				}

				root->color = node::BLACK;
			}

			// Left rotates the specified node:
			void left_rotate(node * x)
			{
				node * y = x->right;
				x->right = y->left;

				if(y->left != nullptr)
					y->left->parent = x;
				
				y->parent = x->parent;

				if(x->parent == nullptr)
					root = y;
				else if(x == x->parent->left)
					x->parent->left = y;
				else
					x->parent->right = y;

				y->left = x;
				x->parent = y;
			}

			// Right rotates the specified node:
			void right_rotate(node * x)
			{
				node * y = x->left;
				x->left = y->right;

				if(y->right != nullptr)
					y->right->parent = x;
				
				y->parent = x->parent;

				if(x->parent == nullptr)
					root = y;
				else if(x == x->parent->left)
					x->parent->left = y;
				else
					x->parent->right = y;

				y->right = x;
				x->parent = y;
			}

			node * root = nullptr;
	};
}

#endif

