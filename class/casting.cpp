#include "casting.h"
#include <unordered_map>

namespace up {

	namespace casts {

		using cast_type = cast_func_type;
		using cast_pair = std::pair< const meta_type*, cast_type >;
		using cast_vector = std::vector< cast_pair >;
		using cast_map = std::unordered_map < const meta_type*, cast_vector >;

		cast_map& get_upcast_map() {
			static cast_map map;
			return map;
		}

		cast_map& get_downcast_map() {
			static cast_map map;
			return map;
		}
				
		void declare_upcast(const meta_type* a, const meta_type* b, cast_func_type cast)
		{
			get_upcast_map()[a].emplace_back(b, cast);			
		}

		void declare_downcast(const meta_type* a, const meta_type* b, cast_func_type cast) {
			get_downcast_map()[a].emplace_back(b, cast);
		}

		using path_type = std::vector< cast_func_type >;

		path_type search_path(cast_map& map, const meta_type* src, const meta_type* dest)
		{
			path_type path;
			using stack_elem_type = std::tuple< const meta_type*, cast_vector&, int >;

			std::vector< stack_elem_type > search_stack;
			std::vector< cast_func_type > cast_stack;

			search_stack.emplace_back(src, map[src], 0);

			while(search_stack.size()) {
				auto& top = search_stack.back();

				const meta_type* curr_class = std::get<0>(top);
				const auto& curr_cast_vector = std::get<1>(top);
				size_t curr_cast_index = std::get<2>(top);

				if(curr_class==dest) {
					return cast_stack;
				} else if(curr_cast_vector.size() == curr_cast_index) {
					search_stack.pop_back();
					if(cast_stack.size()) cast_stack.pop_back();
				} else {
					const auto* next_class = curr_cast_vector[curr_cast_index].first;
					cast_func_type next_cast = curr_cast_vector[curr_cast_index].second;
					++std::get<2>(top);
					search_stack.emplace_back(next_class, map[next_class], 0);
					cast_stack.emplace_back(next_cast);
				}
			}

			return path_type();
		}
		
		const void* do_upcast(const meta_type* src_class, const void* src_ptr, const meta_type* dest_class)
		{
			using pair_type = std::pair<const type*, const void*>;
			using queue_type = std::queue< pair_type >;
			
			if (src_class == dest_class) {
				return src_ptr;
			}

			auto path = search_path(get_upcast_map(), src_class, dest_class);

			if(path.size()) {
				for(auto& func:path) {
					src_ptr = func(src_ptr);
				}

				return src_ptr;
			} else {
				return nullptr;
			}
		}

		void* do_upcast(const meta_type* src_class, void* src_ptr, const meta_type* dest_class)
		{
			return const_cast<void*>(do_upcast(src_class, static_cast<const void*>(src_ptr), dest_class));
		}

		const void* do_downcast(const meta_type* src_class, const void* src_ptr, const meta_type* dest_class)
		{
			using pair_type = std::pair<const type*, const void*>;
			using queue_type = std::queue< pair_type >;

			if(src_class==dest_class) {
				return src_ptr;
			}

			auto path = search_path(get_downcast_map(), src_class, dest_class);

			if(path.size()) {
				for(auto& func:path) {
					src_ptr = func(src_ptr);
				}

				return src_ptr;
			} else {
				return nullptr;
			}
		}

		void* do_downcast(const meta_type* src_class, void* src_ptr, const meta_type* dest_class)
		{
			return const_cast<void*>(do_downcast(src_class, static_cast<const void*>(src_ptr), dest_class));
		}

	}

}