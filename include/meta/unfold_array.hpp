#include "numbers.hpp"

namespace meta {

template <typename ArrayT>
concept Accesible = requires (ArrayT array, Index i){array[i];};

template <Accesible ArrayT, Index i>
struct GetElem { static auto get(ArrayT ptr) { return ptr[i]; } };

template <Index Offset, typename Funtion, Accesible ArrayT, size_t... Indices>
auto _unfoldAndCall_impl(Funtion f, ArrayT array, std::index_sequence<Indices...>)
{
    return f(GetElem<ArrayT, Offset + Indices>::get(array)...);
}

template <Index Size, Index Offset = 0,  typename Funtion, Accesible ArrayT>
auto unfoldAndCall(Funtion f, ArrayT array)
{
    return _unfoldAndCall_impl<Offset>(f, array, std::make_index_sequence<Size>{});
}

template <class Class, Index Offset, Accesible ArrayT, size_t... Indices>
auto _unfoldAndConstruct_impl([[maybe_unused]] ArrayT array, std::index_sequence<Indices...>) 
{
    return Class(GetElem<ArrayT, Offset + Indices>::get(array)...);
}

template <class Class, Index Size, Index Offset = 0, Accesible ArrayT>
auto unfoldAndConstruct(ArrayT ptr)
{
    return _unfoldAndConstruct_impl<Class, Offset>(ptr, std::make_index_sequence<Size>{});
}

} //namespace meta