#include <unordered_map>
#include "meta_type.h"
#include "reference.h"

namespace up {
	
	std::unordered_map< std::string, const meta_type* >& get_name_map() {
		static std::unordered_map< std::string, const meta_type* > the_map;
		return the_map;
	}

	std::unordered_map< const type_info*, const meta_type* >& get_info_map() {
		static std::unordered_map< const type_info*, const meta_type* > the_map;
		return the_map;
	}

	void register_type(std::string id, const meta_type* ptr) {
		get_name_map().emplace(id, ptr);
	}

	void register_type(const type_info& info, const meta_type* ptr) {
		get_info_map().emplace(&info, ptr);
	}

	const meta_type* lookup_type(const char* name) {
		auto& map = get_name_map();
		auto it = map.find(name);
		if(it!=map.end()) {
			return it->second;
		} else {
			return nullptr;
		}
	}

	const meta_type* lookup_type(const type_info& info) {
		auto& map = get_info_map();
		auto it = map.find(&info);
		if(it!=map.end()) {
			return it->second;
		} else {
			return nullptr;
		}
	}

	member::member(std::string id, type_ref type, visibility_types vis)
		: id_(std::move(id)), type_(&type), vis_(vis)
	{
	}

	const std::string & member::id() const
	{
		return id_;
	}

	type_ref member::type() const
	{
		return *type_;
	}

	member::visibility_types member::visibility() const
	{
		return vis_;
	}

	const std::string& type_fragment::name() const {
		return type().id();
	}

	meta_type::meta_type()
	{
	}

	meta_type::meta_type(std::string Id)
		: id_(std::move(Id))
	{
		register_type(id_, this);
	}
	
	void meta_type::declare_fragment(std::unique_ptr<type_fragment> frag_ptr)
	{
		fragments_.emplace_back(std::move(frag_ptr));
	}

	void meta_type::declare_member(std::unique_ptr<member> member_ptr)
	{
		indexed_members_.emplace_back(std::move(member_ptr));
	}

	meta_type::fragment_sequence meta_type::fragments() const
	{
		return fragment_sequence(fragment_iterator(fragments_.begin()), fragment_iterator(fragments_.end()));
	}

	meta_type::member_sequence meta_type::members() const
	{
		return member_sequence(member_iterator(indexed_members_.begin()), member_iterator(indexed_members_.end()));
	}

	ref<void> member::bind(const ref<void>& myref) const {
		return ref_const_cast(bind(static_cast<const ref<const void>& >(myref)));
	}

}



