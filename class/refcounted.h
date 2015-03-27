#pragma once
#include <cstddef>

namespace up {

	class refcounted_virtual {
	public:
		refcounted_virtual();
		virtual ~refcounted_virtual() {}

		friend void intrusive_ptr_add_ref(refcounted_virtual*);
		friend void intrusive_ptr_release(refcounted_virtual*);

	private:
		std::size_t refcount_;
	};

}
