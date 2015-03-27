#include <iostream>
#include <cassert>
#include "integral_proxies.h"
#include "introspection.h"
#include "casting.h"

struct some_base {
	virtual void foo() = 0;
	int some;
};

using some_base_meta = up::static_class<some_base>;
some_base_meta some_base_class("up.some_base_meta");
const some_base_meta& get_meta_class(const some_base*) { return some_base_class; }

struct some_base2 {
	virtual void bar() const = 0;
	int some;
};

using some_base2_meta = up::static_class<some_base>;

some_base2_meta some_base2_class("up.some_base2_meta");
const some_base2_meta& get_meta_class(const some_base2*) { return some_base2_class; }

struct short_tag {};

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

	short short_;
	float float_;
};

extern const char short_id[] = "short_val";
extern const char float_id[] = "float_val";

using short_member = up::static_member< short_id, up::adapters::method_integer<sample_struct, short, &sample_struct::get_short, &sample_struct::set_short, true > >;
using float_member = up::static_member< float_id, up::adapters::method_real   <sample_struct, float, &sample_struct::get_float, &sample_struct::set_float, true > >;

using sample_struct_meta = up::static_class< sample_struct, up::bases<some_base, some_base2>,
	short_member,
	float_member
>;

void sample_struct::set_short(short value) {
//	short_member::raise(this, &up::integer::changing, value);
	short_ = value;
//	short_member::raise(this, &up::integer::changed, value);
}

sample_struct_meta sample_struct_class("up.sample_struct");
const sample_struct_meta& get_meta_class(const sample_struct*) { return sample_struct_class; }

struct sample_struct2 {};
using sample_struct_meta2 = up::static_class< sample_struct2, up::bases<> >;
sample_struct_meta2 sample_struct2_class("up.sample_struct2");
const sample_struct_meta2& get_meta_class(const sample_struct2*) { return sample_struct2_class; }


void user_func(const up::mutable_object_ref& ref)
{
	auto num_members = ref.num_members();
	for(size_t i = 0; i < num_members; ++i) {
		std::cout<<"member \""<<ref.member_id(i)<<"\"\n";
	}

	auto typed_ref = up::cast<up::integer>(ref.get_member(0));
	assert(typed_ref);
	std::cout << "limits: {" << typed_ref->limits().first << "," << typed_ref->limits().second << "}\n";

	auto link = typed_ref->changed.subscribe([](int value) { std::cout<<"Value changed to: "<<value<<"\n"; } );
	typed_ref->set(22);
	typed_ref.reset();
	typed_ref = up::cast<up::integer>(ref.get_member(0));

	std::cout<<"bases: \n";
	up::bases_breadth_first(ref.get_class(), [](const up::class_* cl) {
		std::cout<<cl->get_name()<<"\n";
	});

}

void class_test() {
	sample_struct sample;
	sample_struct2 sample2;
	some_base& base = sample;

	auto ref = up::make_ref(base);

	auto typed = up::cast_full<sample_struct>(ref);
	assert(typed);
	typed->set_float(4.0);
	typed->bar();

	user_func(up::make_ref(sample));
}