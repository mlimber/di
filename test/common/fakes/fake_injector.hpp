//
// Copyright (c) 2014 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_FAKE_INJECTOR_HPP
#define BOOST_DI_FAKE_INJECTOR_HPP

#include <typeinfo>
#include "boost/di/aux_/type_traits.hpp"

namespace boost { namespace di {

template<class TExpected = void>
struct fake_injector {
    template<class T, class TParent>
    decltype(auto) create() const noexcept {
        expect_eq(typeid(TExpected).name(), typeid(T).name());
        static aux::remove_accessors_t<T> object;
        return object;
    }
};

}} // boost::di

#endif

