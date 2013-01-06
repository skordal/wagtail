// Wagtail OS
// (c) Kristian Klomsten Skordal 2012 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/wagtail>

#ifndef WAGTAIL_RB_TREE_H
#define WAGTAIL_RB_TREE_H

namespace wagtail
{
	/**
	 * Red-black tree class. This class is implemented using algorithms and guidance from
	 * "Introduction to Algorithms" by Cormen, et al. Turns out, it is nigh impossible to
	 * get a different perspective on such trees, as Cormen seems to be everyone else's 
	 * source on such trees as well.
	 *
	 * @tparam K key type.
	 * @tparam T data value type.
	 * @todo Write a method for deleting values from the tree.
	 */
	template<typename K, typename T> class rbtree final
	{
		public:
			/**
			 * Destructs a red-black tree object.
			 * @todo Remove nodes left in the tree.
			 */
			~rbtree() {}

			/**
			 * Inserts a node into the tree.
			 * @param key the key to insert the value with.
			 * @param data the value to insert into the tree.
			 */
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

			/**
			 * Gets the value of a node.
			 * @param key the key to retrieve the value for.
			 * @param value_out a reference to a variable to store the retrieved value in.
			 * @return whether the key was found in the tree.
			 */
			bool get_value(const K & key, T & value_out) const
			{
				node * n = find_node(key);
				if(n == nullptr)
					return false;
				
				value_out = n->content;
				return true;
			}

			/**
			 * Checks if a key exists in the tree.
			 * @param key the key to look for.
			 * @return `true` if the key was found, `false` otherwise.
			 */
			bool key_exists(const K & key) const
			{
				return find_node(key) != nullptr;
			}

		private:
			/**
			 * Structure representing a tree node.
			 * @todo Make this a class using private fields and accessors.
			 */
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

			/**
			 * Finds the node with the specified key.
			 * @param key the key to look for.
			 * @return the node, if found, `nullptr` otherwise.
			 */
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

			/**
			 * Fixes the tree up after insertion.
			 * @param n the newly inserted node.
			 */
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

			/**
			 * Left rotates the tree at the specified node.
			 * @param x the node to rotate around.
			 */
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

			/**
			 * Right rotates the tree at the specified node.
			 * @param x the node to rotate around.
			 */
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

