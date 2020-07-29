// Copyright (c) 2018 NVIDIA Corporation
// Modifications Copyright© 2019 Advanced Micro Devices, Inc. All rights reserved.
// Author: Bryce Adelstein Lelbach <brycelelbach@gmail.com>
//
// Distributed under the Boost Software License v1.0 (boost.org/LICENSE_1_0.txt)

#pragma once

#include <thrust/detail/config.h>
#include <thrust/detail/cpp11_required.h>

#if THRUST_CPP_DIALECT >= 2011

#include <thrust/system/hip/pointer.h>
#include <thrust/system/hip/detail/execution_policy.h>

THRUST_BEGIN_NS

namespace system { namespace hip
{

struct ready_event;

template <typename T>
struct ready_future;

struct unique_eager_event;

template <typename T>
struct unique_eager_future;

template <typename... Events>
__host__
unique_eager_event when_all(Events&&... evs);

}} // namespace system::hip

namespace hip
{

using thrust::system::hip::ready_event;

using thrust::system::hip::ready_future;

using thrust::system::hip::unique_eager_event;
using event = unique_eager_event;

using thrust::system::hip::unique_eager_future;
template <typename T> using future = unique_eager_future<T>;

using thrust::system::hip::when_all;

} // namespace hip

template <typename DerivedPolicy>
__host__
thrust::hip::unique_eager_event
unique_eager_event_type(
  thrust::hip::execution_policy<DerivedPolicy> const&
) noexcept;

template <typename T, typename DerivedPolicy>
__host__
thrust::hip::unique_eager_future<T>
unique_eager_future_type(
  thrust::hip::execution_policy<DerivedPolicy> const&
) noexcept;

THRUST_END_NS

#include <thrust/system/hip/detail/future.inl>

#endif // THRUST_CPP_DIALECT >= 2011
