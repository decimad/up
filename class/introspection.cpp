#include "introspection.h"
#include <queue>
#include <stack>

namespace up {

	void bases_breadth_first(const class_* cl, const std::function<void(const class_*)>& func) {
		std::queue< const class_* > queue;
		queue.push(cl);

		while(queue.size()) {
			const class_* front = queue.front();
			queue.pop();
			func(front);

			const std::size_t size = front->num_bases();
			for(std::size_t i = 0; i<size; ++i) {
				queue.push(front->get_base(i));
			}
		}
	}

	void bases_depth_first(const class_* cl, const std::function<void(const class_*)>& func) {
		std::stack< const class_* > stack;
		stack.push(cl);

		while(stack.size()) {
			const class_* front = stack.top();
			stack.pop();

			func(front);

			const std::size_t size = front->num_bases();
			for(std::size_t i = 0; i<size; ++i) {
				stack.push(front->get_base(i));
			}
		}

	}

}