/*
  Copyright (c) 2021, Triad National Security, LLC
  All rights reserved
 */

#pragma once

#include <cstddef>

#include <flecsi/topo/unstructured/interface.hh>


namespace fsp {

template<std::size_t Dimension, bool IncludeExtras = false>
struct config_base : flecsi::topo::help {

  //! Type for sizes
  using size = std::size_t;
  //! Floating-point type
  using real = double;

  static constexpr size num_dimensions = Dimension;
  static constexpr bool include_extras = IncludeExtras;


  struct detail {
    template<class Pre, class Lst>
    struct list_prepend;

    template<class Pre, class ... LstElem>
    struct list_prepend<Pre, list<LstElem...>>
    {
      using value = list<Pre, LstElem...>;
    };


    /**
     * \tparam H source value for connectivity
     * \tparam Before type list of index spaces before H
     * \tparam After type list of index spaces after H
     */
    template<auto H, class Before, class After> struct connect_full;

    /**
     * Base Case 1: Before is empty and H is first element in type list
     */
    template<auto H, auto AfterHead, auto ... AfterTail>
    struct connect_full<H, has<>, has<AfterHead, AfterTail...>>
    {
      using type = typename list_prepend<from<H, to<AfterHead, AfterTail...>>,
                                         typename connect_full<AfterHead, has<H>, has<AfterTail...>>::type>::value;
    };

    /**
     * General case: Before and After are non-empty and H is in interior of
     * type list
    */
    template<auto H, auto ... Before, auto AfterHead, auto ... AfterTail>
    struct connect_full<H, has<Before...>, has<AfterHead, AfterTail...>>
    {
      using type = typename list_prepend<from<H, to<Before..., AfterHead, AfterTail...>>,
                                         typename connect_full<AfterHead, has<Before..., H>, has<AfterTail...>>::type>::value;
    };

    /**
     * Base Case 2: After is empty and H is last element in type list
     */
    template<auto H, auto ...Before>
    struct connect_full<H, has<Before...>, has<>>
    {
      using type = list<from<H, to<Before...>>>;
    };
  };


  template <class T> struct fully_connected;
  template<auto H, auto ... V>
  struct fully_connected<has<H, V...>>
  {
    using type = typename detail::template connect_full<H, has<>, has<V...>>::type;
  };
};


template<std::size_t Dimension, bool IncludeExtras = false>
struct config;

} // namespace fsp
