#ifndef MP_LINKEDLIST_HPP
#define MP_LINKEDLIST_HPP

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <assert.h>
#include "IntrusivePtr.hpp"

// lockfree

template<typename T>
class LinkedList {
public:
	struct Item;
	typedef IntrusivePtr<Item> ItemPtr;

	struct Item : public boost::intrusive_ref_counter< Item, boost::thread_safe_counter > {
		ItemPtr next;
		T value;
	};

private:
	ItemPtr first, last;

public:
	// not thread-safe!
	struct Iterator {
		ItemPtr ptr;

		Iterator(const ItemPtr& _ptr) : ptr(_ptr) {}

		T& operator*() {
			return ptr->value;
		}

		Iterator& operator++() {
			ptr = ptr->next;
			return *this;
		}

		bool operator!=(const Iterator& other) const {
			return ptr != other.ptr;
		}
	};

	Iterator begin() {
		return Iterator(first);
	}

	Iterator end() {
		return Iterator(NULL);
	}

	// only single producer supported
	ItemPtr push_back(ItemPtr item = NULL) {
		if(!item) item.reset(new Item());
		ItemPtr oldLast = last;
		if(oldLast) oldLast->next = item;
		last = item;
		if(!first) first = item;
		return item;
	}

	// only single consumer supported
	ItemPtr pop_front() {
		ItemPtr oldFirst = first;
		if(oldFirst) {
			first = oldFirst->next;
			if(!first) last = NULL;
			return oldFirst;
		}
		return NULL;
	}

	// not multithreading safe at all
	void clear() {
		first.reset();
		last.reset();
	}

	bool empty() {
		return !first;
	}

	// be very careful!
	T& front() { return first->value; }
	T& back() { return last->value; }


	void _check_sanity() {
		if(!first) assert(!last);
		else assert(last);
	}
};


#endif // LINKEDLIST_HPP