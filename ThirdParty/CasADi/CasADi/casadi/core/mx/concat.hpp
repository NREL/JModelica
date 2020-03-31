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


#ifndef CASADI_CONCAT_HPP
#define CASADI_CONCAT_HPP

#include "mx_node.hpp"
#include <map>
#include <stack>

/// \cond INTERNAL

namespace casadi {
  /** \brief Concatenation: Join multiple expressions stacking the nonzeros
      \author Joel Andersson
      \date 2014
  */
  class CASADI_CORE_EXPORT Concat : public MXNode {
  public:

    /// Constructor
    Concat(const std::vector<MX>& x);

    /// Destructor
    virtual ~Concat() = 0;

    /// Evaluate the function numerically
    virtual void evaluateD(const DMatrixPtrV& input, DMatrixPtrV& output, std::vector<int>& itmp,
                           std::vector<double>& rtmp);

    /// Evaluate the function symbolically (SX)
    virtual void evaluateSX(const SXPtrV& input, SXPtrV& output, std::vector<int>& itmp,
                            std::vector<SXElement>& rtmp);

    /// Evaluate the function (template)
    template<typename T, typename MatV, typename MatVV>
    void evaluateGen(const MatV& input, MatV& output, std::vector<int>& itmp, std::vector<T>& rtmp);

    /// Propagate sparsity
    virtual void propagateSparsity(DMatrixPtrV& input, DMatrixPtrV& output, bool fwd);

    /** \brief Generate code for the operation */
    virtual void generateOperation(std::ostream &stream, const std::vector<std::string>& arg,
                                   const std::vector<std::string>& res, CodeGenerator& gen) const;

    /// Get the nonzeros of matrix
    virtual MX getGetNonzeros(const Sparsity& sp, const std::vector<int>& nz) const;

    /** \brief Check if two nodes are equivalent up to a given depth */
    virtual bool isEqual(const MXNode* node, int depth) const { return sameOpAndDeps(node, depth);}
  };


  /** \brief Horizontal concatenation
      \author Joel Andersson
      \date 2013
  */
  class CASADI_CORE_EXPORT Horzcat : public Concat {
  public:

    /// Constructor
    Horzcat(const std::vector<MX>& x);

    /// Clone function
    virtual Horzcat* clone() const { return new Horzcat(*this);}

    /// Destructor
    virtual ~Horzcat() {}

    /// Print a part of the expression */
    virtual void printPart(std::ostream &stream, int part) const;

    /// Evaluate the function symbolically (MX)
    virtual void evaluateMX(const MXPtrV& input, MXPtrV& output, const MXPtrVV& fwdSeed,
                            MXPtrVV& fwdSens, const MXPtrVV& adjSeed, MXPtrVV& adjSens,
                            bool output_given);

    /** \brief Get the operation */
    virtual int getOp() const { return OP_HORZCAT;}
  };

  /** \brief Vertical concatenation of vectors
      \author Joel Andersson
      \date 2014
  */
  class CASADI_CORE_EXPORT Vertcat : public Concat {
  public:

    /// Constructor
    Vertcat(const std::vector<MX>& x);

    /// Clone function
    virtual Vertcat* clone() const { return new Vertcat(*this);}

    /// Destructor
    virtual ~Vertcat() {}

    /// Print a part of the expression */
    virtual void printPart(std::ostream &stream, int part) const;

    /// Evaluate the function symbolically (MX)
    virtual void evaluateMX(const MXPtrV& input, MXPtrV& output, const MXPtrVV& fwdSeed,
                            MXPtrVV& fwdSens, const MXPtrVV& adjSeed, MXPtrVV& adjSens,
                            bool output_given);

    /** \brief Get the operation */
    virtual int getOp() const { return OP_VERTCAT;}
  };

  /** \brief Diagonal concatenation of matrices
      \author Joris Gillis
      \date 2014
  */
  class CASADI_CORE_EXPORT Diagcat : public Concat {
  public:

    /// Constructor
    Diagcat(const std::vector<MX>& x);

    /// Clone function
    virtual Diagcat* clone() const { return new Diagcat(*this);}

    /// Destructor
    virtual ~Diagcat() {}

    /// Print a part of the expression */
    virtual void printPart(std::ostream &stream, int part) const;

    /// Evaluate the function symbolically (MX)
    virtual void evaluateMX(const MXPtrV& input, MXPtrV& output, const MXPtrVV& fwdSeed,
                            MXPtrVV& fwdSens, const MXPtrVV& adjSeed, MXPtrVV& adjSens,
                            bool output_given);

    /** \brief Get the operation */
    virtual int getOp() const { return OP_DIAGCAT;}
  };

} // namespace casadi
/// \endcond

#endif // CASADI_CONCAT_HPP
