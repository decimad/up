#include "introspection.h"
#include <queue>
#include <stack>

namespace up {

	void bases_breadth_first(const type* cl, const std::function<void(const type*)>& func) {
		std::queue< const type* > queue;
		queue.push(cl);

		while(queue.size()) {
			const type* front = queue.front();
			queue.pop();
			func(front);

			const std::size_t size = front->num_bases();
			for(std::size_t i = 0; i<size; ++i) {
				queue.push(front->get_base(i));
			}
		}
	}

	void bases_depth_first(const type* cl, const std::function<void(const type*)>& func) {
		std::stack< const type* > stack;
		stack.push(cl);

		while(stack.size()) {
			const type* front = stack.top();
			stack.pop();

			func(front);

			const std::size_t size = front->num_bases();
			for(std::size_t i = 0; i<size; ++i) {
				stack.push(front->get_base(i));
			}
		}

	}

}