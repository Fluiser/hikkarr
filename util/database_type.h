#pragma once
#include <boost/preprocessor.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/bind.hpp>
#include <dpp/dpp.h>
#include "type.h"

#define REM(...) __VA_ARGS__
#define EAT(...)

// Retrieve the type
#define TYPEOF(x) DETAIL_TYPEOF(DETAIL_TYPEOF_PROBE x,)
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_HEAD(x, ...) REM x
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),
// Strip off the type
#define STRIP(x) EAT x
// Show the type without parenthesis
#define PAIR(x) REM x

template<class M, class T>
struct make_const
{
    typedef T type;
};

template<class M, class T>
struct make_const<const M, T>
{
    typedef typename boost::add_const<T>::type type;
};


#define REFLECTABLE(...) \
static const int fields_n = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
    friend struct reflector; \
    template<int N, class Self> \
    struct field_data {}; \
    BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define REFLECT_EACH(r, data, i, x) \
    PAIR(x); \
    template<class Self> \
    struct field_data<i, Self> \
{ \
        Self & self; \
        field_data(Self & self) : self(self) {} \
    \
        typename make_const<Self, TYPEOF(x)>::type & get() \
    { \
            return self.STRIP(x); \
    }\
        typename boost::add_const<TYPEOF(x)>::type & get() const \
    { \
            return self.STRIP(x); \
    }\
        const char * name() const \
    {\
            return BOOST_PP_STRINGIZE(STRIP(x)); \
    } \
};

struct reflector
{
    //Get field_data at index N
    template<int N, class T>
    static typename T::template field_data<N, T> get_field_data(T& x)
    {
        return typename T::template field_data<N, T>(x);
    }

    // Get the number of fields
    template<class T>
    struct fields
    {
        static const int n = T::fields_n;
    };
};

struct field_visitor
{
    template<class C, class Visitor, class T>
    void operator()(C& c, Visitor v, T)
    {
        v(reflector::get_field_data<T::value>(c));
    }
};

template<class C, class Visitor>
void visit_each(C & c, Visitor v)
{
    typedef boost::mpl::range_c<int,0,reflector::fields<C>::n> range;
    boost::mpl::for_each<range>(boost::bind<void>(field_visitor(), boost::ref(c), v, _1));
}

struct profile {
    REFLECTABLE(
        (dpp::snowflake) id,
        (i32) level,
        (i32) xp,
        (f32) reputation,
        (f32) money,
        (std::string) about,
        //statistics
        (u64) messages
    )
};
struct guild_info {
    REFLECTABLE(
        (dpp::snowflake) id,
        (u64) messages
    )
};

