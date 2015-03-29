#pragma once

namespace meta {

	template< typename... Elements >
	struct list {};

	//
	// union_list
	//
	template< typename... Lists >
	struct union_list;

	template< template< typename... > class ListType, typename... ListA_Args, typename... ListB_Args, typename... Lists >
	struct union_list< ListType<ListA_Args...>, ListType<ListB_Args...>, Lists... >
	{
		using type = typename union_list< ListType< ListA_Args..., ListB_Args... >, Lists... >::type;
	};

	template< template< typename... > class ListType, typename... List_Args >
	struct union_list< ListType<List_Args...> >
	{
		using type = ListType<List_Args...>;
	};

	template< >
	struct union_list< >
	{
		//static_assert(false, "Empty union not allowed");
	};

	template< typename... Lists >
	using union_list_t = typename union_list< Lists... >::type;

	//
	// transform_list: Src!{SrcElems...} -> Dest!{UnaryFunction(SrcElems)...}
	//
	namespace detail {
		template< typename... Elems >
		struct auto_list {};
	}

	template< typename SrcList, template< typename > class UnaryFunction, template< typename... > class DestListType = detail::auto_list >
	struct transform_list;

	template< template< typename... > class SrcListType, typename... SrcListElems, template< typename > class UnaryFunction, template< typename... > class DestListType >
	struct transform_list< SrcListType<SrcListElems...>, UnaryFunction, DestListType >
	{
		using type = DestListType< typename UnaryFunction<SrcListElems>::type... >;
	};

	template< template< typename... > class SrcListType, typename... SrcListElems, template< typename > class UnaryFunction >
	struct transform_list< SrcListType<SrcListElems...>, UnaryFunction, detail::auto_list >
	{
		using type = SrcListType< typename UnaryFunction<SrcListElems>::type... >;
	};

	template< typename SrcList, template<typename> class Transformer, template< typename... > class DestListType = detail::auto_list >
	using transform_list_t = typename transform_list< SrcList, Transformer, DestListType >::type;

	//
	// apply: Src!{SrcElems...} -> F(SrcElems...)
	//
	template< typename SrcList, template<typename...> class VariadicFunction >
	struct apply;

	template< template<typename...> class SrcList, typename... SrcElements, template< typename... > class VariadicFunction >
	struct apply< SrcList<SrcElements...>, VariadicFunction >
	{
		using type = typename VariadicFunction<SrcElements...>::type;
	};

	//
	// apply_t: Src!{SrcElems...} -> F(SrcElems...)
	//
	template< typename SrcList, template<typename...> class VariadicFunction >
	using apply_t = typename apply< SrcList, VariadicFunction >::type;

}