#include <iostream>
#include <cassert>
#include "adapters.h"
#include "introspection.h"
#include "casting.h"

struct some_base {
	virtual void foo() = 0;
	int some;
};

using some_base_meta = up::static_class<some_base>;
some_base_meta some_base_class("up.some_base_meta");
const some_base_meta& get_meta_type(const some_base*) { return some_base_class; }

struct some_base2 {
	virtual void bar() const = 0;
	int some;
};

using some_base2_meta = up::static_class<some_base>;

some_base2_meta some_base2_class("up.some_base2_meta");
const some_base2_meta& get_meta_type(const some_base2*) { return some_base2_class; }

struct short_tag {};


enum sample_enum {
	Value1,
	Value2
};

using up::adapters::enum_field;

auto sample_enum_set = up::adapters::static_enumeration_set<sample_enum>(
	enum_field(Value1, "Value1"),
	enum_field(Value2, "Value2")
	);

struct sample_struct : public some_base, public some_base2
{
	short get_short() const {
		return short_;
	}

	void set_short(short value);

	float get_float() const {
		return float_;
	}

	void set_float(float value) {
		float_ = value;
	}

	void foo() override {

	}

	void bar() const override {
		std::cout<<"bar called! "<<float_<<"\n";
	}

	void some() {
		short_ = 4;

	}

	sample_enum get_enum() const {
		return enum_;
	}

	void set_enum(sample_enum value) {
		enum_ = value;
	}
	
	sample_enum enum_;
	short short_;
	float float_;
};

namespace {
	using short_member = up::adapters::static_method_integer<sample_struct, short, &sample_struct::get_short, &sample_struct::set_short, nullptr, true >;
	using float_member = up::adapters::method_real   <sample_struct, float, &sample_struct::get_float, &sample_struct::set_float, true >;
	using enum_member = up::adapters::method_enum< sample_struct, sample_enum, &sample_struct::get_enum, &sample_struct::set_enum, true >;
}

//using sample_struct_meta = up::static_class< sample_struct >;
//const sample_struct_meta& get_meta_type(const sample_struct*) { return sample_struct_class; }

auto sample_struct_class = up::class_<sample_struct>("sample_struct")
	.bases<some_base, some_base2>()
	.member(short_member("short_val", up::member::visibility_types::Public))
	.member(float_member("float_val", up::member::visibility_types::Public))
	.member(enum_member("enum_val", up::member::visibility_types::Public, &sample_enum_set))
	.member(
		up::adapters::static_group<sample_struct, 0>("my_group", up::member::visibility_types::Public)
		.member(
			up::adapters::static_group<sample_struct, 1>("nested_group", up::member::visibility_types::Public)
			.member(short_member("short_val", up::member::visibility_types::Public))
			.member(float_member("float_val", up::member::visibility_types::Public))
			.member(enum_member("enum_val", up::member::visibility_types::Public, &sample_enum_set))
			)
		.member(short_member("short_val", up::member::visibility_types::Public))
		.member(float_member("float_val", up::member::visibility_types::Public))
		.member(enum_member("enum_val", up::member::visibility_types::Public, &sample_enum_set))
		);

EXPOSE_CLASS(sample_struct);

void sample_struct::set_short(short value) {
	short_member::raise(this, &up::integer::changing, value);
	short_ = value;
	short_member::raise(this, &up::integer::changed, value);
}

struct sample_struct2 {};
using sample_struct_meta2 = up::static_class< sample_struct2 >;
auto sample_struct2_class = sample_struct_meta2("up.sample_struct2");
const sample_struct_meta2& get_meta_type(const sample_struct2*) { return sample_struct2_class; }

void user_func(const up::ref<void>& ref)
{
	auto typed_ref = ref.members()[0].as<up::integer>();
	assert(typed_ref);
	std::cout << "limits: {" << typed_ref->limits().first << "," << typed_ref->limits().second << "}\n";

	auto link = typed_ref->changed.subscribe([](int value) { std::cout << "Value changed to: " << value << "\n"; });

	for (auto& member : ref.type()->members()) {
		std::cout << member.id() << " (" << member.type().id() << ")\n";
	}

	ref.for_each<up::group>([](auto group) {
		group.for_each<up::group>([](auto group) {
			group.if_<up::integer>([](auto integer) {
				integer->set(44);
			});
		});
	});
	
	typed_ref->set(22);
	typed_ref.reset();
	typed_ref = up::cast<up::integer>(ref.members()[0]);

	std::cout << "object type: " << ref.type()->id() << "\n";
	std::cout << "fragments: \n";
	for (auto& frag : ref.type()->fragments()) {
		std::cout << frag.name() << "\n";
	}

	std::cout << "enum values:\n";
	auto typed_ref2 = up::cast<up::enumeration>(ref.members()[2]);

	auto link2 = typed_ref2->changed.subscribe([](const up::enumeration_member& mem) {
		std::cout << "Enum changed to: " << mem.label() << "\n";
	});

	for (const auto& val : typed_ref2->values()) {
		std::cout << "* " << val.label() << "\n";
		typed_ref2->set(val);
	}



}

void typetest() {
	const auto* ptr = &up::class_<sample_struct>::get_instance();

	sample_struct sample;
	some_base& base = sample;

	auto ref = up::make_managed<sample_struct>();
	auto ref2 = up::cast<some_base>(ref);

	assert(ref2);

	auto void_ref = up::cast<void>(ref);
	auto typed_ref = up::cast<sample_struct>(void_ref);

	assert(typed_ref);

	ref->set_float(4.0);
	ref->bar();

	user_func(up::make_ref(sample));
}