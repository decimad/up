#include <unordered_map>
#include "class.h"

namespace up {
	
	std::unordered_map< std::string, const class_* >& get_name_map() {
		static std::unordered_map< std::string, const class_* > the_map;
		return the_map;
	}

	std::unordered_map< const type_info*, const class_* >& get_info_map() {
		static std::unordered_map< const type_info*, const class_* > the_map;
		return the_map;
	}

	void register_class(std::string id, const class_* ptr) {
		get_name_map().emplace(id, ptr);
	}

	void register_class(const type_info& info, const class_* ptr) {
		get_info_map().emplace(&info, ptr);
	}

	const class_* lookup_class(const char* name) {
		auto& map = get_name_map();
		auto it = map.find(name);
		if(it!=map.end()) {
			return it->second;
		} else {
			return nullptr;
		}
	}

	const class_* lookup_class(const type_info& info) {
		auto& map = get_info_map();
		auto it = map.find(&info);
		if(it!=map.end()) {
			return it->second;
		} else {
			return nullptr;
		}
	}

}



