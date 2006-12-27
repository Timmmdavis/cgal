// Copyright (c) 2000  Utrecht University (The Netherlands),
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
// Author(s)     : Andreas Fabri

#ifndef CGAL_CARTESIAN_TETRAHEDRON_3_H
#define CGAL_CARTESIAN_TETRAHEDRON_3_H

#include <CGAL/Fourtuple.h>
#include <CGAL/Handle_for.h>
#include <vector>
#include <functional>

CGAL_BEGIN_NAMESPACE

template <class R_>
class TetrahedronC3
{
  typedef typename R_::FT                   FT;
  typedef typename R_::Point_3              Point_3;
  typedef typename R_::Plane_3              Plane_3;
  typedef typename R_::Tetrahedron_3        Tetrahedron_3;

  typedef Fourtuple<Point_3>                       Rep;
  typedef typename R_::template Handle<Rep>::type  Base;

  Base base;

public:
  typedef R_                                     R;

  TetrahedronC3() {}

  TetrahedronC3(const Point_3 &p, const Point_3 &q, const Point_3 &r,
                const Point_3 &s)
    : base(p, q, r, s) {}

  const Point_3 &    vertex(int i) const;
  const Point_3 &    operator[](int i) const;

  bool       operator==(const TetrahedronC3 &t) const;
  bool       operator!=(const TetrahedronC3 &t) const;

  Orientation    orientation() const;
  Oriented_side  oriented_side(const Point_3 &p) const;
  Bounded_side   bounded_side(const Point_3 &p) const;

  bool       has_on_boundary(const Point_3 &p) const;
  bool       has_on_positive_side(const Point_3 &p) const;
  bool       has_on_negative_side(const Point_3 &p) const;
  bool       has_on_bounded_side(const Point_3 &p) const;
  bool       has_on_unbounded_side(const Point_3 &p) const;

  bool       is_degenerate() const;
};

template < class R >
bool
TetrahedronC3<R>::
operator==(const TetrahedronC3<R> &t) const
{
  if (CGAL::identical(base, t.base))
      return true;
  if (orientation() != t.orientation())
      return false;

  std::vector< Point_3 > V1;
  std::vector< Point_3 > V2;
  typename std::vector< Point_3 >::iterator uniq_end1;
  typename std::vector< Point_3 >::iterator uniq_end2;
  int k;
  for ( k=0; k < 4; k++) V1.push_back( vertex(k));
  for ( k=0; k < 4; k++) V2.push_back( t.vertex(k));
  typename R::Less_xyz_3 Less_object = R().less_xyz_3_object();
  std::sort(V1.begin(), V1.end(), Less_object);
  std::sort(V2.begin(), V2.end(), Less_object);
  uniq_end1 = std::unique( V1.begin(), V1.end());
  uniq_end2 = std::unique( V2.begin(), V2.end());
  V1.erase( uniq_end1, V1.end());
  V2.erase( uniq_end2, V2.end());
  return V1 == V2;
}

template < class R >
inline
bool
TetrahedronC3<R>::
operator!=(const TetrahedronC3<R> &t) const
{
  return !(*this == t);
}

template < class R >
const typename TetrahedronC3<R>::Point_3 &
TetrahedronC3<R>::
vertex(int i) const
{
  if (i<0) i=(i%4)+4;
  else if (i>3) i=i%4;
  switch (i)
    {
    case 0: return get(base).e0;
    case 1: return get(base).e1;
    case 2: return get(base).e2;
    default: return get(base).e3;
    }
}

template < class R >
inline
const typename TetrahedronC3<R>::Point_3 &
TetrahedronC3<R>::
operator[](int i) const
{
  return vertex(i);
}

template < class R >
Orientation
TetrahedronC3<R>::
orientation() const
{
  return R().orientation_3_object()(vertex(0), vertex(1),
                                    vertex(2), vertex(3));
}

template < class R >
Oriented_side
TetrahedronC3<R>::
oriented_side(const typename TetrahedronC3<R>::Point_3 &p) const
{
  Orientation o = orientation();
  if (o != ZERO)
    return Oriented_side(o * bounded_side(p));

  CGAL_kernel_assertion (!is_degenerate());
  return ON_ORIENTED_BOUNDARY;
}

template < class R >
Bounded_side
TetrahedronC3<R>::
bounded_side(const typename TetrahedronC3<R>::Point_3 &p) const
{
  return R().bounded_side_3_object()
               (static_cast<const typename R::Tetrahedron_3>(*this), p);
}

template < class R >
inline
bool
TetrahedronC3<R>::has_on_boundary
  (const typename TetrahedronC3<R>::Point_3 &p) const
{
  return oriented_side(p) == ON_ORIENTED_BOUNDARY;
}

template < class R >
inline
bool
TetrahedronC3<R>::has_on_positive_side
  (const typename TetrahedronC3<R>::Point_3 &p) const
{
  return oriented_side(p) == ON_POSITIVE_SIDE;
}

template < class R >
inline
bool
TetrahedronC3<R>::has_on_negative_side
  (const typename TetrahedronC3<R>::Point_3 &p) const
{
  return oriented_side(p) == ON_NEGATIVE_SIDE;
}

template < class R >
inline
bool
TetrahedronC3<R>::has_on_bounded_side
  (const typename TetrahedronC3<R>::Point_3 &p) const
{
  return bounded_side(p) == ON_BOUNDED_SIDE;
}

template < class R >
inline
bool
TetrahedronC3<R>::has_on_unbounded_side
  (const typename TetrahedronC3<R>::Point_3 &p) const
{
  return bounded_side(p) == ON_UNBOUNDED_SIDE;
}

template < class R >
inline
bool
TetrahedronC3<R>::is_degenerate() const
{
  return orientation() == COPLANAR;
}

CGAL_END_NAMESPACE

#endif // CGAL_CARTESIAN_TETRAHEDRON_3_H
