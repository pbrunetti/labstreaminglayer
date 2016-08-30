/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_BEGIN_IMPL_20060123_2147)
#define FUSION_BEGIN_IMPL_20060123_2147

#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/view/zip_view/zip_view_iterator_fwd.hpp>
#include <lslboost/fusion/algorithm/transformation/transform.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/type_traits/is_reference.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/mpl/assert.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/fusion/support/unused.hpp>

namespace lslboost { namespace fusion {

    struct zip_view_tag;

    namespace detail
    {
        struct poly_begin
        {
            template<typename T>
            struct result;

            template<typename SeqRef>
            struct result<poly_begin(SeqRef)>
                : mpl::eval_if<is_same<SeqRef, unused_type const&>,
                               mpl::identity<unused_type>,
                               result_of::begin<typename remove_reference<SeqRef>::type> >
            {
                BOOST_MPL_ASSERT((is_reference<SeqRef>));
            };

            template<typename Seq>
            typename result<poly_begin(Seq&)>::type
            operator()(Seq& seq) const
            {
                return fusion::begin(seq);
            }

            template<typename Seq>
            typename result<poly_begin(Seq const&)>::type
            operator()(Seq const& seq) const
            {
                return fusion::begin(seq);
            }

            unused_type operator()(unused_type const&) const
            {
                return unused_type();
            }
        };
    }

    namespace extension
    {
        template<typename Tag>
        struct begin_impl;

        template<>
        struct begin_impl<zip_view_tag>
        {
            template<typename Sequence>
            struct apply
            {
                typedef zip_view_iterator<
                    typename result_of::transform<typename Sequence::sequences, detail::poly_begin>::type,
                    typename Sequence::category> type;

                static type
                call(Sequence& sequence)
                {
                    return type(
                        fusion::transform(sequence.sequences_, detail::poly_begin()));
                }
            };


            
        };
    }
}}

#endif
