#pragma once

#include "class.h"
#include <string>
#include <functional>

namespace up {

	class array {
	public:
		virtual const type* element_class() const = 0;
		virtual std::size_t num_elements() const = 0;
		virtual object_ref& get_element() = 0;
	};

	class map {
	public:
		virtual const type* element_class() const = 0;
		virtual std::size_t num_elements() const = 0;
		virtual object_ref lookup(const std::string& id) = 0;
		virtual void enumerate(std::function<void(const std::string&, object_ref)>) = 0;
	};



}