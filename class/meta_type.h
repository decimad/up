#pragma once
#include <cstddef>
#include <typeinfo>
#include <tuple>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include <unordered_map>

//#include "reference.h"
#include <boost/iterator/indirect_iterator.hpp>
#include "sequence.h"


namespace up {

	class meta_type;

	template< typename T >
	class ref;

	template< typename Dest, typename Src >
	ref<Dest> cast(const ref<Src>&);


	using type_ref = const meta_type&;
	using type_ptr = const meta_type*;

	class member
	{
	public:
		enum class visibility_types {
			Public,
			Protected,
			Private
		};

		member(std::string id, type_ref type, visibility_types vis);
		member(const member&) = default;
		member(member&&) = default;


		const std::string& id() const;
		type_ref type() const;
		visibility_types visibility() const;
		
		// Bind with container object storage duration
		virtual ref<const void> bind(const ref<const void>&) const = 0;
		
		ref<void> bind(const ref<void>& myref) const;

		template<typename Interface>
		ref<Interface> as(const ref<void>& myref) const {
			return cast<Interface>(bind(myref));
		}

		template<typename Interface>
		bool is() const {
			return &type() == lookup_meta_type<Interface>();
		}
				
		virtual ~member() {}

	private:
		std::string id_;
		type_ptr type_;
		visibility_types vis_;
	};

	class type_fragment {
	public:
		virtual ~type_fragment() {}

		virtual const std::string& name() const = 0;		
		virtual const meta_type& type() const = 0;
		
		virtual ref<void> bind(const ref<void> /*fragment_ref*/ & obj) = 0;
		virtual ref<const void> bind(const ref<const void> /*const_fragment_ref*/& obj) = 0;
	};

	class meta_type {
	public:
		meta_type();
		meta_type(std::string Id);

		using fragment_container = std::vector< std::unique_ptr<type_fragment> >;
		using unordered_member_container = std::unordered_map< std::string, member* >;
		using indexed_member_container = std::vector< std::unique_ptr<member> >;

	protected:
		/* hack */
	public:
		void declare_member(std::unique_ptr<member> mem_ptr);
		unordered_member_container lookup_members_;
		indexed_member_container indexed_members_;

		void declare_fragment(std::unique_ptr<type_fragment> frag_ptr);
		fragment_container fragments_;
	
	public:
		// V2
		using fragment_iterator = boost::indirect_iterator< fragment_container::const_iterator >;
		using fragment_sequence = up::sequence< fragment_iterator >;

		fragment_sequence fragments() const;

		using member_iterator = boost::indirect_iterator< indexed_member_container::const_iterator >;
		using member_sequence = up::sequence< member_iterator >;

		member_sequence members() const;

		const std::string& id() const { return id_; }

		/* hack */
		void set_name(std::string str)
		{
			id_ = str;
		}

	private:
		std::string id_;
	};
		
	void register_type(std::string id, const meta_type* ptr);
	void register_type(const type_info&, const meta_type* ptr);

	type_ptr lookup_type(const char* name);
	type_ptr lookup_type(const type_info& info);

}
