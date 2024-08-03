/******************************************************************************
 * Copyright (c) 2016, NVIDIA CORPORATION.  All rights reserved.
 * Modifications Copyright (c) 2019-2024, Advanced Micro Devices, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/
#pragma once

#include <thrust/detail/config.h>

#if THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HIP
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/system/hip/detail/transform.h>

THRUST_NAMESPACE_BEGIN
namespace hip_rocprim
{

template <class Derived, class ItemsIt, class MapIt, class ResultIt>
void THRUST_HIP_FUNCTION
scatter(execution_policy<Derived>& policy,
        ItemsIt                    first,
        ItemsIt                    last,
        MapIt                      map,
        ResultIt                   result)
{
    hip_rocprim::transform(policy,
                           first,
                           last,
                           thrust::make_permutation_iterator(result, map),
                           identity());
}

template <class Derived,
          class ItemsIt,
          class MapIt,
          class StencilIt,
          class ResultIt,
          class Predicate>
void THRUST_HIP_FUNCTION
scatter_if(execution_policy<Derived>& policy,
           ItemsIt                    first,
           ItemsIt                    last,
           MapIt                      map,
           StencilIt                  stencil,
           ResultIt                   result,
           Predicate                  predicate)
{
    hip_rocprim::transform_if(policy,
                              first,
                              last,
                              stencil,
                              thrust::make_permutation_iterator(result, map),
                              identity(),
                              predicate);
}

template <class Derived,
          class ItemsIt,
          class MapIt,
          class StencilIt,
          class ResultIt,
          class Predicate>
void THRUST_HIP_FUNCTION
scatter_if(execution_policy<Derived>& policy,
           ItemsIt                    first,
           ItemsIt                    last,
           MapIt                      map,
           StencilIt                  stencil,
           ResultIt                   result)
{
    hip_rocprim::scatter_if(
        policy, first, last, map, stencil, result, identity()
    );
}

} // namespace hip_rocprim
THRUST_NAMESPACE_END
#endif
