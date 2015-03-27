#include <unordered_map>
#include "type.h"

namespace up {
	
	std::unordered_map< std::string, const type* >& get_name_map() {
		static std::unordered_map< std::string, const type* > the_map;
		return the_map;
	}

	std::unordered_map< const type_info*, const type* >& get_info_map() {
		static std::unordered_map< const type_info*, const type* > the_map;
		return the_map;
	}

	void register_type(std::string id, const type* ptr) {
		get_name_map().emplace(id, ptr);
	}

	void register_type(const type_info& info, const type* ptr) {
		get_info_map().emplace(&info, ptr);
	}

	const type* lookup_type(const char* name) {
		auto& map = get_name_map();
		auto it = map.find(name);
		if(it!=map.end()) {
			return it->second;
		} else {
			return nullptr;
		}
	}

	const type* lookup_type(const type_info& info) {
		auto& map = get_info_map();
		auto it = map.find(&info);
		if(it!=map.end()) {
			return it->second;
		} else {
			return nullptr;
		}
	}

}



