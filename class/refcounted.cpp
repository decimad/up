#include "refcounted.h"

namespace up {

	refcounted_virtual::refcounted_virtual()
		: refcount_(0)
	{}

	void intrusive_ptr_add_ref(refcounted_virtual* ptr) {
		ptr->refcount_.fetch_add(1u, std::memory_order_relaxed);
	}

	void intrusive_ptr_release(refcounted_virtual* ptr)
	{
		if (ptr->refcount_.fetch_sub(1u, std::memory_order_release) == 1) {
			std::atomic_thread_fence(std::memory_order_acquire);
			delete ptr;
		}
	}

}