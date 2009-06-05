/* Copyright (C) 2009 Free Software Foundation, Inc.
* Contributed by Jan Sjodin <jan.sjodin@amd.com>.

* This file is part of the Polyhedral Compilation Package Library (libpcp).

* Libpcp is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2.1 of the License, or
* (at your option) any later version.

* Libpcp is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
* more details.

* You should have received a copy of the GNU Lesser General Public License 
* along with libpcp; see the file COPYING.LIB.  If not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
* MA 02110-1301, USA.  

* As a special exception, if you link this library with other files, some
* of which are compiled with GCC, to produce an executable, this library
* does not by itself cause the resulting executable to be covered by the
* GNU General Public License.  This exception does not however invalidate
* any other reasons why the executable file might be covered by the GNU
* General Public License.  
*/

#ifndef _PCP_POLY_SPI_C_
#define _PCP_POLY_SPI_C_


#ifdef __cplusplus
extern "C" {
#endif

struct pcp_poly_spi
{
  /* Coefficient Primitives */
  int (*coefficientGetValue)(void* coeff);
  const char* (*coefficientToString)(void* coeff);
  void* (*coefficientCreate)(int value);
  void (*coefficientDestroy)(void* coeff);

  /* Linear Expr Primitives */
  void* (*linearExprCopy)(void* linearExpr);
  void (*linearExprAddToCoefficient)(void* linearExpr, int dimension, void* value);
  void (*linearExprAddToInhomogenous)(void* linearExpr, void* value);
  void (*linearExprSetCoefficient)(void* linearExp, int dimension, void* value);
  int (*linearExprGetNumDimensions)(void* linearExpr);
  void (*linearExprSubtract)(void* linearExpr, void* subtract);
  const char* (*linearExprToString)(void* linearExpr);

  void* (*linearExprCreate)();
  void* (*linearExprCreateDim)(int numDimensions);
  void (*linearExprDestroy)(void* linearExpr);

  /* Constraint primitives */
  void* (*constraintCreateEqualZero)(void* linearExpr);
  void* (*constraintCreateGreaterEqualZero)(void* linearExpr);
  void (*constraintDestroy)(void* constraint);
  const char* (*constraintToString)(void* constraint);

  /* Polyhedron primitives */
  void* (*polyhedronGetConstraintSystem)(void* polyhedron);
  void (*polyhedronAddConstraint)(void* polyhedron, void* constraint);
  void (*polyhedronAddConstraints)(void* polyhedron, void* constraintSystem);
  void (*polyhedronRemap)(void* polyhedron, int* map, int sizen);
  void* (*polyhedronCopy)(void* polyhedron);

  void* (*polyhedronCreate)(int numDimensions);
  void (*polyhedronDestroy)(void* polyhedron);

  /* PointsetPowerset primitives */
  void* (*pointsetPowersetCreate)(void* polyhedron);
  void (*pointsetPowersetDestroy)(void* poinsetPowerset);

};

#ifdef __cplusplus
}
PcpPolySpi* createPPLPolySpi();
#endif

#endif /* _PCP_POLY_SPI_C_ */
