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


#ifndef CASADI_SX_FUNCTION_INTERNAL_HPP
#define CASADI_SX_FUNCTION_INTERNAL_HPP

#include "sx_function.hpp"
#include "x_function_internal.hpp"

#ifdef WITH_OPENCL
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#endif // WITH_OPENCL
/// \cond INTERNAL

namespace casadi {
#ifdef WITH_OPENCL
  /** \brief Singleton for the sparsity propagation kernel
      TODO: Move to a separate file and make non sparsity pattern specific
      \author Joel Andersson
      \date 2013
  */
  class SparsityPropagationKernel {
  public:
    // Default constructor
    SparsityPropagationKernel();

    // Destructor
    ~SparsityPropagationKernel();

    // Copy constructor and equality operator
    // (not implemented, declared to prevent use of the default ones)
    SparsityPropagationKernel(const SparsityPropagationKernel& sparsityPropagationKernel);
    SparsityPropagationKernel&
      operator=(const SparsityPropagationKernel& sparsityPropagationKernel);

    // Data members (all public)
    cl_device_id device_id;
    cl_context context;
    cl_command_queue command_queue;
    cl_platform_id platform_id;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
  };
#endif // WITH_OPENCL

/** \brief  Internal node class for SXFunction
  A regular user should never work with any Node class. Use SXFunction directly.
  \author Joel Andersson
  \date 2010
*/
class CASADI_CORE_EXPORT SXFunctionInternal :
        public XFunctionInternal<SXFunction, SXFunctionInternal, Matrix<SXElement>, SXNode>{
  friend class SXFunction;

  protected:
    /** \brief  Constructor (only to be called from SXFunction, therefore protected) */
    SXFunctionInternal(const std::vector<Matrix<SXElement> >& inputv,
                       const std::vector<Matrix<SXElement> >& outputv);

  public:

  /** \brief  Make a deep copy */
  virtual SXFunctionInternal* clone() const;

  /** \brief  Destructor */
  virtual ~SXFunctionInternal();

  /** \brief  Evaluate the function numerically */
  virtual void evaluate();

  /** \brief  Helper class to be plugged into evaluateGen when working
   * with a value known only at runtime */
  struct int_runtime {
    const int value;
    int_runtime(int v) : value(v) {}
  };

  /** \brief  Helper class to be plugged into evaluateGen when working
   * with a value known already at compiletime */
  template<int v>
  struct int_compiletime {
    static const int value = v;
  };

  /** \brief  evaluate symbolically while also propagating directional derivatives */
  virtual void evalSXsparse(const std::vector<SX>& arg, std::vector<SX>& res,
                      const std::vector<std::vector<SX> >& fseed,
                            std::vector<std::vector<SX> >& fsens,
                      const std::vector<std::vector<SX> >& aseed,
                            std::vector<std::vector<SX> >& asens);

  /** \brief  Check if smooth */
  bool isSmooth() const;

  /** \brief  Print the algorithm */
  virtual void print(std::ostream &stream) const;

  /** \brief Hessian (forward over adjoint) via source code transformation */
  SX hess(int iind=0, int oind=0);

  /** \brief  DATA MEMBERS */

  /** \brief  An element of the algorithm, namely a binary operation */
  typedef ScalarAtomic AlgEl;

  /** \brief  An element of the tape */
  template<typename T>
  struct TapeEl {
    T d[2];
  };

  /** \brief  all binary nodes of the tree in the order of execution */
  std::vector<AlgEl> algorithm_;

  /** \brief  Working vector for numeric calculation */
  std::vector<double> work_;

  /// work vector for symbolic calculations (allocated first time)
  std::vector<SXElement> s_work_;
  std::vector<SXElement> free_vars_;

  /// The expressions corresponding to each binary operation
  std::vector<SXElement> operations_;

  /// The expressions corresponding to each constant
  std::vector<SXElement> constants_;

  /** \brief  Initialize */
  virtual void init();

  /** \brief Generate code for the declarations of the C function */
  virtual void generateDeclarations(std::ostream &stream, const std::string& type,
                                    CodeGenerator& gen) const;

  /** \brief Generate code for the body of the C function */
  virtual void generateBody(std::ostream &stream, const std::string& type,
                            CodeGenerator& gen) const;

  /** \brief Clear the function from its symbolic representation, to free up memory,
   * no symbolic evaluations are possible after this */
  void clearSymbolic();

  /// Propagate a sparsity pattern through the algorithm
  virtual void spEvaluate(bool fwd);

  /// Is the class able to propagate seeds through the algorithm?
  virtual bool spCanEvaluate(bool fwd) { return true;}

  /// Reset the sparsity propagation
  virtual void spInit(bool fwd);

  /** \brief Return Jacobian of all input elements with respect to all output elements */
  virtual Function getFullJacobian();

  /// With just-in-time compilation using OpenCL
  bool just_in_time_opencl_;

  /// With just-in-time compilation for the sparsity propagation
  bool just_in_time_sparsity_;

#ifdef WITH_OPENCL
  // Initialize sparsity propagation using OpenCL
  void allocOpenCL();

  // Propagate sparsity using OpenCL
  void evaluateOpenCL();

  // Free memory for sparsity propagation using OpenCL
  void freeOpenCL();

  // Initialize sparsity propagation using OpenCL
  void spAllocOpenCL();

  // Propagate sparsity using OpenCL
  void spEvaluateOpenCL(bool fwd);

  // Free memory for sparsity propagation using OpenCL
  void spFreeOpenCL();

  // Compile OpenCL program
  static void compileProgram(cl_program program);

  // Execute OpenCL kernel
  static void executeKernel(cl_kernel kernel);

  // OpenCL memory object for the numerical evaluation
  cl_program program_;

  // OpenCL memory object for the sparsity propagation
  cl_program sp_program_;

  // Buffers and kernels for numerical evaluation
  std::vector<cl_mem> input_memobj_, output_memobj_;
  cl_kernel kernel_;

  // Buffers and kernels for sparsity propagation
  std::vector<cl_mem> sp_input_memobj_, sp_output_memobj_;
  cl_kernel sp_fwd_kernel_, sp_adj_kernel_;

  // OpenCL context. TODO: Nothing class specific in this class, move to a central location
  static SparsityPropagationKernel sparsity_propagation_kernel_;

#endif // WITH_OPENCL

};


} // namespace casadi

/// \endcond
#endif // CASADI_SX_FUNCTION_INTERNAL_HPP
