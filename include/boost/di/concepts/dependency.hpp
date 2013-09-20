//
// Copyright (c) 2012-2013 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_CONCEPTS_DEPENDENCY_HPP
#define BOOST_DI_CONCEPTS_DEPENDENCY_HPP

#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/has_xxx.hpp>

#include <boost/di/scopes/deduce.hpp>
#include <boost/di/scopes/external.hpp>
#include <boost/di/type_traits/make_plain.hpp>
#include <boost/di/convertibles/convertible_ref.hpp>
#include <boost/di/convertibles/convertible_shared.hpp>
#include "boost/di/config.hpp"

namespace boost {
namespace di {
namespace concepts {

namespace detail {

template<typename T>
struct scope_traits
{
    typedef T type;
};

template<>
struct scope_traits<mpl::_1>
{
    typedef scopes::deduce type;
};

BOOST_MPL_HAS_XXX_TRAIT_DEF(named_type)

template<typename T, typename = void>
struct named_traits
{
    typedef T type;
};

template<typename T>
struct named_traits<T, typename enable_if<has_named_type<T> >::type>
{
    typedef typename type_traits::remove_accessors<
        typename T::named_type
    >::type type;
};

} // namespace detail

template<
    typename TScope
  , typename TExpected
  , typename TGiven = TExpected
  , typename TContext = mpl::vector0<>
  , typename TBind = typename mpl::lambda<
        is_same<mpl::_1, TExpected>
    >::type
>
class dependency
    : public detail::scope_traits<TScope>::type::template
          scope<typename detail::named_traits<TExpected>::type, TGiven>
{
    typedef typename detail::scope_traits<TScope>::type::template
        scope<typename detail::named_traits<TExpected>::type, TGiven> scope_type;

    template<
        typename T
      , typename U
      , template<typename> class TConvertible
    >
    struct external
    {
        typedef dependency<
            scopes::external<TConvertible>,
            T
          , U
          , TContext
          , TBind
        > type;
    };

public:
    typedef dependency type;
    typedef typename detail::scope_traits<TScope>::type scope;
    typedef TExpected expected;
    typedef TGiven given;
    typedef TContext context;
    typedef TBind bind;

    dependency() { }

    template<typename T>
    explicit dependency(const T& obj)
        : scope_type(obj)
    { }

    template<typename T>
    explicit dependency(T& obj)
        : scope_type(obj)
    { }

    template<typename T>
    explicit dependency(shared_ptr<T> obj)
        : scope_type(obj)
    { }

    template<typename T>
    struct rebind
    {
        typedef dependency<
            typename mpl::if_<
                is_same<scope, scopes::deduce>
              , T
              , scope
            >::type
          , TExpected
          , TGiven
          , TContext
          , TBind
        > other;
    };

    template<typename T>
    static typename external<T, TExpected, convertibles::convertible_value>::type
    to(const T& obj, typename enable_if<is_arithmetic<T> >::type* = 0) {
        return typename external<T, TExpected, convertibles::convertible_value>::type(obj);
    }

    template<typename T>
    static typename external<T, const TExpected, convertibles::convertible_ref>::type
    to(const T& obj, typename disable_if<is_arithmetic<T> >::type* = 0) {
        return typename external<T, const TExpected, convertibles::convertible_ref>::type(obj);
    }

    template<typename T>
    static typename external<T, TExpected, convertibles::convertible_ref>::type
    to(T& obj) {
        return typename external<T, TExpected, convertibles::convertible_ref>::type(obj);
    }

    template<typename T>
    static typename external<T, TExpected, convertibles::convertible_shared>::type
    to(shared_ptr<T> obj) {
        return typename external<T, TExpected, convertibles::convertible_shared>::type(obj);
    }
};

} // namespace concepts
} // namespace di
} // namespace boost

#endif

