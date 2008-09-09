// Copyright (c) 1999,2007  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
//
//
// Author(s)     : Andreas Fabri, Michael Hemmer

#ifndef CGAL_LEDA_RATIONAL_H
#define CGAL_LEDA_RATIONAL_H

#include <CGAL/number_type_basic.h>

#ifdef CGAL_USE_LEDA

#include <CGAL/leda_coercion_traits.h>
#include <CGAL/Interval_nt.h>

#include <CGAL/Residue.h>
#include <CGAL/Modular_traits.h>

#include <CGAL/Needs_parens_as_product.h>

#include <utility>

#include <CGAL/LEDA_basic.h>
#if CGAL_LEDA_VERSION < 500
#include <LEDA/rational.h>
#include <LEDA/interval.h>
#else
#include <LEDA/numbers/rational.h>
#include <LEDA/numbers/interval.h>
#endif

#include <CGAL/leda_integer.h> // for GCD in Fraction_traits

CGAL_BEGIN_NAMESPACE

template <> class Algebraic_structure_traits< leda_rational >
  : public Algebraic_structure_traits_base< leda_rational,
                                            Field_tag >  {
  public:
    typedef Tag_true            Is_exact;
    typedef Tag_false           Is_numerical_sensitive;

//    TODO: How to implement this without having sqrt?
//    typedef INTERN_AST::Is_square_per_sqrt< Type >
//                                                                 Is_square;

    class Simplify
      : public std::unary_function< Type&, void > {
      public:
        void operator()( Type& x) const {
            x.normalize();
        }
    };

};

template <> class Real_embeddable_traits< leda_rational >
  : public INTERN_RET::Real_embeddable_traits_base< leda_rational , CGAL::Tag_true > {
  public:
  
    class Abs
      : public std::unary_function< Type, Type > {
      public:
        Type operator()( const Type& x ) const {
            return CGAL_LEDA_SCOPE::abs( x );
        }
    };

    class Sign
      : public std::unary_function< Type, ::CGAL::Sign > {
      public:
        ::CGAL::Sign operator()( const Type& x ) const {
            return (::CGAL::Sign) CGAL_LEDA_SCOPE::sign( x );
        }
    };

    class Compare
      : public std::binary_function< Type, Type,
                                Comparison_result > {
      public:
        Comparison_result operator()( const Type& x,
                                      const Type& y ) const {
          return (Comparison_result) CGAL_LEDA_SCOPE::compare( x, y );
        }
        CGAL_IMPLICIT_INTEROPERABLE_BINARY_OPERATOR_WITH_RT(Type,Comparison_result)
    };

    class To_double
      : public std::unary_function< Type, double > {
      public:
        double operator()( const Type& x ) const {
          return x.to_double();
        }
    };

    class To_interval
      : public std::unary_function< Type, std::pair< double, double > > {
      public:
        std::pair<double, double> operator()( const Type& x ) const {

#if CGAL_LEDA_VERSION >= 501
          CGAL_LEDA_SCOPE::interval temp(x);
          std::pair<double, double> result(temp.lower_bound(),temp.upper_bound());
          CGAL_postcondition(Type(result.first)<=x);
          CGAL_postcondition(Type(result.second)>=x);
          return result;
#else
          CGAL_LEDA_SCOPE::bigfloat xnum = x.numerator();
          CGAL_LEDA_SCOPE::bigfloat xden = x.denominator();
          CGAL_LEDA_SCOPE::bigfloat xupp =
                                    div(xnum,xden,53,CGAL_LEDA_SCOPE::TO_P_INF);
          CGAL_LEDA_SCOPE::bigfloat xlow =
                                    div(xnum,xden,53,CGAL_LEDA_SCOPE::TO_N_INF);

          // really smallest positive double
          double MinDbl = CGAL_LEDA_SCOPE::fp::compose_parts(0,0,0,1);

          double low = xlow.to_double();
          while(Type(low) > x) low = low - MinDbl;

          double upp = xupp.to_double();
          while(Type(upp) < x) upp = upp + MinDbl;

          std::pair<double, double> result(low,upp);
          CGAL_postcondition(Type(result.first)<=x);
          CGAL_postcondition(Type(result.second)>=x);
          return result;
#endif
          // Original CGAL to_interval (seemed to be inferior)
          //  // There's no guarantee about the error of to_double(), so I add
          //  //  3 ulps...
          //  Protect_FPU_rounding<true> P (CGAL_FE_TONEAREST);
          //  Interval_nt_advanced approx (z.to_double());
          //  FPU_set_cw(CGAL_FE_UPWARD);
          //
          //  approx += Interval_nt<false>::smallest();
          //  approx += Interval_nt<false>::smallest();
          //  approx += Interval_nt<false>::smallest();
          //  return approx.pair();

        }
    };
};

/*! \ingroup NiX_Fraction_traits_spec
 *  \brief Specialization of Fraction_traits for ::leda::rational
 */
template <>
class Fraction_traits< leda_rational > {
public:
    typedef leda_rational Type;
    typedef ::CGAL::Tag_true Is_fraction;
    typedef leda_integer Numerator_type;
    typedef Numerator_type Denominator_type;

    typedef Algebraic_structure_traits< Numerator_type >::Gcd Common_factor;

    class Decompose {
    public:
        typedef Type first_argument_type;
        typedef Numerator_type& second_argument_type;
        typedef Numerator_type& third_argument_type;
        void operator () (
                const Type& rat,
                Numerator_type& num,
                Numerator_type& den) {
            num = rat.numerator();
            den = rat.denominator();
        }
    };

    class Compose {
    public:
        typedef Numerator_type first_argument_type;
        typedef Numerator_type second_argument_type;
        typedef Type result_type;
        Type operator ()(
                const Numerator_type& num ,
                const Numerator_type& den ) {
            Type result(num, den);
            result.normalize();
            return result;
        }
    };
};

// Modular_traits
template<>
class Modular_traits< ::leda::rational > {
  typedef ::leda::integer Integer;
  typedef CGAL::Modular_traits<Integer> MT_int;
public:
  typedef ::leda::rational NT;
  typedef ::CGAL::Tag_true Is_modularizable;
  typedef CGAL::Residue Residue_type;

  struct Modular_image{
    Residue_type operator()(const NT& rat){
      MT_int::Modular_image int_mod;
      Residue_type num = int_mod(rat.numerator());
      Residue_type den = int_mod(rat.denominator());
      return num/den;
    }
  };
  struct Modular_image_representative{
    NT operator()(const Residue_type& x){
      return NT(x.get_value());
    }
  };    
};


template <class F>
class Output_rep< leda_rational, F> {
    const leda_rational& t;
public:
    //! initialize with a const reference to \a t.
    Output_rep( const leda_rational& tt) : t(tt) {}
    //! perform the output, calls \c operator\<\< by default.
    std::ostream& operator()( std::ostream& out) const {
        switch (get_mode(out)) {
        case IO::PRETTY:{
            if(t.denominator() == leda_integer(1))
                return out <<t.numerator();
            else
                return out << t.numerator()
                           << "/"
                           << t.denominator();
            break;
        }

        default:
            return out << t.numerator()
                       << "/"
                       << t.denominator();
        }
    }
};

template <>
struct Needs_parens_as_product< leda_rational >{
    bool operator()( leda_rational t){
        if (t.denominator() != 1 )
            return true;
        else
            return needs_parens_as_product(t.numerator()) ;
    }
};

template <>
class Output_rep< leda_rational, Parens_as_product_tag > {
    const leda_rational& t;
public:
    // Constructor
    Output_rep( const leda_rational& tt) : t(tt) {}
    // operator
    std::ostream& operator()( std::ostream& out) const {
        Needs_parens_as_product< leda_rational > needs_parens_as_product;
        if (needs_parens_as_product(t))
            return out <<"("<< oformat(t) <<")";
        else
            return out << oformat(t);
    }
};

template < >
class Benchmark_rep< leda_rational > {
    const leda_rational& t;
public:
    //! initialize with a const reference to \a t.
    Benchmark_rep( const leda_rational& tt) : t(tt) {}
    //! perform the output, calls \c operator\<\< by default.
    std::ostream& operator()( std::ostream& out) const { 
            return 
                out << "Rational(" << t.numerator() << "," 
                    << t.denominator() << ")";
    }

    static std::string get_benchmark_name() {
        return "Rational";
    }

};


CGAL_END_NAMESPACE

// Unary + is missing for leda::rational
namespace leda{
inline rational operator+( const rational& i) { return i; }
}

//since types are included by leda_coercion_traits.h:
#include <CGAL/leda_integer.h>
#include <CGAL/leda_rational.h>
#include <CGAL/leda_bigfloat.h>
#include <CGAL/leda_real.h>

#endif // CGAL_USE_LEDA

#endif  // CGAL_LEDA_RATIONAL_H
