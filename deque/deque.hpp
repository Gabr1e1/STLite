#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <iostream>
#include <cstddef>

namespace sjtu
{
	template<class T>
	class deque {
	private:
		class Node;
		struct LLNode
		{
			T data;
			LLNode *next, *prev;
			Node *corres;
			LLNode(Node *fa, const T &_data) : data(_data), next(nullptr), prev(nullptr), corres(fa) {}
		};
		struct LinkedList
		{
			LLNode *head, *tail;
			int size;
			LinkedList() : head(nullptr), tail(nullptr), size(0) {}
			LinkedList(const LinkedList &other, Node *c)
			{
				head = copyAll(c, other.head);
				size = other.size;
			}
			~LinkedList()
			{
				LLNode *t = head;
				while (t != nullptr)
				{
					LLNode *tmp = t->next;
					delete t;
					t = tmp;
				}
			}

			//the same copy constructor as type Node
			LLNode* copyAll(Node *c, LLNode *cur, LLNode *prev = nullptr)
			{
				if (cur == nullptr) return nullptr;
				LLNode *t = new LLNode(c, cur->data);
				tail = t;
				t->prev = prev;
				if (prev != nullptr) prev->next = t;
				t->next = copyAll(c, cur->next, t);
				return t;
			}

			void push_front(Node *c, const T &value) //enough size for one more element
			{
				if (size == 0)
				{
					head = new LLNode(c, value);
					tail = head;
				}
				else
				{
					LLNode *t = new LLNode(c, value);
					t->next = head; head->prev = t;
					head = t;
				}
				size++;
			}

			void pop_front()
			{
				size--;
				if (size == 0)
				{
					delete head;
					head = tail = nullptr;
				}
				else
				{
					head = head->next;
					delete head->prev;
					head->prev = nullptr;
				}
			}

			void push_back(Node *c, const T &value)
			{
				if (size == 0)
				{
					head = new LLNode(c, value);
					tail = head;
				}
				else
				{
					LLNode *t = new LLNode(c, value);
					t->prev = tail; tail->next = t;
					tail = t;
				}
				size++;
			}

			void pop_back()
			{
				size--;
				if (size == 0) delete head, head = tail = nullptr;
				else tail = tail->prev, delete tail->next, tail->next = nullptr;
			}
		};

	private:
		const int ChunkSize = 100;
		struct Node
		{
			LinkedList *arr;
			Node *next, *prev;

			Node() : arr(new LinkedList()), next(nullptr), prev(nullptr) {}
			Node(int __size) : arr(new LinkedList()), next(nullptr), prev(nullptr) {}
			Node(const Node &other) : arr(new LinkedList(*other.arr, this)), next(nullptr), prev(nullptr) {}
			~Node()
			{
				delete arr;
			}
		};

	private:
		Node *head, *tail;
		int __size;

	private:
		//copy all elements, assign tail pointer but not head pointer
		Node* copyAll(Node *cur, Node *prev = nullptr)
		{
			if (cur == nullptr) return nullptr;
			Node *t = new Node(*cur);
			tail = t;
			t->prev = prev;
			if (prev != nullptr) prev->next = t;
			t->next = copyAll(cur->next, t);
			return t;
		}

		void __clear(Node *cur)
		{
			if (cur == nullptr) return;
			__clear(cur->next);
			delete cur;
		}

	public:
		deque() : head(nullptr), tail(nullptr), __size(0) { }
		deque(const deque &other) : __size(other.__size)
		{
			head = copyAll(other.head);
		}

		~deque()
		{
			__clear(head);
			head = tail = nullptr;
		}

		deque &operator=(const deque &other)
		{
			if (this == &other) return *this;
			__clear(head);
			head = copyAll(other.head);
			__size = other.__size;
			return *this;
		}

	public:
		class const_iterator;
		class iterator
		{
			friend const_iterator;
			friend iterator deque<T>::insert(iterator, const T&);
			friend iterator deque<T>::erase(iterator);

		private:
			LLNode *cur;
			int curPos;
			deque<T> *corres;

		public:
			iterator() = default;
			iterator(const iterator &o) = default;
			iterator &operator=(const iterator &o) = default;
			iterator(LLNode *_cur, int pos, deque *que) : cur(_cur), curPos(pos), corres(que) {}

		private:
			int getIndex() const
			{
				if (cur == nullptr && curPos == 0) return corres->__size;

				Node *t = corres->head;
				int counter = 0;
				while (t != cur->corres)
				{
					counter += t->arr->size;
					t = t->next;
				}
				counter += curPos;
				return counter;
			}

		public:
			bool valid()
			{
				return cur != nullptr && curPos < cur->corres->arr->size && corres != nullptr;
			}

			iterator operator+(const int &n) const
			{
				if (n == 0) return *this;
				return corres->find(getIndex() + n);
			}

			iterator operator-(const int &n) const
			{
				if (n == 0) return *this;
				return corres->find(getIndex() - n);
			}

			// return the distance between two iterator
			int operator-(const iterator &rhs) const
			{
				if (corres != rhs.corres) throw invalid_iterator();
				return getIndex() - rhs.getIndex();
			}

			iterator operator+=(const int &n)
			{
				(*this) = (*this) + n;
				return (*this);
			}

			iterator operator-=(const int &n)
			{
				(*this) = (*this) - n;
				return (*this);
			}

			iterator& operator++()
			{
				if (cur == nullptr) throw invalid_iterator();
				if (cur == cur->corres->arr->tail) //reach the end of the current chunk
				{
					if (cur->corres->next == nullptr)
					{
						cur = nullptr, curPos = 0;
						return *this;
					}
					cur = cur->corres->next->arr->head;
					curPos = 0;
				}
				else
				{
					cur = cur->next;
					curPos++;
				}
				return (*this);
			}

			iterator operator++(int)
			{
				iterator tmp = *this;
				++(*this);
				return tmp;
			}

			iterator& operator--()
			{
				if (cur == nullptr && curPos == 0) //the end
				{
					cur = corres->tail->arr->tail;
					curPos = corres->tail->arr->size - 1;
				}
				else if (cur == cur->corres->arr->head)
				{
					cur = cur->corres->prev->arr->tail;
					curPos = 0;
				}
				else
				{
					cur = cur->prev;
					curPos--;
				}
				return (*this);
			}

			iterator operator--(int)
			{
				iterator tmp = *this;
				--(*this);
				return tmp;
			}

			T& operator*() const
			{
				return cur->data;
			}
			T* operator->() const noexcept
			{
				return &cur->data;
			}

			bool operator==(const iterator &rhs) const
			{
				return (corres == rhs.corres) && (cur == rhs.cur) && (curPos == rhs.curPos);
			}
			bool operator==(const const_iterator &rhs) const
			{
				return (corres == rhs.corres) && (cur == rhs.cur) && (curPos == rhs.curPos);
			}
			bool operator!=(const iterator &rhs) const
			{
				return !((*this) == rhs);
			}
			bool operator!=(const const_iterator &rhs) const
			{
				return !((*this) == rhs);
			}
		};

		/*
		class const_iterator
		{
		private:
			LLNode *cur;
			int curPos;
			const deque<T> *corres;

		public:
			const_iterator() = default;
			const_iterator(const const_iterator &o) = default;
			const_iterator &operator=(const const_iterator &o) = default;
			const_iterator &operator=(const iterator &o)
			{
				cur = o.cur;
				curPos = o.curPos;
				corres = o.corres;
			}
			const_iterator(LLNode *_cur, int pos, const deque *que) : cur(_cur), curPos(pos), corres(que) {}
		*/

	private:
		iterator find(int num)
		{
			num++;
			Node *t = head;
			while (t != nullptr && num > (t->arr->size))
			{
				num -= t->arr->size;
				t = t->next;
			}
			if (t == nullptr || num > t->arr->size) return end();
			LLNode *t2 = t->arr->head;
			for (int i = 0; i < num - 1; i++, t2 = t2->next);
			return iterator(t2, num - 1, this);
		}

	public:
		iterator begin()
		{
			if (__size == 0) return iterator(nullptr, -1, this);
			else return iterator(head->arr->head, 0, this);
		}

		const_iterator cbegin() const
		{
			if (__size == 0) return const_iterator(nullptr, -1, this);
			else return const_iterator(head->arr->head, 0, this);
		}

		iterator end()
		{
			return iterator(nullptr, 0, this);
		}

		const_iterator cend() const
		{
			return const_iterator(nullptr, 0, this);
		}

	private:
		Node* split(Node *cur, int pos)
		{
			Node *newNode = new Node();
			LLNode *t = cur->arr->head;
			for (int i = 0; i < pos; i++) t = t->next;
			newNode->arr->tail = cur->arr->tail;
			cur->arr->tail = t->prev;

			if (t->prev != nullptr) t->prev->next = nullptr;
			t->prev = nullptr;

			newNode->arr->head = t;
			for (auto tmp = newNode->arr->head; tmp != nullptr; tmp = tmp->next) tmp->corres = newNode;
			newNode->arr->size = cur->arr->size - pos;
			cur->arr->size = pos;

			newNode->next = cur->next;
			if (cur->next != nullptr) cur->next->prev = newNode;
			else tail = newNode;
			cur->next = newNode;
			newNode->prev = cur;

			return newNode;
		}

	public:
		/**
		 * access specified element with bounds checking
		 * throw index_out_of_bound if out of bound.
		 */
		T& at(const int &pos)
		{
			if (pos > __size) throw index_out_of_bound();
			return *find(pos);
		}

		const T& at(const int &pos) const
		{
			if (pos > __size) throw index_out_of_bound();
			return *find(pos);
		}

		T& operator[](const int &pos)
		{
			T &ret = at(pos);
			return ret;
		}

		const T& operator[](const int &pos) const
		{
			const T& ret = at(pos);
			return ret;
		}

		/**
		 * access the first / last element
		 * throw container_is_empty when the container is empty.
		 */
		const T& front() const
		{
			if (__size == 0) throw container_is_empty();
			return head->arr->head->data;
		}

		const T& back() const
		{
			if (__size == 0) throw container_is_empty();
			return tail->arr->tail->data;
		}

		bool empty() const
		{
			return __size == 0;
		}

		int size() const
		{
			return __size;
		}

		void clear()
		{
			__clear(head);
			head = tail = nullptr;
			__size = 0;
		}

		iterator insert(iterator pos, const T &value)
		{
			//std::cout << "INSERT" << std::endl;
			if (pos == end())
			{
				push_back(value);
				return iterator(tail->arr->tail, tail->arr->size - 1, this);
			}
			if (pos == begin())
			{
				push_front(value);
				return begin();
			}
			if (!pos.valid()) throw invalid_iterator();
			Node *t = split(pos.cur->corres, pos.curPos);
			t->arr->push_front(t, value);
			__size++;
			return iterator(t->arr->head, 0, this);
		}
		/**
		 * removes specified element at pos.
		 * removes the element at pos.
		 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
		 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
		 */
		iterator erase(iterator pos)
		{
			if (!pos.valid()) throw invalid_iterator();
			Node *t = split(pos.cur->corres, pos.curPos);
			t->arr->pop_front();
			__size--;
			return iterator(t->arr->head, 0, this);
		}

		void push_back(const T &value)
		{
			if (__size == 0)
			{
				tail = new Node();
				head = tail;
			}
			else if (tail->arr->size == ChunkSize)
			{
				Node *t = new Node();
				tail->next = t;
				t->prev = tail;
				tail = t;
			}
			tail->arr->push_back(tail, value);
			__size++;
		}

		void pop_back()
		{
			if (empty()) throw container_is_empty();
			tail->arr->pop_back();
			if (tail->arr->size == 0) tail = tail->prev;
			__size--;
		}

		void push_front(const T &value)
		{
			if (__size == 0)
			{
				tail = new Node();
				head = tail;
			}
			else if (head->arr->size == ChunkSize)
			{
				Node *t = new Node();
				t->next = head;
				head->prev = t;
				head = t;
			}
			head->arr->push_front(head, value);
			__size++;
		}

		void pop_front()
		{
			if (empty()) throw container_is_empty();
			head->arr->pop_front();
			if (head->arr->size == 0) head = head->next;
			__size--;
		}
	};
}
#endif
