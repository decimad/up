#pragma once
#include <vector>
#include <memory>
#include "sequence.h"
#include <unordered_map>

namespace up {

	class class_;

	using std::unique_ptr;

	class namespace_ {
	public:
		namespace_(namespace_& parent);

		using nested_container_type = std::vector< unique_ptr< namespace_ > >;
		using nested_iterator = nested_container_type::iterator;
		using nested_sequence = sequence< nested_iterator >;

		using class_container_type = std::unordered_map< std::string, const class_* >;
		using class_iterator = class_container_type::iterator;
		using class_sequence = sequence< class_iterator >;

		sequence< nested_iterator > nested();
		sequence< class_iterator> classes();

		static namespace_& global() {
			static namespace_ ns(0);
			return ns;
		}

	private:
		namespace_();

		void add_nested(namespace_*);
		void add_class(const class_*);

	private:
		std::unordered_map<std::string, const class_*> classes_;
		std::vector< namespace_* > nested_spaces_;
		namespace_* parent;
	};
	
}
