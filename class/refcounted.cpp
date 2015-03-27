#include "refcounted.h"

namespace up {

	refcounted_virtual::refcounted_virtual()
		: refcount_(0)
	{}

	void intrusive_ptr_add_ref(refcounted_virtual* ptr) {
		++ptr->refcount_;
	}

	void intrusive_ptr_release(refcounted_virtual* ptr)
	{
		--ptr->refcount_;
		if(ptr->refcount_==0) {
			delete ptr;
		}
	}

}