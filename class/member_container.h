#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include "meta_type.h"
#include "sequence.h"

namespace up {
	
	class member_container {
	public:
		using unordered_member_container = std::unordered_map< std::string, member* >;
		using indexed_member_container = std::vector< std::unique_ptr<member> >;

		using member_iterator = boost::indirect_iterator< indexed_member_container::const_iterator >;
		using member_sequence = up::sequence< member_iterator >;

		member_container() = default;
		member_container(member_container&&) = default;


		member_sequence members() const;
		member* mem(const std::string&) const;

		void declare_member(std::unique_ptr<member> mem_ptr);

	protected:
		unordered_member_container lookup_members_;
		indexed_member_container indexed_members_;
	};

	class static_member_container
		: public member_container
	{
	public:
		template< typename Member0, typename Enabled = std::enable_if<std::is_convertible<Member0, member>::value >::type, typename... Members >
		static_member_container(Member0&& mem0, Members&&... members)
		{
			declare_members(std::forward<Member0>(mem0), std::forward<Members>(members)...);
		}

		static_member_container() = default;
		static_member_container(const static_member_container&) = default;
		static_member_container(static_member_container&&) = default;
		
		template< typename Member0, typename... Members >
		void declare_members(Member0&& mem0, Members&&... members)
		{
			declare_member(std::make_unique<std::decay_t<Member0>>(std::forward<Member0>(mem0)));
			declare_members(std::forward<Members>(members)...);
		}

		void declare_members()
		{
		}


	};



}
