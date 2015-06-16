#include "member_container.h"

namespace up {

	member_container::member_sequence member_container::members() const
	{
		return member_sequence(indexed_members_.begin(), indexed_members_.end());
	}

	member* member_container::mem(const std::string& str) const
	{
		auto it = lookup_members_.find(str);
		return (it != lookup_members_.end()) ? it->second : nullptr;
	}

	void member_container::declare_member(std::unique_ptr<member> mem_ptr)
	{
		auto ptr = mem_ptr.get();
		indexed_members_.emplace_back(std::move(mem_ptr));
		lookup_members_.emplace(ptr->id(), ptr);
	}

}