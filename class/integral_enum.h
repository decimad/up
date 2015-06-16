#pragma once

#include "static_type.h"
#include "events.h"

namespace up {

	class enumeration_set;

	class enumeration_member {
	public:
		enumeration_member(const enumeration_set* myset, std::string label__, size_t index__)
			: myset_(myset), label_(std::move(label__)), index_(index__)
		{}

		enumeration_member(enumeration_member&&) = default;

		const enumeration_set* set() const {
			return myset_;
		}

		const std::string& label() const {
			return label_;
		}

		size_t index()  const {
			return index_;
		}

	private:
		const enumeration_set* myset_;
		std::string label_;
		size_t index_;
	};


	class enumeration_set {
	public:
		using container_type = std::vector<std::unique_ptr<enumeration_member>>;
		using iterator = boost::indirect_iterator<container_type::const_iterator>;

		using sequence_type = up::sequence<iterator>;

		void declare_member(std::string label)
		{
			members_.emplace_back(std::make_unique<enumeration_member>(this, std::move(label), members_.size()));
		}

		sequence_type members() {
			return sequence_type(members_.begin(), members_.end());
		}

		iterator begin() const {
			return members_.begin();
		}

		iterator end() const {
			return members_.end();
		}

	private:
		container_type members_;
	};

	struct enumeration : public up::events_holder_t<enumeration> {
		virtual const enumeration_member& get() const = 0;
		virtual void set(const enumeration_member&) = 0;

		up::holder_signal<void(const enumeration_member&)> changing;
		up::holder_signal<void(const enumeration_member&)> changed;

		virtual const enumeration_set& values() const = 0;

		up::holder_signal<void()> enumeration_set_changed;
	};

	namespace types {
		using enum_type = static_class<enumeration>;
	}

	const types::enum_type&    get_meta_type(const enumeration*);

}