#pragma once
#include <vector>
#include <memory>
#include "sequence.h"
#include <unordered_map>

namespace up {

	class type;

	using std::unique_ptr;

	class namespace_ {
	public:
		namespace_(namespace_& parent);

		using nested_container_type = std::vector< unique_ptr< namespace_ > >;
		using nested_iterator = nested_container_type::iterator;
		using nested_sequence = sequence< nested_iterator >;

		using typecontainer_type = std::unordered_map< std::string, const type* >;
		using typeiterator = typecontainer_type::iterator;
		using typesequence = sequence< typeiterator >;

		sequence< nested_iterator > nested();
		sequence< typeiterator> classes();

		static namespace_& global() {
			static namespace_ ns(0);
			return ns;
		}

	private:
		namespace_();

		void add_nested(namespace_*);
		void add_class(const type*);

	private:
		std::unordered_map<std::string, const type*> classes_;
		std::vector< namespace_* > nested_spaces_;
		namespace_* parent;
	};
	
}
