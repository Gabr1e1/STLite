#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <iostream>
#include <cstddef>
#include <functional>
#include "exceptions.hpp"
using namespace std;

namespace sjtu {

template<typename T, class Compare = std::less<T>>
class priority_queue {
	struct node
	{
		node *firstChild, *rightBrother, *father, *tmpPrev;
		T val;
		node(const T &_val) : firstChild(nullptr), rightBrother(nullptr), father(nullptr), val(_val) { }
	};

private:
	node *root;
	size_t __size;

private:
	void __clear(node *cur)
	{
		if (cur == nullptr) return;
		if (cur->firstChild != nullptr)
		{
			node *u = cur->firstChild;
			while (u != nullptr)
			{
				if (u->father == cur)
				{
					node *tmp = u->rightBrother;
					__clear(u);
					u = tmp;
				}
			}
		}
		delete cur;
		__size = 0;
	}

	void __addAll(priority_queue *que, node *cur)
	{
		que->push(cur->val);
		if (cur->firstChild != nullptr)
		{
			node *u = cur->firstChild;
			while (u != nullptr)
			{
				__addAll(que, u);
				u = u->rightBrother;
			}
		}
	}

public:
	priority_queue() : root(nullptr), __size(0) { }

	priority_queue(const priority_queue &other) : root(nullptr), __size(0)
	{
		__addAll(this, other.root);
	}

	~priority_queue() 
	{
		__clear(root);
		root = nullptr;
		__size = 0;
    }

	priority_queue &operator=(const priority_queue &other) 
	{
		if (this == &other) return *this;
		__clear(root);
		root = nullptr;
		__size = 0;
		
		__addAll(this, other.root);
		return *this;
	}

private:
	void addSon(node *p, node *u)
	{
		u->father = p;
		if (p->firstChild == nullptr) p->firstChild = u;
		else
		{
            u->rightBrother = p->firstChild;
			p->firstChild = u;
		}
	}

	node* __merge(node *u, node *v)
	{
		if (Compare()(u->val, v->val))
		{
			addSon(v, u);
			return v;
		}
		else
		{
			addSon(u, v);
			return u;
		}
	}

public:
	size_t size() const { return __size; }
	bool empty() const { return __size == 0; }

	const T & top() const
    {
		if (empty()) throw container_is_empty();
        return root->val;
	}

	void push(const T &e) 
	{
		node *u = new node(e);
		if (root != nullptr) root = __merge(root, u);
		else root = u;
        __size++;
	}

	void pop() 
	{
		if (empty()) throw container_is_empty();
		__size--;
		//left to right
		node *first = root->firstChild, *second = (first != nullptr ? first->rightBrother : nullptr), *tail = nullptr, *next;
		delete root;

		if (first != nullptr) first->rightBrother = nullptr;
		if (second != nullptr) next = second->rightBrother;
		else next = nullptr;
		if (second != nullptr) second->rightBrother = nullptr;

		while (first != nullptr && second != nullptr)
		{
			node *cur = __merge(first, second);
			cur->tmpPrev = tail;
			tail = cur;

			first = next;
			if (first != nullptr) second = first->rightBrother;
			else second = nullptr;

			if (first != nullptr) first->rightBrother = nullptr;
			if (second != nullptr) next = second->rightBrother;
			else next = nullptr;
			if (second != nullptr) second->rightBrother = nullptr;

		}
		//right to left
		if (first != nullptr) first->tmpPrev = tail, tail = first;
		while (tail != nullptr && tail->tmpPrev != nullptr)
		{
			node *next = tail->tmpPrev->tmpPrev;
			tail = __merge(tail, tail->tmpPrev);
			tail->tmpPrev = next;
		}
		root = tail;
	}

	void merge(priority_queue &other) 
	{
		root = __merge(root, other.root);
		__size += other.__size;
		other.__size = 0;
		other.root = nullptr;
	}
};

}

#endif
