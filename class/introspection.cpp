#include "introspection.h"
#include <queue>
#include <stack>

namespace up {

	/*
	void bases_breadth_first(const meta_type& cl, const std::function<void(type_ref)>& func) {
		std::queue< const meta_type* > queue;
		queue.push(&cl);

		while(queue.size()) {
			const meta_type* front = queue.front();
			queue.pop();
			func(*front);

			const std::size_t size = front->num_bases();
			for(std::size_t i = 0; i<size; ++i) {
				queue.push(front->get_base(i));
			}
		}
	}

	void bases_depth_first(const meta_type& cl, const std::function<void(type_ref)>& func) {
		std::stack< const meta_type* > stack;
		stack.push(&cl);

		while(stack.size()) {
			const meta_type* front = stack.top();
			stack.pop();

			func(*front);

			const std::size_t size = front->num_bases();
			for(std::size_t i = 0; i<size; ++i) {
				stack.push(front->get_base(i));
			}
		}

	}
	*/

}