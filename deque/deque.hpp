#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

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

			//the same copy constructor as type Node
			LLNode* copyAll(Node *c, LLNode *cur, LLNode *prev = nullptr)
			{
				if (cur == nullptr) return nullptr;
				LLNode *t = new LLNode(c, cur->data);
				tail = t;
				t->prev = prev;
				t->next = copyAll(c, cur->next, t);
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
				if (size == 0) head = tail = nullptr;
				else head = head->next;
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
				if (size == 0) head = tail = nullptr;
				else tail = tail->prev;
			}
		};

	private:
		const int ChunkSize = 100;
		struct Node
		{
			LinkedList *arr;
			int size;
			Node *next, *prev;

			Node() : arr(new LinkedList()), size(0), next(nullptr), prev(nullptr) {}
			Node(int __size) : arr(new LinkedList()), size(__size), next(nullptr), prev(nullptr) {}
			Node(const Node &other) : next(nullptr), prev(nullptr)
			{
				size = other.size;
				arr->head = copyAll(this, other->arr->head);
			}
			~Node() {
				delete arr;
			}
		};

	private:
		Node *head, *tail;
		int __size;

	private:
		//copy all elements, assign tail pointer but not head pointer
		Node* copyALL(Node *cur, Node *prev = nullptr)
		{
			if (cur == nullptr) return nullptr;
			Node *t = new Node(*cur);
			tail = t;
			t->prev = prev;
			t->next = copyAll(cur->next, t);
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
			__clear(head);
			head = copyALL(other.head);
			__size = other.__size;
		}

	private:
		class const_iterator;
		class iterator
		{
			friend const_iterator;
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
			int getIndex()
			{
				Node *t = head;
				int counter = 0;
				while (t != cur->corres)
				{
					counter += t->size;
					t = t->next;
				}
				counter += curPos + 1;
				return counter;
			}

		public:
			bool valid()
			{
				return cur != nullptr && curPos < cur->corres->size && corres != nullptr;
			}

			iterator operator+(const int &n) const
			{
				return find(getIndex() + n);
			}

			iterator operator-(const int &n) const
			{
				return find(getIndex() + n);
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
				if (this == end) throw invalid_iterator();
				if (cur == cur->corres->arr->tail) //reach the end of the current chunk
				{
					if (cur->corres->next == nullptr)
					{
						cur = nullptr;
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
				if (cur == nullptr) //the end
				{
					cur = tail->arr->tail;
					curPos = 0;
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
				return cur;
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

		private:
			int getIndex()
			{
				Node *t = head;
				int counter = 0;
				while (t != cur->corres)
				{
					counter += t->size;
					t = t->next;
				}
				counter += curPos;
				return counter;
			}

		public:
			bool valid()
			{
				return cur != nullptr && curPos < cur->corres->size && corres != nullptr;
			}

			const_iterator operator+(const int &n) const {
				return find(getIndex() + n);
			}

			const_iterator operator-(const int &n) const {
				return find(getIndex() - n);
			}

			// return the distance between two iterator
			int operator-(const const_iterator &rhs) const
			{
				if (corres != rhs.corres) throw invalid_iterator();
				return getIndex() - rhs.getIndex();
			}

			const_iterator operator+=(const int &n)
			{
				(*this) = (*this) + n;
				return (*this);
			}

			const_iterator operator-=(const int &n)
			{
				(*this) = (*this) - n;
				return (*this);
			}

			const_iterator& operator++()
			{
				if (this == end) throw invalid_iterator();
				if (cur == cur->corres->arr->tail) //reach the end of the current chunk
				{
					if (cur->corres->next == nullptr)
					{
						cur = nullptr;
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

			const_iterator operator++(int)
			{
				const_iterator tmp = *this;
				++(*this);
				return tmp;
			}

			const_iterator& operator--()
			{
				if (cur == nullptr) //the end
				{
					cur = tail->arr->tail;
					curPos = 0;
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

			const_iterator operator--(int)
			{
				const_iterator tmp = *this;
				--(*this);
				return tmp;
			}

			const T& operator*() const
			{
				return cur->data;
			}
			const T* operator->() const noexcept
			{
				return cur;
			}
		};
	private:
		iterator find(int num)
		{
			Node *t = head;
			while (num > (t->size))
			{
				num -= t->size;
				t = t->next;
			}
			LLNode *t2 = t->arr->head;
			for (int i = 0; i < num; i++, t2 = t2->next);
			return iterator(t2, num, this);
		}

	public:

		iterator begin()
		{
			return iterator(head, head->arr->head, 0, this);
		}

		const_iterator cbegin() const
		{
			return const_iterator(head, head->arr->head, 0, this);
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
			Node *t = new Node();
			LLNode *t = cur->arr->head;
			for (int i = 0; i < pos; i++) t = t->next;

			if (t->prev != nullptr) t->prev->next = nullptr;
			t->prev = nullptr;
			t->arr->head = t;

			t->size = cur->size - (pos + 1);
			cur->size = pos + 1;
			return t;
		}

	public:
		/**
		 * access specified element with bounds checking
		 * throw index_out_of_bound if out of bound.
		 */
		T & at(const int &pos)
		{
			if (pos > __size) throw index_out_of_bound();
			return *find(pos);
		}

		const T & at(const int &pos) const
		{
			if (pos > __size) throw index_out_of_bound();
			return *find(pos);
		}

		T & operator[](const int &pos)
		{
			T &ret = at(pos);
			return ret;
		}

		const T & operator[](const int &pos) const
		{
			const T& ret = at(pos);
			return ret;
		}

		/**
		 * access the first / last element
		 * throw container_is_empty when the container is empty.
		 */
		const T & front() const
		{
			if (__size == 0) throw container_is_empty();
			return head->arr->head->data;
		}

		const T & back() const
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
			if (!pos.valid()) throw invalid_iterator();
			Node *t = new Node();
			t->arr->push_back(value);

			t.next = pos->cur1->next;
			t.prev = pos->cur1;
			if (pos->cur1->next != nullptr) pos->cur1->next->prev = t;
			else tail = t;
			pos->cur1->next = t;

			__size++;
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
			t->pop_front();
			return iterator(t->arr->head, t->arr->size, this);
			__size--;
		}

		void push_back(const T &value)
		{
			if (__size == 0)
			{
				tail = new Node();
				head = tail;
			}
			else if (tail->size == ChunkSize)
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
			__size--;
		}

		void push_front(const T &value)
		{
			if (__size == 0)
			{
				tail = new Node();
				head = new Node();
			}
			else if (head->size == ChunkSize)
			{
				Node *t = new Node();
				t->next = head;
				head->prev = t;
				head = t;
			}
			head->arr->push_front(tail, value);
			__size++;
		}

		void pop_front()
		{
			if (empty()) throw container_is_empty();
			head->arr->pop_front();
			__size--;
		}
	};
}
#endif
