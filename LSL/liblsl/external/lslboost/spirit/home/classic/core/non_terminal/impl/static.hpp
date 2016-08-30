/*=============================================================================
    Copyright (c) 2006 Joao Abecasis
    http://spirit.sourceforge.net/

  Distributed under the Boost Software License, Version 1.0. (See accompanying
  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_STATIC_HPP)
#define BOOST_SPIRIT_STATIC_HPP

#include <lslboost/noncopyable.hpp>
#include <lslboost/call_traits.hpp>
#include <lslboost/aligned_storage.hpp>

#include <lslboost/type_traits/add_pointer.hpp>
#include <lslboost/type_traits/alignment_of.hpp>

#include <lslboost/thread/once.hpp>

#include <memory>   // for placement new

#include <lslboost/spirit/home/classic/namespace.hpp>

namespace lslboost { namespace spirit {

BOOST_SPIRIT_CLASSIC_NAMESPACE_BEGIN

    //
    //  Provides thread-safe initialization of a single static instance of T.
    //
    //  This instance is guaranteed to be constructed on static storage in a
    //  thread-safe manner, on the first call to the constructor of static_.
    //
    //  Requirements:
    //      T is default constructible
    //          (There's an alternate implementation that relaxes this
    //              requirement -- Joao Abecasis)
    //      T::T() MUST not throw!
    //          this is a requirement of lslboost::call_once.
    //
    template <class T, class Tag>
    struct static_
        : lslboost::noncopyable
    {
    private:

        struct destructor
        {
            ~destructor()
            {
                static_::get_address()->~value_type();
            }
        };

        struct default_ctor
        {
            static void construct()
            {
                ::new (static_::get_address()) value_type();
                static destructor d;
            }
        };

    public:

        typedef T value_type;
        typedef typename lslboost::call_traits<T>::reference reference;
        typedef typename lslboost::call_traits<T>::const_reference const_reference;

        static_(Tag = Tag())
        {
            lslboost::call_once(&default_ctor::construct, constructed_);
        }

        operator reference()
        {
            return this->get();
        }

        operator const_reference() const
        {
            return this->get();
        }

        reference get()
        {
            return *this->get_address();
        }

        const_reference get() const
        {
            return *this->get_address();
        }

    private:
        typedef typename lslboost::add_pointer<value_type>::type pointer;

        static pointer get_address()
        {
            return static_cast<pointer>(data_.address());
        }

        typedef lslboost::aligned_storage<sizeof(value_type),
            lslboost::alignment_of<value_type>::value> storage_type;

        static storage_type data_;
        static once_flag constructed_;
    };

    template <class T, class Tag>
    typename static_<T, Tag>::storage_type static_<T, Tag>::data_;

    template <class T, class Tag>
    once_flag static_<T, Tag>::constructed_ = BOOST_ONCE_INIT;

BOOST_SPIRIT_CLASSIC_NAMESPACE_END

}} // namespace BOOST_SPIRIT_CLASSIC_NS

#endif // include guard
