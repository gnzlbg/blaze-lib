//=================================================================================================
/*!
//  \file blaze/math/expressions/TDMatTSMatMultExpr.h
//  \brief Header file for the transpose dense matrix/transpose sparse matrix multiplication expression
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZE_MATH_EXPRESSIONS_TDMATTSMATMULTEXPR_H_
#define _BLAZE_MATH_EXPRESSIONS_TDMATTSMATMULTEXPR_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <stdexcept>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/MatMatMultExpr.h>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/expressions/Computation.h>
#include <blaze/math/expressions/DenseMatrix.h>
#include <blaze/math/expressions/Forward.h>
#include <blaze/math/expressions/MatMatMultExpr.h>
#include <blaze/math/Intrinsics.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/shims/Reset.h>
#include <blaze/math/shims/Serial.h>
#include <blaze/math/traits/ColumnExprTrait.h>
#include <blaze/math/traits/MultExprTrait.h>
#include <blaze/math/traits/MultTrait.h>
#include <blaze/math/traits/RowExprTrait.h>
#include <blaze/math/traits/SubmatrixExprTrait.h>
#include <blaze/math/traits/TDMatDVecMultExprTrait.h>
#include <blaze/math/traits/TDMatSVecMultExprTrait.h>
#include <blaze/math/traits/TSMatDVecMultExprTrait.h>
#include <blaze/math/traits/TSMatSVecMultExprTrait.h>
#include <blaze/math/traits/TDVecTDMatMultExprTrait.h>
#include <blaze/math/traits/TDVecTSMatMultExprTrait.h>
#include <blaze/math/traits/TSVecTDMatMultExprTrait.h>
#include <blaze/math/typetraits/Columns.h>
#include <blaze/math/typetraits/IsColumnMajorMatrix.h>
#include <blaze/math/typetraits/IsColumnVector.h>
#include <blaze/math/typetraits/IsComputation.h>
#include <blaze/math/typetraits/IsDenseMatrix.h>
#include <blaze/math/typetraits/IsDenseVector.h>
#include <blaze/math/typetraits/IsDiagonal.h>
#include <blaze/math/typetraits/IsExpression.h>
#include <blaze/math/typetraits/IsLower.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/math/typetraits/IsRowVector.h>
#include <blaze/math/typetraits/IsSparseMatrix.h>
#include <blaze/math/typetraits/IsSparseVector.h>
#include <blaze/math/typetraits/IsStrictlyLower.h>
#include <blaze/math/typetraits/IsStrictlyUpper.h>
#include <blaze/math/typetraits/IsSymmetric.h>
#include <blaze/math/typetraits/IsUniLower.h>
#include <blaze/math/typetraits/IsUniUpper.h>
#include <blaze/math/typetraits/IsUpper.h>
#include <blaze/math/typetraits/RequiresEvaluation.h>
#include <blaze/math/typetraits/Rows.h>
#include <blaze/system/Thresholds.h>
#include <blaze/util/Assert.h>
#include <blaze/util/constraints/Reference.h>
#include <blaze/util/DisableIf.h>
#include <blaze/util/EnableIf.h>
#include <blaze/util/InvalidType.h>
#include <blaze/util/logging/FunctionTrace.h>
#include <blaze/util/mpl/And.h>
#include <blaze/util/mpl/Or.h>
#include <blaze/util/SelectType.h>
#include <blaze/util/Types.h>
#include <blaze/util/typetraits/RemoveReference.h>
#include <blaze/util/valuetraits/IsTrue.h>


namespace blaze {

//=================================================================================================
//
//  CLASS TDMATTSMATMULTEXPR
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Expression object for transpose dense matrix-transpose sparse matrix multiplications.
// \ingroup dense_matrix_expression
//
// The TDMatTSMatMultExpr class represents the compile time expression for multiplications between
// a column-major dense matrix and a column-major sparse matrix.
*/
template< typename MT1    // Type of the left-hand side dense matrix
        , typename MT2 >  // Type of the right-hand side sparse matrix
class TDMatTSMatMultExpr : public DenseMatrix< TDMatTSMatMultExpr<MT1,MT2>, true >
                         , private MatMatMultExpr
                         , private Computation
{
 private:
   //**Type definitions****************************************************************************
   typedef typename MT1::ResultType     RT1;  //!< Result type of the left-hand side dense matrix expression.
   typedef typename MT2::ResultType     RT2;  //!< Result type of the right-hand side sparse matrix expression.
   typedef typename RT1::ElementType    ET1;  //!< Element type of the left-hand side dense matrix expression.
   typedef typename RT2::ElementType    ET2;  //!< Element type of the right-hand side sparse matrix expression.
   typedef typename MT1::CompositeType  CT1;  //!< Composite type of the left-hand side dense matrix expression.
   typedef typename MT2::CompositeType  CT2;  //!< Composite type of the right-hand side sparse matrix expression.
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the composite type of the left-hand side dense matrix expression.
   enum { evaluateLeft = IsComputation<MT1>::value || RequiresEvaluation<MT1>::value };
   //**********************************************************************************************

   //**********************************************************************************************
   //! Compilation switch for the composite type of the right-hand side sparse matrix expression.
   enum { evaluateRight = IsComputation<MT2>::value || RequiresEvaluation<MT2>::value };
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! The CanExploitSymmetry struct is a helper struct for the selection of the optimal
       evaluation strategy. In case the target matrix is row-major and either of the two
       matrix operands is symmetric, \a value is set to 1 and an optimized evaluation
       strategy is selected. Otherwise \a value is set to 0 and the default strategy is
       chosen. */
   template< typename T1, typename T2, typename T3 >
   struct CanExploitSymmetry {
      enum { value = IsRowMajorMatrix<T1>::value &&
                     ( IsSymmetric<T2>::value || IsSymmetric<T3>::value ) };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! The IsEvaluationRequired struct is a helper struct for the selection of the parallel
       evaluation strategy. In case either of the two matrix operands requires an intermediate
       evaluation, the nested \value will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct IsEvaluationRequired {
      enum { value = ( evaluateLeft || evaluateRight ) &&
                     !CanExploitSymmetry<T1,T2,T3>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case all three involved data types are suited for a vectorized computation of the
       matrix multiplication, the nested \value will be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseVectorizedKernel {
      enum { value = !IsDiagonal<T2>::value &&
                     T1::vectorizable && T2::vectorizable &&
                     IsColumnMajorMatrix<T1>::value &&
                     IsSame<typename T1::ElementType,typename T2::ElementType>::value &&
                     IsSame<typename T1::ElementType,typename T3::ElementType>::value &&
                     IntrinsicTrait<typename T1::ElementType>::addition &&
                     IntrinsicTrait<typename T1::ElementType>::subtraction &&
                     IntrinsicTrait<typename T1::ElementType>::multiplication };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case a vectorized computation of the matrix multiplication is not possible, but a
       loop-unrolled computation is feasible, the nested \value will be set to 1, otherwise
       it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseOptimizedKernel {
      enum { value = !UseVectorizedKernel<T1,T2,T3>::value &&
                     !IsDiagonal<T2>::value &&
                     !IsResizable<typename T1::ElementType>::value &&
                     !IsResizable<ET2>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

   //**********************************************************************************************
   /*! \cond BLAZE_INTERNAL */
   //! Helper structure for the explicit application of the SFINAE principle.
   /*! In case neither a vectorized nor optimized computation is possible, the nested \value will
       be set to 1, otherwise it will be 0. */
   template< typename T1, typename T2, typename T3 >
   struct UseDefaultKernel {
      enum { value = !UseVectorizedKernel<T1,T2,T3>::value &&
                     !UseOptimizedKernel<T1,T2,T3>::value };
   };
   /*! \endcond */
   //**********************************************************************************************

 public:
   //**Type definitions****************************************************************************
   typedef TDMatTSMatMultExpr<MT1,MT2>                 This;           //!< Type of this TDMatTSMatMultExpr instance.
   typedef typename MultTrait<RT1,RT2>::Type           ResultType;     //!< Result type for expression template evaluations.
   typedef typename ResultType::OppositeType           OppositeType;   //!< Result type with opposite storage order for expression template evaluations.
   typedef typename ResultType::TransposeType          TransposeType;  //!< Transpose type for expression template evaluations.
   typedef typename ResultType::ElementType            ElementType;    //!< Resulting element type.
   typedef typename IntrinsicTrait<ElementType>::Type  IntrinsicType;  //!< Resulting intrinsic element type.
   typedef const ElementType                           ReturnType;     //!< Return type for expression template evaluations.
   typedef const ResultType                            CompositeType;  //!< Data type for composite expression templates.

   //! Composite type of the left-hand side dense matrix expression.
   typedef typename SelectType< IsExpression<MT1>::value, const MT1, const MT1& >::Type  LeftOperand;

   //! Composite type of the right-hand side sparse matrix expression.
   typedef typename SelectType< IsExpression<MT2>::value, const MT2, const MT2& >::Type  RightOperand;

   //! Type for the assignment of the left-hand side dense matrix operand.
   typedef typename SelectType< evaluateLeft, const RT1, CT1 >::Type  LT;

   //! Type for the assignment of the right-hand side sparse matrix operand.
   typedef typename SelectType< evaluateRight, const RT2, CT2 >::Type  RT;
   //**********************************************************************************************

   //**Compilation flags***************************************************************************
   //! Compilation switch for the expression template evaluation strategy.
   enum { vectorizable = !IsDiagonal<MT1>::value &&
                         MT1::vectorizable &&
                         IsSame<ET1,ET2>::value &&
                         IntrinsicTrait<ET1>::addition &&
                         IntrinsicTrait<ET1>::multiplication };

   //! Compilation switch for the expression template assignment strategy.
   enum { smpAssignable = !evaluateLeft  && MT1::smpAssignable &&
                          !evaluateRight && MT2::smpAssignable };
   //**********************************************************************************************

   //**Constructor*********************************************************************************
   /*!\brief Constructor for the TDMatTSMatMultExpr class.
   //
   // \param lhs The left-hand side dense matrix operand of the multiplication expression.
   // \param rhs The right-hand side sparse matrix operand of the multiplication expression.
   */
   explicit inline TDMatTSMatMultExpr( const MT1& lhs, const MT2& rhs )
      : lhs_( lhs )  // Left-hand side dense matrix of the multiplication expression
      , rhs_( rhs )  // Right-hand side sparse matrix of the multiplication expression
   {
      BLAZE_INTERNAL_ASSERT( lhs.columns() == rhs.rows(), "Invalid matrix sizes" );
   }
   //**********************************************************************************************

   //**Access operator*****************************************************************************
   /*!\brief 2D-access to the matrix elements.
   //
   // \param i Access index for the row. The index has to be in the range \f$[0..M-1]\f$.
   // \param j Access index for the column. The index has to be in the range \f$[0..N-1]\f$.
   // \return The resulting value.
   */
   inline ReturnType operator()( size_t i, size_t j ) const {
      BLAZE_INTERNAL_ASSERT( i < lhs_.rows()   , "Invalid row access index"    );
      BLAZE_INTERNAL_ASSERT( j < rhs_.columns(), "Invalid column access index" );

      typedef typename RemoveReference<CT2>::Type::ConstIterator  ConstIterator;

      ElementType tmp = ElementType();

      // Early exit
      if( lhs_.columns() == 0UL )
         return tmp;

      // Fast computation in case the right-hand side sparse matrix directly provides iterators
      if( !RequiresEvaluation<MT2>::value )
      {
         CT2 B( rhs_ );  // Evaluation of the right-hand side sparse matrix operand

         const ConstIterator end( ( IsLower<MT1>::value )?( B.upperBound(i,j) ):( B.end(j) ) );
         ConstIterator element( ( IsUpper<MT1>::value )?( B.lowerBound(i,j) ):( B.begin(j) ) );

         // Early exit in case column j is empty
         if( element == end )
            return tmp;

         // Calculating element (i,j)
         tmp = lhs_(i,element->index()) * element->value();
         ++element;
         for( ; element!=end; ++element )
            tmp += lhs_(i,element->index()) * element->value();
      }

      // Default computation in case the right-hand side sparse matrix doesn't provide iterators
      else
      {
         const size_t kbegin( ( IsUpper<MT1>::value || IsLower<MT2>::value )?( i ):( 0UL ) );
         const size_t kend  ( ( IsLower<MT1>::value || IsUpper<MT2>::value )?( i+1UL ):( lhs_.columns() ) );
         BLAZE_INTERNAL_ASSERT( kbegin <= kend, "Invalid loop indices detected" );

         tmp = lhs_(i,kbegin) * rhs_(kbegin,j);
         for( size_t k=kbegin+1UL; k<kend; ++k ) {
            tmp += lhs_(i,k) * rhs_(k,j);
         }
      }

      return tmp;
   }
   //**********************************************************************************************

   //**Rows function*******************************************************************************
   /*!\brief Returns the current number of rows of the matrix.
   //
   // \return The number of rows of the matrix.
   */
   inline size_t rows() const {
      return lhs_.rows();
   }
   //**********************************************************************************************

   //**Columns function****************************************************************************
   /*!\brief Returns the current number of columns of the matrix.
   //
   // \return The number of columns of the matrix.
   */
   inline size_t columns() const {
      return rhs_.columns();
   }
   //**********************************************************************************************

   //**Left operand access*************************************************************************
   /*!\brief Returns the left-hand side transpose dense matrix operand.
   //
   // \return The left-hand side transpose dense matrix operand.
   */
   inline LeftOperand leftOperand() const {
      return lhs_;
   }
   //**********************************************************************************************

   //**Right operand access************************************************************************
   /*!\brief Returns the right-hand side transpose sparse matrix operand.
   //
   // \return The right-hand side transpose sparse matrix operand.
   */
   inline RightOperand rightOperand() const {
      return rhs_;
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can alias with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case the expression can alias, \a false otherwise.
   */
   template< typename T >
   inline bool canAlias( const T* alias ) const {
      return ( lhs_.isAliased( alias ) || rhs_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression is aliased with the given address \a alias.
   //
   // \param alias The alias to be checked.
   // \return \a true in case an alias effect is detected, \a false otherwise.
   */
   template< typename T >
   inline bool isAliased( const T* alias ) const {
      return ( lhs_.isAliased( alias ) || rhs_.isAliased( alias ) );
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the operands of the expression are properly aligned in memory.
   //
   // \return \a true in case the operands are aligned, \a false if not.
   */
   inline bool isAligned() const {
      return lhs_.isAligned();
   }
   //**********************************************************************************************

   //**********************************************************************************************
   /*!\brief Returns whether the expression can be used in SMP assignments.
   //
   // \return \a true in case the expression can be used in SMP assignments, \a false if not.
   */
   inline bool canSMPAssign() const {
      return ( columns() > SMP_TDMATTSMATMULT_THRESHOLD );
   }
   //**********************************************************************************************

 private:
   //**Member variables****************************************************************************
   LeftOperand  lhs_;  //!< Left-hand side dense matrix of the multiplication expression.
   RightOperand rhs_;  //!< Right-hand side sparse matrix of the multiplication expression.
   //**********************************************************************************************

   //**Assignment to dense matrices****************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a transpose dense matrix-transpose sparse matrix multiplication to a
   //        dense matrix (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a transpose dense matrix-
   // transpose sparse matrix multiplication expression to a dense matrix.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline typename DisableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      assign( DenseMatrix<MT,SO>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side dense matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side sparse matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      TDMatTSMatMultExpr::selectAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default assignment to row-major dense matrices**********************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default assignment of a transpose dense matrix-transpose sparse matrix multiplication
   //        to row-major dense matrices (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the default row-major assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline void selectAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      const size_t ipos( A.rows() & size_t(-4) );
      BLAZE_INTERNAL_ASSERT( ( A.rows() - ( A.rows() % 4UL ) ) == ipos, "Invalid end calculation" );

      for( size_t i=0UL; i<ipos; i+=4UL ) {
         for( size_t j=0UL; j<B.columns(); ++j )
         {
            ConstIterator element( ( IsUpper<MT4>::value )?( B.lowerBound(i,j) ):( B.begin(j) ) );
            const ConstIterator end( ( IsLower<MT4>::value )?( B.upperBound(i+4UL,j) ):( B.end(j) ) );

            if( element == end ) {
               reset( (~C)(i    ,j) );
               reset( (~C)(i+1UL,j) );
               reset( (~C)(i+2UL,j) );
               reset( (~C)(i+3UL,j) );
               continue;
            }

            (~C)(i    ,j) = A(i    ,element->index()) * element->value();
            (~C)(i+1UL,j) = A(i+1UL,element->index()) * element->value();
            (~C)(i+2UL,j) = A(i+2UL,element->index()) * element->value();
            (~C)(i+3UL,j) = A(i+3UL,element->index()) * element->value();
            ++element;
            for( ; element!=end; ++element ) {
               (~C)(i    ,j) += A(i    ,element->index()) * element->value();
               (~C)(i+1UL,j) += A(i+1UL,element->index()) * element->value();
               (~C)(i+2UL,j) += A(i+2UL,element->index()) * element->value();
               (~C)(i+3UL,j) += A(i+3UL,element->index()) * element->value();
            }
         }
      }

      for( size_t i=ipos; i<A.rows(); ++i ) {
         for( size_t j=0UL; j<B.columns(); ++j )
         {
            ConstIterator element( ( IsUpper<MT4>::value )?( B.lowerBound(i,j) ):( B.begin(j) ) );
            const ConstIterator end( ( IsLower<MT4>::value )?( B.upperBound(i,j) ):( B.end(j) ) );

            if( element == end ) {
               reset( (~C)(i,j) );
               continue;
            }

            (~C)(i,j) = A(i,element->index()) * element->value();
            ++element;
            for( ; element!=end; ++element )
               (~C)(i,j) += A(i,element->index()) * element->value();
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default assignment to column-major dense matrices*******************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default assignment of a transpose dense matrix-transpose sparse matrix multiplication
   //        to column-major dense matrices (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the default column-major assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5> >::Type
      selectAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         for( size_t i=0UL; i<(~C).rows(); ++i ) {
            reset( (~C)(i,j) );
         }

         ConstIterator element( B.begin(j) );
         const ConstIterator end( B.end(j) );

         for( ; element!=end; ++element )
         {
            const size_t j1( element->index() );

            if( IsDiagonal<MT4>::value )
            {
               (~C)(j1,j) = A(j1,j1) * element->value();
            }
            else
            {
               const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
               const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
               BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

               for( size_t i=ibegin; i<iend; ++i ) {
                  if( isDefault( (~C)(i,j) ) )
                     (~C)(i,j) = A(i,j1) * element->value();
                  else
                     (~C)(i,j) += A(i,j1) * element->value();
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Optimized assignment to column-major dense matrices*****************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Optimized assignment of a transpose dense matrix-transpose sparse matrix multiplication
   //        to column-major dense matrices (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the optimized column-major assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseOptimizedKernel<MT3,MT4,MT5> >::Type
      selectAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      reset( ~C );

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         const ConstIterator end( B.end(j) );
         ConstIterator element( B.begin(j) );

         const size_t nonzeros( B.nonZeros(j) );
         const size_t kpos( nonzeros & size_t(-4) );
         BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

         for( size_t k=0UL; k<kpos; k+=4UL )
         {
            const size_t j1( element->index() );
            const ET2    v1( element->value() );
            ++element;
            const size_t j2( element->index() );
            const ET2    v2( element->value() );
            ++element;
            const size_t j3( element->index() );
            const ET2    v3( element->value() );
            ++element;
            const size_t j4( element->index() );
            const ET2    v4( element->value() );
            ++element;

            BLAZE_INTERNAL_ASSERT( j1 < j2 && j2 < j3 && j3 < j4, "Invalid sparse matrix index detected" );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j4+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            const size_t inum( iend - ibegin );
            const size_t ipos( ibegin + ( inum & size_t(-4) ) );
            BLAZE_INTERNAL_ASSERT( ( ibegin + inum - ( inum % 4UL ) ) == ipos, "Invalid end calculation" );

            for( size_t i=ibegin; i<ipos; i+=4UL ) {
               (~C)(i    ,j) += A(i    ,j1) * v1 + A(i    ,j2) * v2 + A(i    ,j3) * v3 + A(i    ,j4) * v4;
               (~C)(i+1UL,j) += A(i+1UL,j1) * v1 + A(i+1UL,j2) * v2 + A(i+1UL,j3) * v3 + A(i+1UL,j4) * v4;
               (~C)(i+2UL,j) += A(i+2UL,j1) * v1 + A(i+2UL,j2) * v2 + A(i+2UL,j3) * v3 + A(i+2UL,j4) * v4;
               (~C)(i+3UL,j) += A(i+3UL,j1) * v1 + A(i+3UL,j2) * v2 + A(i+3UL,j3) * v3 + A(i+3UL,j4) * v4;
            }
            for( size_t i=ipos; i<iend; ++i ) {
               (~C)(i,j) += A(i,j1) * v1 + A(i,j2) * v2 + A(i,j3) * v3 + A(i,j4) * v4;
            }
         }

         for( ; element!=end; ++element )
         {
            const size_t j1( element->index() );
            const ET2    v1( element->value() );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            const size_t inum( iend - ibegin );
            const size_t ipos( ibegin + ( inum & size_t(-4) ) );
            BLAZE_INTERNAL_ASSERT( ( ibegin + inum - ( inum % 4UL ) ) == ipos, "Invalid end calculation" );

            for( size_t i=ibegin; i<ipos; i+=4UL ) {
               (~C)(i    ,j) += A(i    ,j1) * v1;
               (~C)(i+1UL,j) += A(i+1UL,j1) * v1;
               (~C)(i+2UL,j) += A(i+2UL,j1) * v1;
               (~C)(i+3UL,j) += A(i+3UL,j1) * v1;
            }
            for( size_t i=ipos; i<iend; ++i ) {
               (~C)(i,j) += A(i,j1) * v1;
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized assignment to column-major dense matrices****************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to column-major dense matrices (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the vectorized column-major assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedKernel<MT3,MT4,MT5> >::Type
      selectAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef IntrinsicTrait<ElementType>  IT;
      typedef typename MT5::ConstIterator  ConstIterator;

      reset( ~C );

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         const ConstIterator end( B.end(j) );
         ConstIterator element( B.begin(j) );

         const size_t nonzeros( B.nonZeros(j) );
         const size_t kpos( nonzeros & size_t(-4) );
         BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

         for( size_t k=0UL; k<kpos; k+=4UL )
         {
            const size_t        j1( element->index() );
            const IntrinsicType v1( set( element->value() ) );
            ++element;
            const size_t        j2( element->index() );
            const IntrinsicType v2( set( element->value() ) );
            ++element;
            const size_t        j3( element->index() );
            const IntrinsicType v3( set( element->value() ) );
            ++element;
            const size_t        j4( element->index() );
            const IntrinsicType v4( set( element->value() ) );
            ++element;

            BLAZE_INTERNAL_ASSERT( j1 < j2 && j2 < j3 && j3 < j4, "Invalid sparse matrix index detected" );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 & size_t(-IT::size) ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j4+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            for( size_t i=ibegin; i<iend; i+=IT::size ) {
               (~C).store( i, j, (~C).load(i,j) + A.load(i,j1) * v1 + A.load(i,j2) * v2 + A.load(i,j3) * v3 + A.load(i,j4) * v4 );
            }
         }

         for( ; element!=end; ++element )
         {
            const size_t        j1( element->index() );
            const IntrinsicType v1( set( element->value() ) );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 & size_t(-IT::size) ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            for( size_t i=ibegin; i<iend; i+=IT::size ) {
               (~C).store( i, j, (~C).load(i,j) + A.load(i,j1) * v1 );
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Assignment to sparse matrices***************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Assignment of a transpose dense matrix-transpose sparse matrix multiplication to a
   //        sparse matrix (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side sparse matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized assignment of a transpose dense matrix-
   // transpose sparse matrix multiplication expression to a sparse matrix.
   */
   template< typename MT  // Type of the target sparse matrix
           , bool SO >    // Storage order of the target sparse matrix
   friend inline typename DisableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      assign( SparseMatrix<MT,SO>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      typedef typename SelectType< SO, ResultType, OppositeType >::Type  TmpType;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_SAME_STORAGE_ORDER( MT, TmpType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename TmpType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const TmpType tmp( serial( rhs ) );
      assign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring assignment to row-major matrices************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to a row-major matrix (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the symmetry-based restructuring assignment of a transpose dense
   // matrix-transpose sparse matrix multiplication expression to a row-major matrix. Due to the
   // explicit application of the SFINAE principle this function can only be selected by the
   // compiler in case the symmetry of either of the two matrix operands can be exploited.
   */
   template< typename MT >  // Type of the target matrix
   friend inline typename EnableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      assign( Matrix<MT,false>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( IsSymmetric<MT1>::value && IsSymmetric<MT2>::value )
         assign( ~lhs, trans( rhs.lhs_ ) * trans( rhs.rhs_ ) );
      else if( IsSymmetric<MT1>::value )
         assign( ~lhs, trans( rhs.lhs_ ) * rhs.rhs_ );
      else
         assign( ~lhs, rhs.lhs_ * trans( rhs.rhs_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to dense matrices*******************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Addition assignment of a transpose dense matrix-transpose sparse matrix multiplication
   //        to a dense matrix (\f$ A+=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized addition assignment of a transpose dense
   // matrix-transpose sparse matrix multiplication expression to a dense matrix.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline typename DisableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      addAssign( DenseMatrix<MT,SO>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side dense matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side sparse matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      TDMatTSMatMultExpr::selectAddAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default addition assignment to row-major dense matrices*************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default addition assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to row-major dense matrices (\f$ A+=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the default addition assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication to a row-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline void selectAddAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      const size_t ipos( A.rows() & size_t(-4) );
      BLAZE_INTERNAL_ASSERT( ( A.rows() - ( A.rows() % 4UL ) ) == ipos, "Invalid end calculation" );

      for( size_t i=0UL; i<ipos; i+=4UL ) {
         for( size_t j=0UL; j<B.columns(); ++j )
         {
            ConstIterator element( ( IsUpper<MT4>::value )?( B.lowerBound(i,j) ):( B.begin(j) ) );
            const ConstIterator end( ( IsLower<MT4>::value )?( B.upperBound(i+4UL,j) ):( B.end(j) ) );

            for( ; element!=end; ++element ) {
               (~C)(i    ,j) += A(i    ,element->index()) * element->value();
               (~C)(i+1UL,j) += A(i+1UL,element->index()) * element->value();
               (~C)(i+2UL,j) += A(i+2UL,element->index()) * element->value();
               (~C)(i+3UL,j) += A(i+3UL,element->index()) * element->value();
            }
         }
      }

      for( size_t i=ipos; i<A.rows(); ++i ) {
         for( size_t j=0UL; j<B.columns(); ++j )
         {
            ConstIterator element( ( IsUpper<MT4>::value )?( B.lowerBound(i,j) ):( B.begin(j) ) );
            const ConstIterator end( ( IsLower<MT4>::value )?( B.upperBound(i,j) ):( B.end(j) ) );

            for( ; element!=end; ++element )
               (~C)(i,j) += A(i,element->index()) * element->value();
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default addition assignment to column-major dense matrices**********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default addition assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to column-major dense matrices (\f$ A+=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the default addition assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5> >::Type
      selectAddAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         ConstIterator element( B.begin(j) );
         const ConstIterator end( B.end(j) );

         for( ; element!=end; ++element )
         {
            const size_t j1( element->index() );

            if( IsDiagonal<MT4>::value )
            {
               (~C)(j1,j) += A(j1,j1) * element->value();
            }
            else
            {
               const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
               const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
               BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

               const size_t inum( iend - ibegin );
               const size_t ipos( ibegin + ( inum & size_t(-4) ) );
               BLAZE_INTERNAL_ASSERT( ( ibegin + inum - ( inum % 4UL ) ) == ipos, "Invalid end calculation" );

               for( size_t i=ibegin; i<ipos; i+=4UL ) {
                  (~C)(i    ,j) += A(i    ,j1) * element->value();
                  (~C)(i+1UL,j) += A(i+1UL,j1) * element->value();
                  (~C)(i+2UL,j) += A(i+2UL,j1) * element->value();
                  (~C)(i+3UL,j) += A(i+3UL,j1) * element->value();
               }
               for( size_t i=ipos; i<iend; ++i ) {
                  (~C)(i,j) += A(i,j1) * element->value();
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Optimized addition assignment to column-major dense matrices********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Optimized addition assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to column-major dense matrices (\f$ A+=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the optimized addition assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseOptimizedKernel<MT3,MT4,MT5> >::Type
      selectAddAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         const ConstIterator end( B.end(j) );
         ConstIterator element( B.begin(j) );

         const size_t nonzeros( B.nonZeros(j) );
         const size_t kpos( nonzeros & size_t(-4) );
         BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

         for( size_t k=0UL; k<kpos; k+=4UL )
         {
            const size_t j1( element->index() );
            const ET2    v1( element->value() );
            ++element;
            const size_t j2( element->index() );
            const ET2    v2( element->value() );
            ++element;
            const size_t j3( element->index() );
            const ET2    v3( element->value() );
            ++element;
            const size_t j4( element->index() );
            const ET2    v4( element->value() );
            ++element;

            BLAZE_INTERNAL_ASSERT( j1 < j2 && j2 < j3 && j3 < j4, "Invalid sparse matrix index detected" );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j4+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            const size_t inum( iend - ibegin );
            const size_t ipos( ibegin + ( inum & size_t(-4) ) );
            BLAZE_INTERNAL_ASSERT( ( ibegin + inum - ( inum % 4UL ) ) == ipos, "Invalid end calculation" );

            for( size_t i=ibegin; i<ipos; i+=4UL ) {
               (~C)(i    ,j) += A(i    ,j1) * v1 + A(i    ,j2) * v2 + A(i    ,j3) * v3 + A(i    ,j4) * v4;
               (~C)(i+1UL,j) += A(i+1UL,j1) * v1 + A(i+1UL,j2) * v2 + A(i+1UL,j3) * v3 + A(i+1UL,j4) * v4;
               (~C)(i+2UL,j) += A(i+2UL,j1) * v1 + A(i+2UL,j2) * v2 + A(i+2UL,j3) * v3 + A(i+2UL,j4) * v4;
               (~C)(i+3UL,j) += A(i+3UL,j1) * v1 + A(i+3UL,j2) * v2 + A(i+3UL,j3) * v3 + A(i+3UL,j4) * v4;
            }
            for( size_t i=ipos; i<iend; ++i ) {
               (~C)(i,j) += A(i,j1) * v1 + A(i,j2) * v2 + A(i,j3) * v3 + A(i,j4) * v4;
            }
         }

         for( ; element!=end; ++element )
         {
            const size_t j1( element->index() );
            const ET2    v1( element->value() );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            const size_t inum( iend - ibegin );
            const size_t ipos( ibegin + ( inum & size_t(-4) ) );
            BLAZE_INTERNAL_ASSERT( ( ibegin + inum - ( inum % 4UL ) ) == ipos, "Invalid end calculation" );

            for( size_t i=ibegin; i<ipos; i+=4UL ) {
               (~C)(i    ,j) += A(i    ,j1) * v1;
               (~C)(i+1UL,j) += A(i+1UL,j1) * v1;
               (~C)(i+2UL,j) += A(i+2UL,j1) * v1;
               (~C)(i+3UL,j) += A(i+3UL,j1) * v1;
            }
            for( size_t i=ipos; i<iend; ++i ) {
               (~C)(i,j) += A(i,j1) * v1;
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized addition assignment to column-major dense matrices*******************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized addition assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to column-major dense matrices (\f$ A+=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the vectorized addition assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedKernel<MT3,MT4,MT5> >::Type
      selectAddAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef IntrinsicTrait<ElementType>  IT;
      typedef typename MT5::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         const ConstIterator end( B.end(j) );
         ConstIterator element( B.begin(j) );

         const size_t nonzeros( B.nonZeros(j) );
         const size_t kpos( nonzeros & size_t(-4) );
         BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

         for( size_t k=0UL; k<kpos; k+=4UL )
         {
            const size_t        j1( element->index() );
            const IntrinsicType v1( set( element->value() ) );
            ++element;
            const size_t        j2( element->index() );
            const IntrinsicType v2( set( element->value() ) );
            ++element;
            const size_t        j3( element->index() );
            const IntrinsicType v3( set( element->value() ) );
            ++element;
            const size_t        j4( element->index() );
            const IntrinsicType v4( set( element->value() ) );
            ++element;

            BLAZE_INTERNAL_ASSERT( j1 < j2 && j2 < j3 && j3 < j4, "Invalid sparse matrix index detected" );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 & size_t(-IT::size) ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j4+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            for( size_t i=ibegin; i<iend; i+=IT::size ) {
               (~C).store( i, j, (~C).load(i,j) + A.load(i,j1) * v1 + A.load(i,j2) * v2 + A.load(i,j3) * v3 + A.load(i,j4) * v4 );
            }
         }

         for( ; element!=end; ++element )
         {
            const size_t        j1( element->index() );
            const IntrinsicType v1( set( element->value() ) );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 & size_t(-IT::size) ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            for( size_t i=ibegin; i<iend; i+=IT::size ) {
               (~C).store( i, j, (~C).load(i,j) + A.load(i,j1) * v1 );
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring addition assignment to row-major matrices*************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring addition assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to a row-major matrix (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the symmetry-based restructuring addition assignment of a transpose
   // dense matrix-transpose sparse matrix multiplication expression to a row-major matrix. Due to
   // the explicit application of the SFINAE principle this function can only be selected by the
   // compiler in case the symmetry of either of the two matrix operands can be exploited.
   */
   template< typename MT >  // Type of the target matrix
   friend inline typename EnableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      addAssign( Matrix<MT,false>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( IsSymmetric<MT1>::value && IsSymmetric<MT2>::value )
         addAssign( ~lhs, trans( rhs.lhs_ ) * trans( rhs.rhs_ ) );
      else if( IsSymmetric<MT1>::value )
         addAssign( ~lhs, trans( rhs.lhs_ ) * rhs.rhs_ );
      else
         addAssign( ~lhs, rhs.lhs_ * trans( rhs.rhs_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Addition assignment to sparse matrices******************************************************
   // No special implementation for the addition assignment to sparse matrices.
   //**********************************************************************************************

   //**Subtraction assignment to dense matrices****************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Subtraction assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to a dense matrix (\f$ A-=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized subtraction assignment of a transpose
   // dense matrix-transpose sparse matrix multiplication expression to a dense matrix.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline void subAssign( DenseMatrix<MT,SO>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( serial( rhs.lhs_ ) );  // Evaluation of the left-hand side dense matrix operand
      RT B( serial( rhs.rhs_ ) );  // Evaluation of the right-hand side sparse matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      TDMatTSMatMultExpr::selectSubAssignKernel( ~lhs, A, B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default subtraction assignment to row-major dense matrices**********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default subtraction assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to row-major matrices (\f$ A-=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the default subtraction assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication to a row-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline void selectSubAssignKernel( DenseMatrix<MT3,false>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      const size_t ipos( A.rows() & size_t(-4) );
      BLAZE_INTERNAL_ASSERT( ( A.rows() - ( A.rows() % 4UL ) ) == ipos, "Invalid end calculation" );

      for( size_t i=0UL; i<ipos; i+=4UL ) {
         for( size_t j=0UL; j<B.columns(); ++j )
         {
            ConstIterator element( ( IsUpper<MT4>::value )?( B.lowerBound(i,j) ):( B.begin(j) ) );
            const ConstIterator end( ( IsLower<MT4>::value )?( B.upperBound(i+4UL,j) ):( B.end(j) ) );

            for( ; element!=end; ++element ) {
               (~C)(i    ,j) -= A(i    ,element->index()) * element->value();
               (~C)(i+1UL,j) -= A(i+1UL,element->index()) * element->value();
               (~C)(i+2UL,j) -= A(i+2UL,element->index()) * element->value();
               (~C)(i+3UL,j) -= A(i+3UL,element->index()) * element->value();
            }
         }
      }

      for( size_t i=ipos; i<A.rows(); ++i ) {
         for( size_t j=0UL; j<B.columns(); ++j )
         {
            ConstIterator element( ( IsUpper<MT4>::value )?( B.lowerBound(i,j) ):( B.begin(j) ) );
            const ConstIterator end( ( IsLower<MT4>::value )?( B.upperBound(i,j) ):( B.end(j) ) );

            for( ; element!=end; ++element )
               (~C)(i,j) -= A(i,element->index()) * element->value();
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Default subtraction assignment to column-major dense matrices*******************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Default subtraction assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to column-major matrices (\f$ A-=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the default subtraction assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseDefaultKernel<MT3,MT4,MT5> >::Type
      selectSubAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         ConstIterator element( B.begin(j) );
         const ConstIterator end( B.end(j) );

         for( ; element!=end; ++element )
         {
            const size_t j1( element->index() );

            if( IsDiagonal<MT4>::value )
            {
               (~C)(j1,j) -= A(j1,j1) * element->value();
            }
            else
            {
               const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
               const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
               BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

               const size_t inum( iend - ibegin );
               const size_t ipos( ibegin + ( inum & size_t(-4) ) );
               BLAZE_INTERNAL_ASSERT( ( ibegin + inum - ( inum % 4UL ) ) == ipos, "Invalid end calculation" );

               for( size_t i=ibegin; i<ipos; i+=4UL ) {
                  (~C)(i    ,j) -= A(i    ,j1) * element->value();
                  (~C)(i+1UL,j) -= A(i+1UL,j1) * element->value();
                  (~C)(i+2UL,j) -= A(i+2UL,j1) * element->value();
                  (~C)(i+3UL,j) -= A(i+3UL,j1) * element->value();
               }
               for( size_t i=ipos; i<iend; ++i ) {
                  (~C)(i,j) -= A(i,j1) * element->value();
               }
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Optimized subtraction assignment to column-major dense matrices*****************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Optimized subtraction assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to column-major matrices (\f$ A-=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the optimized subtraction assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseOptimizedKernel<MT3,MT4,MT5> >::Type
      selectSubAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef typename MT5::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         const ConstIterator end( B.end(j) );
         ConstIterator element( B.begin(j) );

         const size_t nonzeros( B.nonZeros(j) );
         const size_t kpos( nonzeros & size_t(-4) );
         BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

         for( size_t k=0UL; k<kpos; k+=4UL )
         {
            const size_t j1( element->index() );
            const ET2    v1( element->value() );
            ++element;
            const size_t j2( element->index() );
            const ET2    v2( element->value() );
            ++element;
            const size_t j3( element->index() );
            const ET2    v3( element->value() );
            ++element;
            const size_t j4( element->index() );
            const ET2    v4( element->value() );
            ++element;

            BLAZE_INTERNAL_ASSERT( j1 < j2 && j2 < j3 && j3 < j4, "Invalid sparse matrix index detected" );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j4+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            const size_t inum( iend - ibegin );
            const size_t ipos( ibegin + ( inum & size_t(-4) ) );
            BLAZE_INTERNAL_ASSERT( ( ibegin + inum - ( inum % 4UL ) ) == ipos, "Invalid end calculation" );

            for( size_t i=ibegin; i<ipos; i+=4UL ) {
               (~C)(i    ,j) -= A(i    ,j1) * v1 + A(i    ,j2) * v2 + A(i    ,j3) * v3 + A(i    ,j4) * v4;
               (~C)(i+1UL,j) -= A(i+1UL,j1) * v1 + A(i+1UL,j2) * v2 + A(i+1UL,j3) * v3 + A(i+1UL,j4) * v4;
               (~C)(i+2UL,j) -= A(i+2UL,j1) * v1 + A(i+2UL,j2) * v2 + A(i+2UL,j3) * v3 + A(i+2UL,j4) * v4;
               (~C)(i+3UL,j) -= A(i+3UL,j1) * v1 + A(i+3UL,j2) * v2 + A(i+3UL,j3) * v3 + A(i+3UL,j4) * v4;
            }
            for( size_t i=ipos; i<iend; ++i ) {
               (~C)(i,j) -= A(i,j1) * v1 + A(i,j2) * v2 + A(i,j3) * v3 + A(i,j4) * v4;
            }
         }

         for( ; element!=end; ++element )
         {
            const size_t j1( element->index() );
            const ET2    v1( element->value() );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            const size_t inum( iend - ibegin );
            const size_t ipos( ibegin + ( inum & size_t(-4) ) );
            BLAZE_INTERNAL_ASSERT( ( ibegin + inum - ( inum % 4UL ) ) == ipos, "Invalid end calculation" );

            for( size_t i=ibegin; i<ipos; i+=4UL ) {
               (~C)(i    ,j) -= A(i    ,j1) * v1;
               (~C)(i+1UL,j) -= A(i+1UL,j1) * v1;
               (~C)(i+2UL,j) -= A(i+2UL,j1) * v1;
               (~C)(i+3UL,j) -= A(i+3UL,j1) * v1;
            }
            for( size_t i=ipos; i<iend; ++i ) {
               (~C)(i,j) -= A(i,j1) * v1;
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Vectorized subtraction assignment to column-major dense matrices****************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Vectorized subtraction assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to column-major matrices (\f$ A-=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param C The target left-hand side dense matrix.
   // \param A The left-hand side dense matrix operand.
   // \param B The right-hand side sparse matrix operand.
   // \return void
   //
   // This function implements the vectorized subtraction assignment kernel for the transpose dense
   // matrix-transpose sparse matrix multiplication to a column-major dense matrix.
   */
   template< typename MT3    // Type of the left-hand side target matrix
           , typename MT4    // Type of the left-hand side matrix operand
           , typename MT5 >  // Type of the right-hand side matrix operand
   static inline typename EnableIf< UseVectorizedKernel<MT3,MT4,MT5> >::Type
      selectSubAssignKernel( DenseMatrix<MT3,true>& C, const MT4& A, const MT5& B )
   {
      typedef IntrinsicTrait<ElementType>  IT;
      typedef typename MT5::ConstIterator  ConstIterator;

      for( size_t j=0UL; j<B.columns(); ++j )
      {
         const ConstIterator end( B.end(j) );
         ConstIterator element( B.begin(j) );

         const size_t nonzeros( B.nonZeros(j) );
         const size_t kpos( nonzeros & size_t(-4) );
         BLAZE_INTERNAL_ASSERT( ( nonzeros - ( nonzeros % 4UL ) ) == kpos, "Invalid end calculation" );

         for( size_t k=0UL; k<kpos; k+=4UL )
         {
            const size_t        j1( element->index() );
            const IntrinsicType v1( set( element->value() ) );
            ++element;
            const size_t        j2( element->index() );
            const IntrinsicType v2( set( element->value() ) );
            ++element;
            const size_t        j3( element->index() );
            const IntrinsicType v3( set( element->value() ) );
            ++element;
            const size_t        j4( element->index() );
            const IntrinsicType v4( set( element->value() ) );
            ++element;

            BLAZE_INTERNAL_ASSERT( j1 < j2 && j2 < j3 && j3 < j4, "Invalid sparse matrix index detected" );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 & size_t(-IT::size) ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j4+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            for( size_t i=ibegin; i<iend; i+=IT::size ) {
               (~C).store( i, j, (~C).load(i,j) - A.load(i,j1) * v1 - A.load(i,j2) * v2 - A.load(i,j3) * v3 - A.load(i,j4) * v4 );
            }
         }

         for( ; element!=end; ++element )
         {
            const size_t        j1( element->index() );
            const IntrinsicType v1( set( element->value() ) );

            const size_t ibegin( ( IsLower<MT4>::value )?( j1 & size_t(-IT::size) ):( 0UL ) );
            const size_t iend  ( ( IsUpper<MT4>::value )?( j1+1UL ):( A.rows() ) );
            BLAZE_INTERNAL_ASSERT( ibegin <= iend, "Invalid loop indices detected" );

            for( size_t i=ibegin; i<iend; i+=IT::size ) {
               (~C).store( i, j, (~C).load(i,j) - A.load(i,j1) * v1 );
            }
         }
      }
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring subtraction assignment to row-major matrices**********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring subtraction assignment of a transpose dense matrix-transpose sparse
   //        matrix multiplication to a row-major matrix (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the symmetry-based restructuring subtraction assignment of a
   // transpose dense matrix-transpose sparse matrix multiplication expression to a row-major
   // matrix. Due to the explicit application of the SFINAE principle this function can only
   // be selected by the compiler in case the symmetry of either of the two matrix operands
   // can be exploited.
   */
   template< typename MT >  // Type of the target matrix
   friend inline typename EnableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      subAssign( Matrix<MT,false>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( IsSymmetric<MT1>::value && IsSymmetric<MT2>::value )
         subAssign( ~lhs, trans( rhs.lhs_ ) * trans( rhs.rhs_ ) );
      else if( IsSymmetric<MT1>::value )
         subAssign( ~lhs, trans( rhs.lhs_ ) * rhs.rhs_ );
      else
         subAssign( ~lhs, rhs.lhs_ * trans( rhs.rhs_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Subtraction assignment to sparse matrices***************************************************
   // No special implementation for the subtraction assignment to sparse matrices.
   //**********************************************************************************************

   //**Multiplication assignment to dense matrices*************************************************
   // No special implementation for the multiplication assignment to dense matrices.
   //**********************************************************************************************

   //**Multiplication assignment to sparse matrices************************************************
   // No special implementation for the multiplication assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP assignment to dense matrices************************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP assignment of a transpose dense matrix-transpose sparse matrix multiplication
   //        to a dense matrix (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized SMP assignment of a transpose dense
   // matrix-transpose sparse matrix multiplication expression to a dense matrix. Due to the
   // explicit application of the SFINAE principle this function can only be selected by the
   // compiler in case either of the two matrix operands requires an intermediate evaluation
   // and no symmetry can be exploited.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline typename EnableIf< IsEvaluationRequired<MT,MT1,MT2> >::Type
      smpAssign( DenseMatrix<MT,SO>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side sparse matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpAssign( ~lhs, A * B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP assignment to sparse matrices***********************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP assignment of a transpose dense matrix-transpose sparse matrix multiplication
   //        to a sparse matrix (\f$ A=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side sparse matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the performance optimized SMP assignment of a transpose dense
   // matrix-transpose sparse matrix multiplication expression to a sparse matrix. Due to the
   // explicit application of the SFINAE principle this function can only be selected by the
   // compiler in case either of the two matrix operands requires an intermediate evaluation
   // and no symmetry can be exploited.
   */
   template< typename MT  // Type of the target sparse matrix
           , bool SO >    // Storage order of the target sparse matrix
   friend inline typename EnableIf< IsEvaluationRequired<MT,MT1,MT2> >::Type
      smpAssign( SparseMatrix<MT,SO>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      typedef typename SelectType< SO, ResultType, OppositeType >::Type  TmpType;

      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( ResultType );
      BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE( OppositeType );
      BLAZE_CONSTRAINT_MATRICES_MUST_HAVE_SAME_STORAGE_ORDER( MT, TmpType );
      BLAZE_CONSTRAINT_MUST_BE_REFERENCE_TYPE( typename TmpType::CompositeType );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      const TmpType tmp( rhs );
      smpAssign( ~lhs, tmp );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring SMP assignment to row-major matrices******************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring SMP assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to a row-major matrix (\f$ C=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be assigned.
   // \return void
   //
   // This function implements the symmetry-based restructuring SMP assignment of a transpose
   // dense matrix-transpose sparse matrix multiplication expression to a row-major matrix. Due
   // to the explicit application of the SFINAE principle this function can only be selected by
   // the compiler in case the symmetry of either of the two matrix operands can be exploited.
   */
   template< typename MT >  // Type of the target matrix
   friend inline typename EnableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      smpAssign( Matrix<MT,false>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( IsSymmetric<MT1>::value && IsSymmetric<MT2>::value )
         smpAssign( ~lhs, trans( rhs.lhs_ ) * trans( rhs.rhs_ ) );
      else if( IsSymmetric<MT1>::value )
         smpAssign( ~lhs, trans( rhs.lhs_ ) * rhs.rhs_ );
      else
         smpAssign( ~lhs, rhs.lhs_ * trans( rhs.rhs_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP addition assignment to dense matrices***************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP addition assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to a dense matrix (\f$ A+=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the performance optimized SMP addition assignment of a transpose
   // dense matrix-transpose sparse matrix multiplication expression to a dense matrix. Due to
   // the explicit application of the SFINAE principle this function can only be selected by the
   // compiler in case either of the two matrix operands requires an intermediate evaluation and
   // no symmetry can be exploited.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline typename EnableIf< IsEvaluationRequired<MT,MT1,MT2> >::Type
      smpAddAssign( DenseMatrix<MT,SO>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side sparse matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpAddAssign( ~lhs, A * B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring SMP addition assignment to row-major matrices*********************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring SMP addition assignment of a transpose dense matrix-transpose sparse
   //        matrix multiplication to a row-major matrix (\f$ C+=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be added.
   // \return void
   //
   // This function implements the symmetry-based restructuring SMP addition assignment of a
   // transpose dense matrix-transpose sparse matrix multiplication expression to a row-major
   // matrix. Due to the explicit application of the SFINAE principle this function can only
   // be selected by the compiler in case the symmetry of either of the two matrix operands
   // can be exploited.
   */
   template< typename MT >  // Type of the target matrix
   friend inline typename EnableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      smpAddAssign( Matrix<MT,false>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( IsSymmetric<MT1>::value && IsSymmetric<MT2>::value )
         smpAddAssign( ~lhs, trans( rhs.lhs_ ) * trans( rhs.rhs_ ) );
      else if( IsSymmetric<MT1>::value )
         smpAddAssign( ~lhs, trans( rhs.lhs_ ) * rhs.rhs_ );
      else
         smpAddAssign( ~lhs, rhs.lhs_ * trans( rhs.rhs_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP addition assignment to sparse matrices**************************************************
   // No special implementation for the SMP addition assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP subtraction assignment to dense matrices************************************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief SMP subtraction assignment of a transpose dense matrix-transpose sparse matrix
   //        multiplication to a dense matrix (\f$ A-=B*C \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side dense matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the performance optimized SMP subtraction assignment of a transpose
   // dense matrix-transpose sparse matrix multiplication expression to a dense matrix. Due to
   // the explicit application of the SFINAE principle this function can only be selected by the
   // compiler in case either of the two matrix operands requires an intermediate evaluation and
   // no symmetry can be exploited.
   */
   template< typename MT  // Type of the target dense matrix
           , bool SO >    // Storage order of the target dense matrix
   friend inline typename EnableIf< IsEvaluationRequired<MT,MT1,MT2> >::Type
      smpSubAssign( DenseMatrix<MT,SO>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      LT A( rhs.lhs_ );  // Evaluation of the left-hand side dense matrix operand
      RT B( rhs.rhs_ );  // Evaluation of the right-hand side sparse matrix operand

      BLAZE_INTERNAL_ASSERT( A.rows()    == rhs.lhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( A.columns() == rhs.lhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( B.rows()    == rhs.rhs_.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == rhs.rhs_.columns(), "Invalid number of columns" );
      BLAZE_INTERNAL_ASSERT( A.rows()    == (~lhs).rows()     , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( B.columns() == (~lhs).columns()  , "Invalid number of columns" );

      smpSubAssign( ~lhs, A * B );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**Restructuring SMP subtraction assignment to row-major matrices******************************
   /*! \cond BLAZE_INTERNAL */
   /*!\brief Restructuring SMP subtraction assignment of a transpose dense matrix-transpose sparse
   //        matrix multiplication to a row-major matrix (\f$ C-=A*B \f$).
   // \ingroup dense_matrix
   //
   // \param lhs The target left-hand side matrix.
   // \param rhs The right-hand side multiplication expression to be subtracted.
   // \return void
   //
   // This function implements the symmetry-based restructuring SMP subtraction assignment of a
   // transpose dense matrix-transpose sparse matrix multiplication expression to a row-major
   // matrix. Due to the explicit application of the SFINAE principle this function can only be
   // selected by the compiler in case the symmetry of either of the two matrix operands can be
   // exploited.
   */
   template< typename MT >  // Type of the target matrix
   friend inline typename EnableIf< CanExploitSymmetry<MT,MT1,MT2> >::Type
      smpSubAssign( Matrix<MT,false>& lhs, const TDMatTSMatMultExpr& rhs )
   {
      BLAZE_FUNCTION_TRACE;

      BLAZE_CONSTRAINT_MUST_NOT_BE_SYMMETRIC_MATRIX_TYPE( MT );

      BLAZE_INTERNAL_ASSERT( (~lhs).rows()    == rhs.rows()   , "Invalid number of rows"    );
      BLAZE_INTERNAL_ASSERT( (~lhs).columns() == rhs.columns(), "Invalid number of columns" );

      if( IsSymmetric<MT1>::value && IsSymmetric<MT2>::value )
         smpSubAssign( ~lhs, trans( rhs.lhs_ ) * trans( rhs.rhs_ ) );
      else if( IsSymmetric<MT1>::value )
         smpSubAssign( ~lhs, trans( rhs.lhs_ ) * rhs.rhs_ );
      else
         smpSubAssign( ~lhs, rhs.lhs_ * trans( rhs.rhs_ ) );
   }
   /*! \endcond */
   //**********************************************************************************************

   //**SMP subtraction assignment to sparse matrices***********************************************
   // No special implementation for the SMP subtraction assignment to sparse matrices.
   //**********************************************************************************************

   //**SMP multiplication assignment to dense matrices*********************************************
   // No special implementation for the SMP multiplication assignment to dense matrices.
   //**********************************************************************************************

   //**SMP multiplication assignment to sparse matrices********************************************
   // No special implementation for the SMP multiplication assignment to sparse matrices.
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT1 );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( MT2 );
   BLAZE_CONSTRAINT_MUST_FORM_VALID_MATMATMULTEXPR( MT1, MT2 );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL BINARY ARITHMETIC OPERATORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Multiplication operator for the multiplication of a column-major dense matrix and a
//        column-major sparse matrix (\f$ A=B*C \f$).
// \ingroup dense_matrix
//
// \param lhs The left-hand side dense matrix for the multiplication.
// \param rhs The right-hand side sparse matrix for the multiplication.
// \return The resulting matrix.
// \exception std::invalid_argument Matrix sizes do not match.
//
// This operator represents the multiplication of a column-major dense matrix and a column-major
// sparse matrix:

   \code
   using blaze::columnMajor;

   blaze::DynamicMatrix<double,columnMajor> A, C;
   blaze::CompressedMatrix<double,columnMajor> B;
   // ... Resizing and initialization
   C = A * B;
   \endcode

// The operator returns an expression representing a dense matrix of the higher-order element
// type of the two involved matrix element types \a T1::ElementType and \a T2::ElementType.
// Both matrix types \a T1 and \a T2 as well as the two element types \a T1::ElementType and
// \a T2::ElementType have to be supported by the MultTrait class template.\n
// In case the current sizes of the two given matrices don't match, a \a std::invalid_argument
// is thrown.
*/
template< typename T1    // Type of the left-hand side dense matrix
        , typename T2 >  // Type of the right-hand side sparse matrix
inline const TDMatTSMatMultExpr<T1,T2>
   operator*( const DenseMatrix<T1,true>& lhs, const SparseMatrix<T2,true>& rhs )
{
   BLAZE_FUNCTION_TRACE;

   if( (~lhs).columns() != (~rhs).rows() )
      throw std::invalid_argument( "Matrix sizes do not match" );

   return TDMatTSMatMultExpr<T1,T2>( ~lhs, ~rhs );
}
//*************************************************************************************************




//=================================================================================================
//
//  ROWS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct Rows< TDMatTSMatMultExpr<MT1,MT2> >
   : public Rows<MT1>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  COLUMNS SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct Columns< TDMatTSMatMultExpr<MT1,MT2> >
   : public Columns<MT2>
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISLOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsLower< TDMatTSMatMultExpr<MT1,MT2> >
   : public IsTrue< And< IsLower<MT1>, IsLower<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUNILOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsUniLower< TDMatTSMatMultExpr<MT1,MT2> >
   : public IsTrue< And< IsUniLower<MT1>, IsUniLower<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSTRICTLYLOWER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsStrictlyLower< TDMatTSMatMultExpr<MT1,MT2> >
   : public IsTrue< Or< And< IsStrictlyLower<MT1>, IsLower<MT2> >
                      , And< IsStrictlyLower<MT2>, IsLower<MT1> > >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsUpper< TDMatTSMatMultExpr<MT1,MT2> >
   : public IsTrue< And< IsUpper<MT1>, IsUpper<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISUNIUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsUniUpper< TDMatTSMatMultExpr<MT1,MT2> >
   : public IsTrue< And< IsUniUpper<MT1>, IsUniUpper<MT2> >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  ISSTRICTLYUPPER SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct IsStrictlyUpper< TDMatTSMatMultExpr<MT1,MT2> >
   : public IsTrue< Or< And< IsStrictlyUpper<MT1>, IsUpper<MT2> >
                      , And< IsStrictlyUpper<MT2>, IsUpper<MT1> > >::value >
{};
/*! \endcond */
//*************************************************************************************************




//=================================================================================================
//
//  EXPRESSION TRAIT SPECIALIZATIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename VT >
struct TDMatDVecMultExprTrait< TDMatTSMatMultExpr<MT1,MT2>, VT >
{
 public:
   //**********************************************************************************************
   typedef typename SelectType< IsDenseMatrix<MT1>::value  && IsColumnMajorMatrix<MT1>::value &&
                                IsSparseMatrix<MT2>::value && IsColumnMajorMatrix<MT2>::value &&
                                IsDenseVector<VT>::value   && IsColumnVector<VT>::value
                              , typename TDMatDVecMultExprTrait< MT1, typename TSMatDVecMultExprTrait<MT2,VT>::Type >::Type
                              , INVALID_TYPE >::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, typename VT >
struct TDMatSVecMultExprTrait< TDMatTSMatMultExpr<MT1,MT2>, VT >
{
 public:
   //**********************************************************************************************
   typedef typename SelectType< IsDenseMatrix<MT1>::value  && IsColumnMajorMatrix<MT1>::value &&
                                IsSparseMatrix<MT2>::value && IsColumnMajorMatrix<MT2>::value &&
                                IsSparseVector<VT>::value  && IsColumnVector<VT>::value
                              , typename TDMatDVecMultExprTrait< MT1, typename TSMatDVecMultExprTrait<MT2,VT>::Type >::Type
                              , INVALID_TYPE >::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename VT, typename MT1, typename MT2 >
struct TDVecTDMatMultExprTrait< VT, TDMatTSMatMultExpr<MT1,MT2> >
{
 public:
   //**********************************************************************************************
   typedef typename SelectType< IsDenseVector<VT>::value   && IsRowVector<VT>::value          &&
                                IsDenseMatrix<MT1>::value  && IsColumnMajorMatrix<MT1>::value &&
                                IsSparseMatrix<MT2>::value && IsColumnMajorMatrix<MT2>::value
                              , typename TDVecTSMatMultExprTrait< typename TDVecTDMatMultExprTrait<VT,MT1>::Type, MT2 >::Type
                              , INVALID_TYPE >::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename VT, typename MT1, typename MT2 >
struct TSVecTDMatMultExprTrait< VT, TDMatTSMatMultExpr<MT1,MT2> >
{
 public:
   //**********************************************************************************************
   typedef typename SelectType< IsSparseVector<VT>::value  && IsRowVector<VT>::value          &&
                                IsDenseMatrix<MT1>::value  && IsColumnMajorMatrix<MT1>::value &&
                                IsSparseMatrix<MT2>::value && IsColumnMajorMatrix<MT2>::value
                              , typename TDVecTSMatMultExprTrait< typename TSVecTDMatMultExprTrait<VT,MT1>::Type, MT2 >::Type
                              , INVALID_TYPE >::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2, bool AF >
struct SubmatrixExprTrait< TDMatTSMatMultExpr<MT1,MT2>, AF >
{
 public:
   //**********************************************************************************************
   typedef typename MultExprTrait< typename SubmatrixExprTrait<const MT1,AF>::Type
                                 , typename SubmatrixExprTrait<const MT2,AF>::Type >::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct RowExprTrait< TDMatTSMatMultExpr<MT1,MT2> >
{
 public:
   //**********************************************************************************************
   typedef typename MultExprTrait< typename RowExprTrait<const MT1>::Type, MT2 >::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
template< typename MT1, typename MT2 >
struct ColumnExprTrait< TDMatTSMatMultExpr<MT1,MT2> >
{
 public:
   //**********************************************************************************************
   typedef typename MultExprTrait< MT1, typename ColumnExprTrait<const MT2>::Type >::Type  Type;
   //**********************************************************************************************
};
/*! \endcond */
//*************************************************************************************************

} // namespace blaze

#endif
