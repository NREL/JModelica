/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef CASADI_TRANSPOSE_HPP
#define CASADI_TRANSPOSE_HPP

#include "mx_node.hpp"
#include <map>
#include <stack>

/// \cond INTERNAL

namespace casadi {
  /** \brief Matrix transpose
      \author Joel Andersson
      \date 2013
  */
  class CASADI_CORE_EXPORT Transpose : public MXNode {
  public:

    /// Constructor
    Transpose(const MX& x);

    /// Clone function
    virtual Transpose* clone() const { return new Transpose(*this);}

    /// Destructor
    virtual ~Transpose() {}

    /// Evaluate the function numerically
    virtual void evaluateD(const DMatrixPtrV& input, DMatrixPtrV& output,
                           std::vector<int>& itmp, std::vector<double>& rtmp);

    /// Evaluate the function symbolically (SX)
    virtual void evaluateSX(const SXPtrV& input, SXPtrV& output, std::vector<int>& itmp,
                            std::vector<SXElement>& rtmp);

    /// Evaluate the function symbolically (MX)
    virtual void evaluateMX(const MXPtrV& input, MXPtrV& output, const MXPtrVV& fwdSeed,
                            MXPtrVV& fwdSens, const MXPtrVV& adjSeed,
                            MXPtrVV& adjSens, bool output_given);

    /// Propagate sparsity
    virtual void propagateSparsity(DMatrixPtrV& input, DMatrixPtrV& output,
                                   std::vector<int>& itmp, std::vector<double>& rtmp, bool fwd);

    /// Print a part of the expression */
    virtual void printPart(std::ostream &stream, int part) const;

    /** \brief Generate code for the operation */
    virtual void generateOperation(std::ostream &stream, const std::vector<std::string>& arg,
                                   const std::vector<std::string>& res, CodeGenerator& gen) const;

    /// Evaluate the function (template)
    template<typename T, typename MatV, typename MatVV>
    void evaluateGen(const MatV& input, MatV& output, std::vector<int>& itmp, std::vector<T>& rtmp);

    /** \brief Get the operation */
    virtual int getOp() const { return OP_TRANSPOSE;}

    /// Get number of temporary variables needed
    virtual void nTmp(size_t& ni, size_t& nr) { ni=size2()+1; nr=0;}

    /// Transpose
    virtual MX getTranspose() const { return dep();}

    /// Solve for square linear system
    //virtual MX getSolve(const MX& r, bool tr, const LinearSolver& linear_solver) const {
    // return dep()->getSolve(r, !tr, linear_solver);} // FIXME #1001

    /** \brief Check if two nodes are equivalent up to a given depth */
    virtual bool isEqual(const MXNode* node, int depth) const { return sameOpAndDeps(node, depth);}
  };

  /** \brief Matrix transpose (dense)
      \author Joel Andersson
      \date 2013
  */
  class CASADI_CORE_EXPORT DenseTranspose : public Transpose {
  public:

    /// Constructor
    DenseTranspose(const MX& x) : Transpose(x) {}

    /// Clone function
    virtual DenseTranspose* clone() const { return new DenseTranspose(*this);}

    /// Destructor
    virtual ~DenseTranspose() {}

    /// Evaluate the function numerically
    virtual void evaluateD(const DMatrixPtrV& input, DMatrixPtrV& output, std::vector<int>& itmp,
                           std::vector<double>& rtmp);

    /// Evaluate the function symbolically (SX)
    virtual void evaluateSX(const SXPtrV& input, SXPtrV& output, std::vector<int>& itmp,
                            std::vector<SXElement>& rtmp);

    /// Propagate sparsity
    virtual void propagateSparsity(DMatrixPtrV& input, DMatrixPtrV& output, std::vector<int>& itmp,
                                   std::vector<double>& rtmp, bool fwd);

    /** \brief Generate code for the operation */
    virtual void generateOperation(std::ostream &stream, const std::vector<std::string>& arg,
                                   const std::vector<std::string>& res, CodeGenerator& gen) const;

    /// Evaluate the function (template)
    template<typename T, typename MatV, typename MatVV>
    void evaluateGen(const MatV& input, MatV& output, std::vector<int>& itmp, std::vector<T>& rtmp);

    /// Get number of temporary variables needed
    virtual void nTmp(size_t& ni, size_t& nr) { ni=0; nr=0;}
  };



} // namespace casadi

#endif // CASADI_TRANSPOSE_HPP
