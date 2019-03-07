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
		node *firstChild, *rightBrother, *father;
		T val;
		node(const T &_val) : firstChild(nullptr), rightBrother(nullptr), father(nullptr), val(_val) { }
	};
	struct Queue
	{
		struct qNode
		{
			qNode *next;
			node *cur;
			qNode(node *_cur = nullptr) : next(nullptr), cur(_cur) {}
		};
		qNode *head = nullptr, *tail = nullptr;
		int size = 0;

		bool empty() { return size == 0; }
		node* pop() 
		{
			size--;
			node* ret = head->cur;
			auto tmp = head;
			head = head->next;
			delete tmp;
			return ret;
		}
		void push(node *t)
		{
			if (size == 0)
			{
				head = new qNode(t);
				tail = head;
			}
			else tail->next = new qNode(t), tail = tail->next;
            size++;
		}
	};

private:
	node *root;
	size_t __size;

private:
	void clear(node *cur)
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
					clear(u);
					u = tmp;
				}
			}
		}
		delete cur;
		__size = 0;
	}

	void addAll(priority_queue *que, node *cur)
	{
		que->push(cur->val);
		if (cur->firstChild != nullptr)
		{
			node *u = cur->firstChild;
			while (u != nullptr)
			{
				addAll(que, u);
				u = u->rightBrother;
			}
		}
	}

public:
	priority_queue() : root(nullptr), __size(0) { }

	priority_queue(const priority_queue &other) : root(nullptr), __size(0)
	{
		addAll(this, other.root);
	}

	~priority_queue() 
	{
		clear(root);
		root = nullptr;
		__size = 0;
    }

	priority_queue &operator=(const priority_queue &other) 
	{
		if (this == &other) return *this;
		clear(root);
		root = nullptr;
		__size = 0;
		
		addAll(this, other.root);
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
        Queue *q = new Queue();
		node *u = root->firstChild;
		while (u != nullptr)
		{
			if (u->father == root) q->push(u);
			node *tmp = u->rightBrother;
            u->father = u->rightBrother = nullptr;
            u = tmp;
		}
        node *newRoot = nullptr;
		while (q->size >= 2)
		{
			node *a = q->pop(), *b = q->pop();
            q->push(newRoot = __merge(a, b));
		}
        delete root;
        if (__size && newRoot == nullptr) newRoot = q->pop();
        root = newRoot;
        delete q;
	}

	size_t size() const 
	{
		return __size;
	}

	bool empty() const 
	{
		return __size == 0;
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
