#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu
{

	template<class Key, class T, class Compare = std::less<Key>>
	class map
	{
	public:
		typedef pair<const Key, T> value_type;
		enum Color { RED, BLACK };

	private:
		struct Node
		{
			Node *left, *right, *father;
			Color color;
			value_type kvpair;
			Node(const value_type &kv, Color c = RED) : left(nullptr), right(nullptr), father(nullptr), color(c), kvpair(kv) {}
		};
	private:
		Node *root;
		size_t __size;

	private:
		void __clear(Node *t)
		{
			if (t == nullptr) return;
			__clear(t->left);
			__clear(t->right);
			delete t;
		}

		Node* __dfs(Node *other, Node *p = nullptr)
		{
			if (other == nullptr) return nullptr;
			Node *t = new Node(other->kvpair, other->color);
			t->father = p;
			t->left = __dfs(other->left, t);
			t->right = __dfs(other->right, t);
			return t;
		}

	public:
		map() : root(nullptr), __size(0) {}
		map(const map &other)
		{
			root = __dfs(other.root);
			__size = other.__size;
		}

		map &operator=(const map &other)
		{
			if (this == &other) return *this;
			__clear(root);
			root = __dfs(other.root);
			__size = other.__size;
			return *this;
		}

		~map()
		{
			__clear(root);
		}

	public:
		class const_iterator;
		class iterator
		{
		public:
			Node *cur;
			map *corres;

		public:
			iterator() = default;
			iterator(const iterator &other) = default;
			iterator(Node *_cur, map *_corres) : cur(_cur), corres(_corres) {}

		public:
			bool checkValid(map *curMap) const { return cur != nullptr && corres == curMap; }

			iterator& operator++()
			{
				if (cur == nullptr) throw invalid_iterator();
				if (cur->right != nullptr)
				{
					cur = cur->right;
					while (cur->left != nullptr) cur = cur->left;
				}
				else
				{
					while (cur != nullptr && cur->father != nullptr && !isLeftSon(cur)) cur = cur->father;
					cur = cur->father;
				}
				return *this;
			}

			iterator operator++(int)
			{
				iterator t = *this;
				++(*this);
				return t;
			}

			iterator & operator--()
			{
				if (cur == nullptr) //end()
				{
					if (corres->empty()) throw invalid_iterator();
					cur = corres->root;
					while (cur->right != nullptr) cur = cur->right;
				}
				else if (cur->left != nullptr)
				{
					cur = cur->left;
					while (cur->right != nullptr) cur = cur->right;
				}
				else
				{
					while (cur != nullptr && cur->father != nullptr && isLeftSon(cur)) cur = cur->father;
					cur = cur->father;
				}
				return *this;
			}

			iterator operator--(int)
			{
				iterator t = *this;
				--(*this);
				return t;
			}

			value_type & operator*() const { return cur->kvpair; }
			value_type* operator->() const noexcept { return &(cur->kvpair); }

			bool operator==(const iterator &rhs) const { return cur == rhs.cur && corres == rhs.corres; }
			bool operator==(const const_iterator &rhs) const { return cur == rhs.cur && corres == rhs.corres; }
			bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
			bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
		};

		class const_iterator
		{
		public:
			Node *cur;
			const map *corres;

		public:
			const_iterator() = default;
			const_iterator(const iterator &other) : cur(other.cur), corres(other.corres) {}
			const_iterator(const const_iterator &other) = default;
			const_iterator(Node *_cur, const map *_corres) : cur(_cur), corres(_corres) {}

		public:
			const_iterator& operator++()
			{
				if (cur == nullptr) throw invalid_iterator();
				if (cur->right != nullptr)
				{
					cur = cur->right;
					while (cur->left != nullptr) cur = cur->left;
				}
				else
				{
					while (cur != nullptr && cur->father != nullptr && !isLeftSon(cur)) cur = cur->father;
					cur = cur->father;
				}
				return *this;
			}

			const_iterator operator++(int)
			{
				const_iterator t = *this;
				++(*this);
				return t;
			}

			const_iterator & operator--()
			{
				if (cur == nullptr) //end()
				{
					if (corres->empty()) throw invalid_iterator();
					cur = corres->root;
					while (cur->right != nullptr) cur = cur->right;
				}
				else if (cur->left != nullptr)
				{
					cur = cur->left;
					while (cur->right != nullptr) cur = cur->right;
				}
				else
				{
					while (cur != nullptr && cur->father != nullptr && isLeftSon(cur)) cur = cur->father;
					cur = cur->father;
				}
				return *this;
			}

			const_iterator operator--(int)
			{
				const_iterator t = *this;
				--(*this);
				return t;
			}

			const value_type & operator*() const { return cur->kvpair; }
			const value_type* operator->() const noexcept { return &(cur->kvpair); }

			bool operator==(const const_iterator &rhs) const { return cur == rhs.cur && corres == rhs.corres; }
			bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
		};

	private:
		static bool isLeftSon(Node *t)
		{
			if (t == nullptr) return true; //For function eraseFixup()
			return t->father != nullptr && t->father->left == t;
		}

		Color getColor(Node *t)
		{
			if (t == nullptr || t->color == BLACK) return BLACK;
			else return RED;
		}

	private:
		void leftRotate(Node *x)
		{
			Node *y = x->right;
			x->right = y->left;
			if (y->left != nullptr) y->left->father = x;
			y->father = x->father;
			if (x->father == nullptr) root = y;
			else if (isLeftSon(x)) x->father->left = y;
			else x->father->right = y;
			y->left = x;
			x->father = y;
		}

		void rightRotate(Node *x)
		{
			Node *y = x->left;
			x->left = y->right;
			if (y->right != nullptr) y->right->father = x;
			y->father = x->father;
			if (x->father == nullptr) root = y;
			else if (isLeftSon(x)) x->father->left = y;
			else x->father->right = y;
			y->right = x;
			x->father = y;
		}

	private:
		void insertFixup(Node *z)
		{
			while (getColor(z->father) == RED)
			{
				if (isLeftSon(z->father))
				{
					Node *y = z->father->father->right;
					if (getColor(y) == RED) //uncle is red
					{
						z->father->color = BLACK;
						y->color = BLACK;
						z->father->father->color = RED;
						z = z->father->father;
					}
					else if (isLeftSon(z)) //uncle is black and z is left son
					{
						z->father->color = BLACK;
						z->father->father->color = RED;
						rightRotate(z->father->father);
					}
					else //uncle is black and z is right son
					{
						z = z->father;
						leftRotate(z);
					}
				}
				else //reflection of the other case
				{
					Node *y = z->father->father->left;
					if (getColor(y) == RED)
					{
						z->father->color = BLACK;
						y->color = BLACK;
						z->father->father->color = RED;
						z = z->father->father;
					}
					else if (!isLeftSon(z))
					{
						z->father->color = BLACK;
						z->father->father->color = RED;
						leftRotate(z->father->father);
					}
					else
					{
						z = z->father;
						rightRotate(z);
					}
				}
			}
			root->color = BLACK;
		}

		void eraseFixup(Node *x, Node *parent)
		{
			Node *w;
			while (x != root && getColor(x) == BLACK)
			{
				if (isLeftSon(x))
				{
					w = parent->right;
					if (getColor(w) == RED)
					{
						w->color = BLACK;
						parent->color = RED;
						leftRotate(parent);
						w = parent->right;
					}
					else if (w == nullptr || (getColor(w->left) == BLACK && getColor(w->right) == BLACK))
					{
						if (w) w->color = RED;
						x = parent;
						parent = parent->father;
					}
					else if (getColor(w->left) == RED && getColor(w->right) == BLACK)
					{
						w->left->color = BLACK;
						w->color = RED;
						rightRotate(w);
						w = parent->right;
					}
					else if (getColor(w->right) == RED)
					{
						w->color = parent->color;
						parent->color = BLACK;
						w->right->color = BLACK;
						leftRotate(parent);
						x = root;
						break;
					}
				}
				else
				{
					w = parent->left;
					if (getColor(w) == RED)
					{
						w->color = BLACK;
						parent->color = RED;
						rightRotate(parent);
						w = parent->left;
					}
					else if (w == nullptr || (getColor(w->left) == BLACK && getColor(w->right) == BLACK))
					{
						if (w != nullptr) w->color = RED;
						x = parent;
						parent = parent->father;
					}
					else if (getColor(w->right) == RED && getColor(w->left) == BLACK)
					{
						w->right->color = BLACK;
						w->color = RED;
						leftRotate(w);
						w = parent->left;
					}
					else if (getColor(w->left) == RED)
					{
						w->color = parent->color;
						parent->color = BLACK;
						w->left->color = BLACK;
						rightRotate(parent);
						x = root;
						break;
					}
				}
			}
			if (x != nullptr) x->color = BLACK;
		}

	private:
		iterator __insert(const value_type &value)
		{
			__size++;
			Node *z = new Node(value, RED);
			Node *x = root, *y = nullptr;
			while (x != nullptr)
			{
				y = x;
				if (Compare()(value.first, x->kvpair.first)) x = x->left;
				else x = x->right;
			}
			z->father = y;
			if (y == nullptr) root = z;
			else if (Compare()(value.first, y->kvpair.first)) y->left = z;
			else y->right = z;
			insertFixup(z);
			return iterator(z, this);
		}

		void __erase(iterator pos)
		{
			__size--;
			Node *z = pos.cur, *x, *y;
			if (z->right == nullptr || z->left == nullptr) y = z;
			else
			{
				y = z->right;
				while (y->left != nullptr) y = y->left;
			}

			if (y->left != nullptr) x = y->left;
			else x = y->right;
			if (x != nullptr) x->father = y->father;

			if (y->father == nullptr) root = x; //y is the root
			else if (isLeftSon(y)) y->father->left = x;
			else y->father->right = x;

			if (y != z) //the case where y is the successor of z
			{
				z->kvpair.~pair();
				new(&z->kvpair) value_type(y->kvpair);
			}
			if (y->color == BLACK)
			{
				eraseFixup(x, y->father);
			}
			delete y;
		}

	public:
		bool empty() const { return __size == 0; }
		size_t size() const { return __size; }
		void clear()
		{
			__clear(root);
			root = nullptr;
			__size = 0;
		}

	public:
		iterator begin()
		{
			Node *leftMost = root;
			while (leftMost != nullptr && leftMost->left != nullptr) leftMost = leftMost->left;
			return iterator(leftMost, this);
		}
		const_iterator cbegin() const
		{
			Node *leftMost = root;
			while (leftMost != nullptr && leftMost->left != nullptr) leftMost = leftMost->left;
			return const_iterator(leftMost, this);
		}

		iterator end() { return iterator(nullptr, this); }
		const_iterator cend() const { return const_iterator(nullptr, this); }

	public:
		iterator find(const Key &key)
		{
			Node *t = root;
			while (t != nullptr)
			{
				if (!(Compare()(t->kvpair.first, key) || Compare()(key, t->kvpair.first)))
				{
					return iterator(t, this);
				}
				if (Compare()(key, t->kvpair.first)) t = t->left;
				else t = t->right;
			}
			return end();
		}
		const_iterator find(const Key &key) const
		{
			Node *t = root;
			while (t != nullptr)
			{
				if (!(Compare()(t->kvpair.first, key) || Compare()(key, t->kvpair.first)))
				{
					return const_iterator(t, this);
				}
				if (Compare()(key, t->kvpair.first)) t = t->left;
				else t = t->right;
			}
			return cend();
		}

		T& at(const Key &key)
		{
			iterator iter = find(key);
			if (iter == end()) throw index_out_of_bound();
			else return iter->second;
		}
		const T& at(const Key &key) const
		{
			const_iterator iter = find(key);
			if (iter == cend()) throw index_out_of_bound();
			else return iter->second;
		}

		T& operator[](const Key &key)
		{
			iterator iter = find(key);
			if (iter == end()) return insert(value_type(key, T())).first.cur->kvpair.second;
			else return iter->second;
		}
		const T & operator[](const Key &key) const { return at(key); }

		pair<iterator, bool> insert(const value_type &value)
		{
			iterator t = find(value.first);
			if (t != end()) return pair<iterator, bool>(t, false);
			return pair<iterator, bool>(__insert(value), true);
		}

		void erase(iterator pos)
		{
			if (!pos.checkValid(this)) throw invalid_iterator();
			__erase(pos);
		}

		size_t count(const Key &key) const { return find(key) != cend(); }
	};

}

#endif
