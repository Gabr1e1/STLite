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
			LLNode(const T &_data) : data(_data), next(nullptr), prev(nullptr) {}
		};
		struct LinkedList
		{
			LLNode *head, *tail;
			int size;
			LinkedList() : head(nullptr), tail(nullptr), size(0) {}
			LinkedList(const LinkedList &other)
			{
				head = copyAll(other.head);
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

			void merge(LinkedList *other)
			{
				if (other->size == 0) return;
				if (head == nullptr) head = other->head;
				else tail->next = other->head, other->head->prev = tail;
				tail = other->tail;
				size += other->size;
				other->head = other->tail = nullptr;
				other->size = 0;
			}

			LLNode* copyAll(LLNode *cur, LLNode *prev = nullptr)
			{
				if (cur == nullptr) return nullptr;
				LLNode *t = new LLNode(cur->data);
				tail = t;
				t->prev = prev;
				if (prev != nullptr) prev->next = t;
				t->next = copyAll(cur->next, t);
				return t;
			}

			void push_front(const T &value) //enough size for one more element
			{
				if (size == 0) head = tail = new LLNode(value);
				else
				{
					LLNode *t = new LLNode(value);
					t->next = head; head->prev = t;
					head = t;
				}
				size++;
			}

			void pop_front()
			{
				size--;
				if (size == 0) delete head, head = tail = nullptr;
				else
				{
					head = head->next, delete head->prev, head->prev = nullptr;
				}
			}

			void push_back(const T &value)
			{
				if (size == 0) head = tail = new LLNode(value);
				else
				{
					LLNode *t = new LLNode(value);
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
		const int ChunkSize = 500;
		struct Node
		{
			LinkedList *arr;
			Node *next, *prev;

			Node() : arr(new LinkedList()), next(nullptr), prev(nullptr) {}
			Node(const Node &other) : arr(new LinkedList(*other.arr)), next(nullptr), prev(nullptr) {}
			~Node() {
				delete arr;
			}
		};

	public:
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
		~deque() {
			__clear(head);
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
			Node *fa;
			LLNode *cur;
			int curPos;
			deque<T> *corres;

		public:
			iterator() = default;
			iterator(const iterator &o) = default;
			iterator &operator=(const iterator &o) = default;
			iterator(Node *_fa, LLNode *_cur, int pos, deque *que) : fa(_fa), cur(_cur), curPos(pos), corres(que) {}

		private:
			int getIndex() const
			{
				if (cur == nullptr && curPos == 0) return corres->__size; //end()
				Node *t = corres->head;
				int counter;
				for (counter = 0; t != fa; t = t->next) counter += t->arr->size;
				counter += curPos;
				return counter;
			}

		public:
			bool valid() const
			{
				return fa != nullptr && cur != nullptr && curPos < fa->arr->size && corres != nullptr;
			}

			iterator operator+(const int &n) const
			{
				if (n == 0) return *this;
				return corres->find(getIndex() + n);
			}

			iterator operator-(const int &n) const
			{
				return *this + (-n);
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
				if (cur == fa->arr->tail) //reach the end of the current chunk
				{
					if (fa->next == nullptr)
					{
						fa = nullptr, cur = nullptr, curPos = 0;
						return *this;
					}
					fa = fa->next, cur = fa->arr->head, curPos = 0;
				}
				else cur = cur->next, curPos++;
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
				if (corres->empty()) throw invalid_iterator();
				if (cur == nullptr && curPos == 0) //end() - 1
				{
					fa = corres->tail;
					cur = corres->tail->arr->tail;
					curPos = corres->tail->arr->size - 1;
				}
				else if (cur == fa->arr->head)
				{
					if (fa->prev == nullptr) throw invalid_iterator();
					fa = fa->prev, cur = fa->arr->tail, curPos = 0;
				}
				else cur = cur->prev, curPos--;
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
				if (!valid()) throw invalid_iterator();
				return cur->data;
			}
			T* operator->() const noexcept
			{
				return &cur->data;
			}

			bool operator==(const iterator &rhs) const
			{
				return (corres == rhs.corres) && (fa == rhs.fa) && (cur == rhs.cur) && (curPos == rhs.curPos);
			}
			bool operator==(const const_iterator &rhs) const
			{
				return (corres == rhs.corres) && (fa == rhs.fa) && (cur == rhs.cur) && (curPos == rhs.curPos);
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
			Node *fa;
			LLNode *cur;
			int curPos;
			const deque<T> *corres;

		public:
			const_iterator() = default;
			const_iterator(const const_iterator &o) = default;
			const_iterator &operator=(const const_iterator &o) = default;
			const_iterator &operator=(const iterator &o)
			{
				fa = o.fa, cur = o.cur, curPos = o.curPos, corres = o.corres;
			}
			const_iterator(Node *_fa, LLNode *_cur, int pos, const deque *que) : fa(_fa), cur(_cur), curPos(pos), corres(que) {}

		private:
			int getIndex() const
			{
				if (cur == nullptr && curPos == 0) return corres->__size; //end()
				Node *t = corres->head;
				int counter;
				for (counter = 0; t != fa; t = t->next) counter += t->arr->size;
				counter += curPos;
				return counter;
			}

		public:
			bool valid() const
			{
				return fa != nullptr && cur != nullptr && curPos < fa->arr->size && corres != nullptr;
			}

			const_iterator operator+(const int &n) const
			{
				if (n == 0) return *this;
				return corres->find(getIndex() + n);
			}

			const_iterator operator-(const int &n) const
			{
				return *this + (-n);
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
				if (cur == nullptr) throw invalid_iterator();
				if (cur == fa->arr->tail) //reach the end of the current chunk
				{
					if (fa->next == nullptr)
					{
						fa = nullptr, cur = nullptr, curPos = 0;
						return *this;
					}
					fa = fa->next, cur = fa->arr->head, curPos = 0;
				}
				else cur = cur->next, curPos++;
				return (*this);
			}

			const_iterator operator++(int)
			{
				iterator tmp = *this;
				++(*this);
				return tmp;
			}

			const_iterator& operator--()
			{
				if (corres->empty()) throw invalid_iterator();
				if (cur == nullptr && curPos == 0) //end() - 1
				{
					fa = corres->tail;
					cur = corres->tail->arr->tail;
					curPos = corres->tail->arr->size - 1;
				}
				else if (cur == fa->arr->head)
				{
					if (fa->prev == nullptr) throw invalid_iterator();
					fa = fa->prev, cur = fa->arr->tail, curPos = 0;
				}
				else cur = cur->prev, curPos--;
				return (*this);
			}

			const_iterator operator--(int)
			{
				iterator tmp = *this;
				--(*this);
				return tmp;
			}

			const T& operator*() const
			{
				if (!valid()) throw invalid_iterator();
				return cur->data;
			}
			const T* operator->() const noexcept
			{
				return &cur->data;
			}

			bool operator==(const const_iterator &rhs) const
			{
				return (corres == rhs.corres) && (fa == rhs.fa) && (cur == rhs.cur) && (curPos == rhs.curPos);
			}
			bool operator!=(const const_iterator &rhs) const
			{
				return !(*this == rhs);
			}
		};

	private:
		iterator find(int num)
		{
			num++;
			if (num == __size + 1) return end();
			Node *t = head;
			while (t != nullptr && num > (t->arr->size))
			{
				num -= t->arr->size;
				t = t->next;
			}
			LLNode *t2 = t->arr->head;
			for (int i = 0; i < num - 1; i++, t2 = t2->next);
			return iterator(t, t2, num - 1, this);
		}

		const_iterator find(int num) const
		{
			if (num == __size + 1) return cend();
			num++;
			Node *t = head;
			while (t != nullptr && num > (t->arr->size))
			{
				num -= t->arr->size;
				t = t->next;
			}
			if (t == nullptr || num > t->arr->size) return cend();
			LLNode *t2 = t->arr->head;
			for (int i = 0; i < num - 1; i++, t2 = t2->next);
			return const_iterator(t, t2, num - 1, this);
		}

	public:
		iterator begin()
		{
			if (empty()) return iterator(nullptr, nullptr, 0, this);
			return iterator(head, head->arr->head, 0, this);
		}
		const_iterator cbegin() const {
			if (empty()) return const_iterator(nullptr, nullptr, 0, this);
			return const_iterator(head, head->arr->head, 0, this);
		}
		iterator end() {
			return iterator(nullptr, nullptr, 0, this);
		}
		const_iterator cend() const {
			return const_iterator(nullptr, nullptr, 0, this);
		}

	private:
		Node* split(Node *cur, int pos, LLNode *t)
		{
			Node *newNode = new Node();
			newNode->arr->head = t;
			newNode->arr->tail = cur->arr->tail;
			cur->arr->tail = t->prev;
			if (cur->arr->tail == nullptr) cur->arr->head = nullptr;

			if (t->prev != nullptr) t->prev->next = nullptr;
			t->prev = nullptr;

			newNode->arr->size = cur->arr->size - pos;
			cur->arr->size = pos;

			newNode->next = cur->next;
			if (cur->next != nullptr) cur->next->prev = newNode;
			cur->next = newNode;
			newNode->prev = cur;

			return newNode;
		}

	public:
		T& at(const int &pos)
		{
			if (pos >= __size || pos < 0) throw index_out_of_bound();
			return *find(pos);
		}

		const T& at(const int &pos) const
		{
			if (pos >= __size || pos < 0)  throw index_out_of_bound();
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

		bool empty() const {
			return __size == 0;
		}

		int size() const {
			return __size;
		}

		void clear()
		{
			__clear(head);
			head = tail = nullptr;
			__size = 0;
		}

	private:
		void merge(Node *a, Node *b)
		{
			a->arr->merge(b->arr);
			a->next = b->next;
			if (b->next != nullptr) b->next->prev = a;
			delete b;
		}

		void maintain()
		{
			if (__size == 0)
			{
				head = tail = nullptr;
				return;
			}
			Node *t = head;
			while (t->next != nullptr)
			{
				if (t->arr->size + t->next->arr->size <= ChunkSize) merge(t, t->next);
				else t = t->next;
			}
			tail = t;
		}

	public:
		iterator insert(iterator pos, const T &value)
		{
			if (pos == end())
			{
				push_back(value);
				return iterator(tail, tail->arr->tail, tail->arr->size - 1, this);
			}
			else if (pos == begin())
			{
				push_front(value);
				return begin();
			}
			int r = pos.getIndex();
			if (!pos.valid() || pos.corres != this) throw invalid_iterator();
			Node *t = split(pos.fa, pos.curPos, pos.cur);
			t->prev->arr->push_back(value);
			__size++;
			maintain();
			return find(r);
		}

		iterator erase(iterator pos)
		{
			int r = pos.getIndex();
			if (!pos.valid() || pos.corres != this) throw invalid_iterator();
			Node *t = split(pos.fa, pos.curPos, pos.cur);
			t->arr->pop_front();
			__size--;
			maintain();
			return find(r);
		}

		void push_back(const T &value)
		{
			if (empty()) head = tail = new Node();
			else if (tail->arr->size == ChunkSize)
			{
				Node *t = new Node();
				tail->next = t;
				t->prev = tail;
				tail = t;
			}
			tail->arr->push_back(value);
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
			if (empty()) head = tail = new Node();
			else if (head->arr->size == ChunkSize)
			{
				Node *t = new Node();
				t->next = head;
				head->prev = t;
				head = t;
			}
			head->arr->push_front(value);
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
