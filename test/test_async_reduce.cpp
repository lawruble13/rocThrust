#include <thrust/detail/config.h>

#if THRUST_CPP_DIALECT >= 2011 && !defined(THRUST_LEGACY_GCC)

#include <thrust/async/reduce.h>
#include <thrust/async/copy.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>

#include "test_header.hpp"

TESTS_DEFINE(AsyncReduceTests, NumericalTestsParams);

template <typename T>
struct custom_plus
{
  __host__ __device__
  T operator()(T lhs, T rhs) const
  {
    return lhs + rhs;
  }
};

#define DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(                                 \
    NAME, MEMBERS, CTOR, DTOR, VALIDATE, ...                                  \
  )                                                                           \
  template <typename T>                                                       \
  struct NAME                                                                 \
  {                                                                           \
    MEMBERS                                                                   \
                                                                              \
    NAME() { CTOR }                                                           \
                                                                              \
    ~NAME() { DTOR }                                                          \
                                                                              \
    template <typename Event>                                                 \
    void validate_event(Event& e)                                             \
    {                                                                         \
      THRUST_UNUSED_VAR(e);                                                   \
      VALIDATE                                                                \
    }                                                                         \
                                                                              \
    template <                                                                \
      typename ForwardIt, typename Sentinel                                   \
    >                                                                         \
    __host__                                                                  \
    auto operator()(                                                          \
      ForwardIt&& first, Sentinel&& last                                      \
    )                                                                         \
    THRUST_DECLTYPE_RETURNS(                                                  \
      ::thrust::async::reduce(                                                \
        __VA_ARGS__                                                           \
      )                                                                       \
    )                                                                         \
  };                                                                          \
  /**/

#define DEFINE_ASYNC_REDUCE_INVOKER(NAME, ...)                                \
  DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(                                       \
    NAME                                                                      \
  , THRUST_PP_EMPTY(), THRUST_PP_EMPTY(), THRUST_PP_EMPTY(), THRUST_PP_EMPTY()\
  , __VA_ARGS__                                                               \
  )                                                                           \
  /**/

#define DEFINE_SYNC_REDUCE_INVOKER(NAME, ...)                                 \
  template <typename T>                                                       \
  struct NAME                                                                 \
  {                                                                           \
                                                                              \
    template <                                                                \
      typename ForwardIt, typename Sentinel                                   \
    >                                                                         \
    __host__                                                                  \
    auto operator()(                                                          \
      ForwardIt&& first, Sentinel&& last                                      \
    )                                                                         \
    THRUST_DECLTYPE_RETURNS(                                                  \
      ::thrust::reduce(                                                       \
        __VA_ARGS__                                                           \
      )                                                                       \
    )                                                                         \
  };                                                                          \
  /**/

DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker
, THRUST_FWD(first), THRUST_FWD(last)
);
DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device
, thrust::device
, THRUST_FWD(first), THRUST_FWD(last)
);
DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_allocator
, thrust::device(thrust::device_allocator<void>{})
, THRUST_FWD(first), THRUST_FWD(last)
);
DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_on
  // Members.
, hipStream_t stream_;
  // Constructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamCreateWithFlags(&stream_, hipStreamNonBlocking)
  );
  // Destructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamDestroy(stream_)
  );
  // `validate_event` member.
, ASSERT_EQ_QUIET(stream_, e.stream().native_handle());
  // Arguments to `thrust::async::reduce`.
, thrust::device.on(stream_)
, THRUST_FWD(first), THRUST_FWD(last)
);
DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_allocator_on
  // Members.
, hipStream_t stream_;
  // Constructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamCreateWithFlags(&stream_, hipStreamNonBlocking)
  );
  // Destructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamDestroy(stream_)
  );
  // `validate_event` member.
, ASSERT_EQ_QUIET(stream_, e.stream().native_handle());
  // Arguments to `thrust::async::reduce`.
, thrust::device(thrust::device_allocator<void>{}).on(stream_)
, THRUST_FWD(first), THRUST_FWD(last)
);

DEFINE_SYNC_REDUCE_INVOKER(
  reduce_sync_invoker
, THRUST_FWD(first), THRUST_FWD(last)
);

DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_init
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
);
DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_init
, thrust::device
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
);
DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_allocator_init
, thrust::device(thrust::device_allocator<void>{})
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
);
DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_on_init
  // Members.
, hipStream_t stream_;
  // Constructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamCreateWithFlags(&stream_, hipStreamNonBlocking)
  );
  // Destructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamDestroy(stream_)
  );
  // `validate_event` member.
, ASSERT_EQ_QUIET(stream_, e.stream().native_handle());
  // Arguments to `thrust::async::reduce`.
, thrust::device.on(stream_)
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
);
DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_allocator_on_init
  // Members.
, hipStream_t stream_;
  // Constructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamCreateWithFlags(&stream_, hipStreamNonBlocking)
  );
  // Destructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamDestroy(stream_)
  );
  // `validate_event` member.
, ASSERT_EQ_QUIET(stream_, e.stream().native_handle());
  // Arguments to `thrust::async::reduce`.
, thrust::device(thrust::device_allocator<void>{}).on(stream_)
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
);

DEFINE_SYNC_REDUCE_INVOKER(
  reduce_sync_invoker_init
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
);

DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_init_plus
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, thrust::plus<T>()
);
DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_init_plus
, thrust::device
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, thrust::plus<T>()
);
DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_allocator_init_plus
, thrust::device(thrust::device_allocator<void>{})
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, thrust::plus<T>()
);
DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_on_init_plus
  // Members.
, hipStream_t stream_;
  // Constructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamCreateWithFlags(&stream_, hipStreamNonBlocking)
  );
  // Destructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamDestroy(stream_)
  );
  // `validate_event` member.
, ASSERT_EQ_QUIET(stream_, e.stream().native_handle());
  // Arguments to `thrust::async::reduce`.
, thrust::device.on(stream_)
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, thrust::plus<T>()
);
DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_allocator_on_init_plus
  // Members.
, hipStream_t stream_;
  // Constructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamCreateWithFlags(&stream_, hipStreamNonBlocking)
  );
  // Destructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamDestroy(stream_)
  );
  // `validate_event` member.
, ASSERT_EQ_QUIET(stream_, e.stream().native_handle());
  // Arguments to `thrust::async::reduce`.
, thrust::device(thrust::device_allocator<void>{}).on(stream_)
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, thrust::plus<T>()
);

DEFINE_SYNC_REDUCE_INVOKER(
  reduce_sync_invoker_init_plus
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, thrust::plus<T>()
);

DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_init_custom_plus
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, custom_plus<T>()
);
DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_init_custom_plus
, thrust::device
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, custom_plus<T>()
);
DEFINE_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_allocator_init_custom_plus
, thrust::device(thrust::device_allocator<void>{})
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, custom_plus<T>()
);
DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_on_init_custom_plus
  // Members.
, hipStream_t stream_;
  // Constructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamCreateWithFlags(&stream_, hipStreamNonBlocking)
  );
  // Destructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamDestroy(stream_)
  );
  // `validate_event` member.
, ASSERT_EQ_QUIET(stream_, e.stream().native_handle());
  // Arguments to `thrust::async::reduce`.
, thrust::device.on(stream_)
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, custom_plus<T>()
);
DEFINE_STATEFUL_ASYNC_REDUCE_INVOKER(
  reduce_async_invoker_device_allocator_on_init_custom_plus
  // Members.
, hipStream_t stream_;
  // Constructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamCreateWithFlags(&stream_, hipStreamNonBlocking)
  );
  // Destructor.
, thrust::hip_rocprim::throw_on_error(
    hipStreamDestroy(stream_)
  );
  // `validate_event` member.
, ASSERT_EQ_QUIET(stream_, e.stream().native_handle());
  // Arguments to `thrust::async::reduce`.
, thrust::device(thrust::device_allocator<void>{}).on(stream_)
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, custom_plus<T>()
);

DEFINE_SYNC_REDUCE_INVOKER(
  reduce_sync_invoker_init_custom_plus
, THRUST_FWD(first), THRUST_FWD(last)
, unittest::random_integer<T>()
, custom_plus<T>()
);

///////////////////////////////////////////////////////////////////////////////

template <
  T
, template <typename> class AsyncReduceInvoker
, template <typename> class SyncReduceInvoker
>
void testAsyncReduce()
{
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);
        for(size_t seed_index = 0; seed_index < random_seeds_count + seed_size; seed_index++)
        {
            unsigned int seed_value
                = seed_index < random_seeds_count ? rand() : seeds[seed_index - random_seeds_count];
            SCOPED_TRACE(testing::Message() << "with seed= " << seed_value);

            thrust::host_vector<T> h0 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value);

            thrust::device_vector<T> d0a(h0);
            thrust::device_vector<T> d0b(h0);
            thrust::device_vector<T> d0c(h0);
            thrust::device_vector<T> d0d(h0);

            AsyncReduceInvoker<T> invoke_async;
            SyncReduceInvoker<T>  invoke_sync;

            ASSERT_EQ(h0, d0a);
            ASSERT_EQ(h0, d0b);
            ASSERT_EQ(h0, d0c);
            ASSERT_EQ(h0, d0d);

            auto f0a = invoke_async(d0a.begin(), d0a.end());
            auto f0b = invoke_async(d0b.begin(), d0b.end());
            auto f0c = invoke_async(d0c.begin(), d0c.end());
            auto f0d = invoke_async(d0d.begin(), d0d.end());

            invoke_async.validate_event(f0a);
            invoke_async.validate_event(f0b);
            invoke_async.validate_event(f0c);
            invoke_async.validate_event(f0d);

            // This potentially runs concurrently with the copies.
            auto const r0 = invoke_sync(h0.begin(), h0.end());

            auto const r1a = TEST_FUTURE_VALUE_RETRIEVAL(f0a);
            auto const r1b = TEST_FUTURE_VALUE_RETRIEVAL(f0b);
            auto const r1c = TEST_FUTURE_VALUE_RETRIEVAL(f0c);
            auto const r1d = TEST_FUTURE_VALUE_RETRIEVAL(f0d);

            ASSERT_EQ(r0, r1a);
            ASSERT_EQ(r0, r1b);
            ASSERT_EQ(r0, r1c);
            ASSERT_EQ(r0, r1d);
        }
    }
};

TYPED_TEST(AsyncReduceTests, TestAsyncReduce)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker,
        reduce_sync_invoker
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicy)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device,
        reduce_sync_invoker
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyAllocator)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_on,
        reduce_sync_invoker
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyOn)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_on,
        reduce_sync_invoker
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyAllocatorOn)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_allocator_on,
        reduce_sync_invoker
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReduceInit)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_init,
        reduce_sync_invoker_init
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyInit)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_init,
        reduce_sync_invoker_init
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyAllocatorInit)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_allocator_init,
        reduce_sync_invoker_init
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyOnInit)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_on_init,
        reduce_sync_invoker_init
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyAllocatorOnInit)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_allocator_on_init,
        reduce_sync_invoker_init
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReduceInitPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_init_plus,
        reduce_sync_invoker_init_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyInitPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_init_plus,
        reduce_sync_invoker_init_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyAllocatorInitPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_allocator_init_plus,
        reduce_sync_invoker_init_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyOnInitPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_on_init_plus,
        reduce_sync_invoker_init_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyAllocatorOnInitPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_allocator_on_init_plus,
        reduce_sync_invoker_init_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReduceInitCustomPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_init_custom_plus,
        reduce_sync_invoker_init_custom_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyInitCustomPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_init_custom_plus,
        reduce_sync_invoker_init_custom_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyAllocatorInitCustomPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_allocator_init_custom_plus,
        reduce_sync_invoker_init_custom_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyOnInitCustomPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_on_init_custom_plus,
        reduce_sync_invoker_init_custom_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyAllocatorOnInitCustomPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduce<
        T,
        reduce_async_invoker_device_allocator_on_init_custom_plus,
        reduce_sync_invoker_init_custom_plus
    >();
}

///////////////////////////////////////////////////////////////////////////////

template <
  T
, template <typename> class AsyncReduceInvoker
, template <typename> class SyncReduceInvoker
>
void testAsyncReduceCountingIterator()
{
    constexpr std::size_t n = 15 * sizeof(T);

    ASSERT_LEQUAL(T(n), unittest::truncate_to_max_representable<T>(n));

    thrust::counting_iterator<T> first(0);
    thrust::counting_iterator<T> last(n);

    AsyncReduceInvoker<T> invoke_async;
    SyncReduceInvoker<T>  invoke_sync;

    auto f0a = invoke_async(first, last);
    auto f0b = invoke_async(first, last);
    auto f0c = invoke_async(first, last);
    auto f0d = invoke_async(first, last);

    invoke_async.validate_event(f0a);
    invoke_async.validate_event(f0b);
    invoke_async.validate_event(f0c);
    invoke_async.validate_event(f0d);

    // This potentially runs concurrently with the copies.
    auto const r0 = invoke_sync(first, last);

    auto const r1a = TEST_FUTURE_VALUE_RETRIEVAL(f0a);
    auto const r1b = TEST_FUTURE_VALUE_RETRIEVAL(f0b);
    auto const r1c = TEST_FUTURE_VALUE_RETRIEVAL(f0c);
    auto const r1d = TEST_FUTURE_VALUE_RETRIEVAL(f0d);

    ASSERT_EQ(r0, r1a);
    ASSERT_EQ(r0, r1b);
    ASSERT_EQ(r0, r1c);
    ASSERT_EQ(r0, r1d);
};

TYPED_TEST(AsyncReduceTests, TestAsyncReduceCountingIterator)
{
    using T = typename TestFixture::input_type;
    testAsyncReduceCountingIterator<
        T,
        reduce_async_invoker,
        reduce_sync_invoker
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyCountingIterator)
{
    using T = typename TestFixture::input_type;
    testAsyncReduceCountingIterator<
        T,
        reduce_async_invoker_device,
        reduce_sync_invoker
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReduceCountingIteratorInit)
{
    using T = typename TestFixture::input_type;
    testAsyncReduceCountingIterator<
        T,
        reduce_async_invoker_init,
        reduce_sync_invoker_init
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyCountingIteratorInit)
{
    using T = typename TestFixture::input_type;
    testAsyncReduceCountingIterator<
        T,
        reduce_async_invoker_device_init,
        reduce_sync_invoker_init
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReduceCountingIteratorInitPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduceCountingIterator<
        T,
        reduce_async_invoker_init_plus,
        reduce_sync_invoker_init_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyCountingIteratorInitPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduceCountingIterator<
        T,
        reduce_async_invoker_device_init_plus,
        reduce_sync_invoker_init_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReduceCountingIteratorInitCustomPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduceCountingIterator<
        T,
        reduce_async_invoker_init_custom_plus,
        reduce_sync_invoker_init_custom_plus
    >();
}

TYPED_TEST(AsyncReduceTests, TestAsyncReducePolicyCountingIteratorInitCustomPlus)
{
    using T = typename TestFixture::input_type;
    testAsyncReduceCountingIterator<
        T,
        reduce_async_invoker_device_init_custom_plus,
        reduce_sync_invoker_init_custom_plus
    >();
}

///////////////////////////////////////////////////////////////////////////////

TYPED_TEST(AsyncReduceTests, TestAsyncReduceUsing)
{
    using T = typename TestFixture::input_type;
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);
        for(size_t seed_index = 0; seed_index < random_seeds_count + seed_size; seed_index++)
        {
            unsigned int seed_value
                = seed_index < random_seeds_count ? rand() : seeds[seed_index - random_seeds_count];
            SCOPED_TRACE(testing::Message() << "with seed= " << seed_value);

            thrust::host_vector<T> h0 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value);

            thrust::device_vector<T> d0a(h0);
            thrust::device_vector<T> d0b(h0);

            ASSERT_EQ(h0, d0a);
            ASSERT_EQ(h0, d0b);

            thrust::device_future<T> f0a;
            thrust::device_future<T> f0b;

            // When you import the customization points into the global namespace,
            // they should be selected instead of the synchronous algorithms.
            {
                using namespace thrust::async;
                f0a = reduce(d0a.begin(), d0a.end());
            }
            {
                using thrust::async::reduce;
                f0b = reduce(d0b.begin(), d0b.end());
            }

            // ADL should find the synchronous algorithms.
            // This potentially runs concurrently with the copies.
            T const r0 = reduce(h0.begin(), h0.end());

            T const r1a = TEST_FUTURE_VALUE_RETRIEVAL(f0a);
            T const r1b = TEST_FUTURE_VALUE_RETRIEVAL(f0b);

            ASSERT_EQ(r0, r1a);
            ASSERT_EQ(r0, r1b);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////

TYPED_TEST(AsyncReduceTests, TestAsyncReduceAfter)
{
    using T = typename TestFixture::input_type;
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);
        for(size_t seed_index = 0; seed_index < random_seeds_count + seed_size; seed_index++)
        {
            unsigned int seed_value
                = seed_index < random_seeds_count ? rand() : seeds[seed_index - random_seeds_count];
            SCOPED_TRACE(testing::Message() << "with seed= " << seed_value);

            thrust::host_vector<T> h0 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value);

            thrust::device_vector<T> d0(h0);

            ASSERT_EQ(h0, d0);

            auto f0 = thrust::async::reduce(
                d0.begin(), d0.end()
            );

            ASSERT_EQ(true, f0.valid_stream());

            auto const f0_stream = f0.stream().native_handle();

            auto f1 = thrust::async::reduce(
                thrust::device.after(f0), d0.begin(), d0.end()
            );

            // Verify that double consumption of a future produces an exception.
            ASSERT_THROWS_EQUAL(
              auto x = thrust::async::reduce(
                  thrust::device.after(f0), d0.begin(), d0.end()
              );
              THRUST_UNUSED_VAR(x)
            , thrust::event_error
            , thrust::event_error(thrust::event_errc::no_state)
            );

            ASSERT_EQ_QUIET(f0_stream, f1.stream().native_handle());

            auto after_policy2 = thrust::device.after(f1);

            auto f2 = thrust::async::reduce(
                after_policy2, d0.begin(), d0.end()
            );

            // Verify that double consumption of a policy produces an exception.
            ASSERT_THROWS_EQUAL(
              auto x = thrust::async::reduce(
                after_policy2, d0.begin(), d0.end()
              );
              THRUST_UNUSED_VAR(x)
            , thrust::event_error
            , thrust::event_error(thrust::event_errc::no_state)
            );

            ASSERT_EQ_QUIET(f0_stream, f2.stream().native_handle());

            // This potentially runs concurrently with the copies.
            T const r0 = thrust::reduce(h0.begin(), h0.end());

            T const r1 = TEST_FUTURE_VALUE_RETRIEVAL(f2);

            ASSERT_EQ(r0, r1);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////

TYPED_TEST(AsyncReduceTests, TestAsyncReduceOnThenAfter)
{
    using T = typename TestFixture::input_type;
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);
        for(size_t seed_index = 0; seed_index < random_seeds_count + seed_size; seed_index++)
        {
            unsigned int seed_value
                = seed_index < random_seeds_count ? rand() : seeds[seed_index - random_seeds_count];
            SCOPED_TRACE(testing::Message() << "with seed= " << seed_value);

            thrust::host_vector<T> h0 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value);

            thrust::device_vector<T> d0(h0);

            ASSERT_EQ(h0, d0);

            hipStream_t stream;
            thrust::hip_rocprim::throw_on_error(
                hipStreamCreateWithFlags(&stream, hipStreamNonBlocking)
            );

            auto f0 = thrust::async::reduce(
                thrust::device.on(stream), d0.begin(), d0.end()
            );

            ASSERT_EQ_QUIET(stream, f0.stream().native_handle());

            auto f1 = thrust::async::reduce(
                thrust::device.after(f0), d0.begin(), d0.end()
            );

            // Verify that double consumption of a future produces an exception.
            ASSERT_THROWS_EQUAL(
              auto x = thrust::async::reduce(
                thrust::device.after(f0), d0.begin(), d0.end()
              );
              THRUST_UNUSED_VAR(x)
            , thrust::event_error
            , thrust::event_error(thrust::event_errc::no_state)
            );

            ASSERT_EQ_QUIET(stream, f1.stream().native_handle());

            auto after_policy2 = thrust::device.after(f1);

            auto f2 = thrust::async::reduce(
              after_policy2, d0.begin(), d0.end()
            );

            // Verify that double consumption of a policy produces an exception.
            ASSERT_THROWS_EQUAL(
              auto x = thrust::async::reduce(
                after_policy2, d0.begin(), d0.end()
              );
              THRUST_UNUSED_VAR(x)
            , thrust::event_error
            , thrust::event_error(thrust::event_errc::no_state)
            );

            ASSERT_EQ_QUIET(stream, f2.stream().native_handle());

            // This potentially runs concurrently with the copies.
            T const r0 = thrust::reduce(h0.begin(), h0.end());

            T const r1 = TEST_FUTURE_VALUE_RETRIEVAL(f2);

            ASSERT_EQ(r0, r1);

            thrust::hip_rocprim::throw_on_error(
                hipStreamDestroy(stream)
            );
        }
    }
};

///////////////////////////////////////////////////////////////////////////////

/*
TYPED_TEST(AsyncReduceTests, TestAsyncReduceAllocatorOnThenAfter)
{
    using T = typename TestFixture::input_type;
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);
        for(size_t seed_index = 0; seed_index < random_seeds_count + seed_size; seed_index++)
        {
            unsigned int seed_value
                = seed_index < random_seeds_count ? rand() : seeds[seed_index - random_seeds_count];
            SCOPED_TRACE(testing::Message() << "with seed= " << seed_value);

            thrust::host_vector<T> h0 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value);

            thrust::device_vector<T> d0(h0);

            ASSERT_EQ(h0, d0);

            hipStream_t stream0;
            thrust::hip_rocprim::throw_on_error(
                hipStreamCreateWithFlags(&stream0, hipStreamNonBlocking)
            );

            hipStream_t stream1;
            thrust::hip_rocprim::throw_on_error(
                hipStreamCreateWithFlags(&stream1, hipStreamNonBlocking)
            );

            auto f0 = thrust::async::reduce(
              thrust::device(thrust::device_allocator<void>{}).on(stream0)
            , d0.begin(), d0.end()
            );

            ASSERT_EQ_QUIET(stream0, f0.stream().native_handle());

            auto f1 = thrust::async::reduce(
              thrust::device(thrust::device_allocator<void>{}).after(f0)
            , d0.begin(), d0.end()
            );

            ASSERT_THROWS_EQUAL(
              auto x = thrust::async::reduce(
                  thrust::device(thrust::device_allocator<void>{}).after(f0)
              , d0.begin(), d0.end()
              );
              THRUST_UNUSED_VAR(x)
            , thrust::event_error
            , thrust::event_error(thrust::event_errc::no_state)
            );

            ASSERT_EQ_QUIET(stream0, f1.stream().native_handle());

            auto f2 = thrust::async::reduce(
              thrust::device(thrust::device_allocator<void>{}).on(stream1).after(f1)
            , d0.begin(), d0.end()
            );

            ASSERT_THROWS_EQUAL(
              auto x = thrust::async::reduce(
                  thrust::device(thrust::device_allocator<void>{}).on(stream1).after(f1)
              , d0.begin(), d0.end()
              );
              THRUST_UNUSED_VAR(x)
            , thrust::event_error
            , thrust::event_error(thrust::event_errc::no_state)
            );

            KNOWN_FAILURE;
            // FIXME: The below fails because you can't combine allocator attachment,
            // `.on`, and `.after`.
            ASSERT_EQ_QUIET(stream1, f2.stream().native_handle());

            // This potentially runs concurrently with the copies.
            T const r0 = thrust::reduce(h0.begin(), h0.end());

            T const r1 = TEST_FUTURE_VALUE_RETRIEVAL(f2);

            ASSERT_EQ(r0, r1);

            thrust::hip_rocprim::throw_on_error(hipStreamDestroy(stream0));
            thrust::hip_rocprim::throw_on_error(hipStreamDestroy(stream1));
        }
    }
}*/

///////////////////////////////////////////////////////////////////////////////

TYPED_TEST(AsyncReduceTests, TestAsyncReduceCaching)
{
    using T = typename TestFixture::input_type;
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);
        for(size_t seed_index = 0; seed_index < random_seeds_count + seed_size; seed_index++)
        {
            unsigned int seed_value
                = seed_index < random_seeds_count ? rand() : seeds[seed_index - random_seeds_count];
            SCOPED_TRACE(testing::Message() << "with seed= " << seed_value);

            thrust::host_vector<T> h0 = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value);

            constexpr std::int64_t m = 32;
            thrust::device_vector<T> d0(h0);

            ASSERT_EQ(h0, d0);

            T const* f0_raw_data;

            {
            // Perform one reduction to ensure there's an entry in the caching
            // allocator.
            auto f0 = thrust::async::reduce(d0.begin(), d0.end());

            TEST_EVENT_WAIT(f0);

            f0_raw_data = f0.raw_data();
            }

            for (std::int64_t i = 0; i < m; ++i)
            {
                auto f1 = thrust::async::reduce(d0.begin(), d0.end());

                ASSERT_EQ(true, f1.valid_stream());
                ASSERT_EQ(true, f1.valid_content());

                ASSERT_EQ_QUIET(f0_raw_data, f1.raw_data());

                // This potentially runs concurrently with the copies.
                T const r0 = thrust::reduce(h0.begin(), h0.end());

                T const r1 = TEST_FUTURE_VALUE_RETRIEVAL(f1);

                ASSERT_EQ(r0, r1);
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////

TYPED_TEST(AsyncReduceTests, TestAsyncCopyThenReduce)
{
    using T = typename TestFixture::input_type;
    for(auto size : get_sizes())
    {
        SCOPED_TRACE(testing::Message() << "with size = " << size);
        for(size_t seed_index = 0; seed_index < random_seeds_count + seed_size; seed_index++)
        {
            unsigned int seed_value
                = seed_index < random_seeds_count ? rand() : seeds[seed_index - random_seeds_count];
            SCOPED_TRACE(testing::Message() << "with seed= " << seed_value);

            thrust::host_vector<T> h0a = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value);
            thrust::host_vector<T> h0b = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value + 1);
            thrust::host_vector<T> h0c = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value + 2);
            thrust::host_vector<T> h0d = get_random_data<T>(
                size, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), seed_value + 3);

            thrust::device_vector<T> d0a(size);
            thrust::device_vector<T> d0b(size);
            thrust::device_vector<T> d0c(size);
            thrust::device_vector<T> d0d(size);

            auto f0a = thrust::async::copy(h0a.begin(), h0a.end(), d0a.begin());
            auto f0b = thrust::async::copy(h0b.begin(), h0b.end(), d0b.begin());
            auto f0c = thrust::async::copy(h0c.begin(), h0c.end(), d0c.begin());
            auto f0d = thrust::async::copy(h0d.begin(), h0d.end(), d0d.begin());

            ASSERT_EQ(true, f0a.valid_stream());
            ASSERT_EQ(true, f0b.valid_stream());
            ASSERT_EQ(true, f0c.valid_stream());
            ASSERT_EQ(true, f0d.valid_stream());

            auto const f0a_stream = f0a.stream().native_handle();
            auto const f0b_stream = f0b.stream().native_handle();
            auto const f0c_stream = f0c.stream().native_handle();
            auto const f0d_stream = f0d.stream().native_handle();

            auto f1a = thrust::async::reduce(
              thrust::device.after(f0a), d0a.begin(), d0a.end()
            );
            auto f1b = thrust::async::reduce(
              thrust::device.after(f0b), d0b.begin(), d0b.end()
            );
            auto f1c = thrust::async::reduce(
              thrust::device.after(f0c), d0c.begin(), d0c.end()
            );
            auto f1d = thrust::async::reduce(
              thrust::device.after(f0d), d0d.begin(), d0d.end()
            );

            ASSERT_EQ(false, f0a.valid_stream());
            ASSERT_EQ(false, f0b.valid_stream());
            ASSERT_EQ(false, f0c.valid_stream());
            ASSERT_EQ(false, f0d.valid_stream());

            ASSERT_EQ(true, f1a.valid_stream());
            ASSERT_EQ(true, f1a.valid_content());
            ASSERT_EQ(true, f1b.valid_stream());
            ASSERT_EQ(true, f1b.valid_content());
            ASSERT_EQ(true, f1c.valid_stream());
            ASSERT_EQ(true, f1c.valid_content());
            ASSERT_EQ(true, f1d.valid_stream());
            ASSERT_EQ(true, f1d.valid_content());

            // Verify that streams were stolen.
            ASSERT_EQ_QUIET(f0a_stream, f1a.stream().native_handle());
            ASSERT_EQ_QUIET(f0b_stream, f1b.stream().native_handle());
            ASSERT_EQ_QUIET(f0c_stream, f1c.stream().native_handle());
            ASSERT_EQ_QUIET(f0d_stream, f1d.stream().native_handle());

            // This potentially runs concurrently with the copies.
            T const r0 = thrust::reduce(h0a.begin(), h0a.end());

            T const r1a = TEST_FUTURE_VALUE_RETRIEVAL(f1a);
            T const r1b = TEST_FUTURE_VALUE_RETRIEVAL(f1b);
            T const r1c = TEST_FUTURE_VALUE_RETRIEVAL(f1c);
            T const r1d = TEST_FUTURE_VALUE_RETRIEVAL(f1d);

            ASSERT_EQ(r0, r1a);
            ASSERT_EQ(r0, r1b);
            ASSERT_EQ(r0, r1c);
            ASSERT_EQ(r0, r1d);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

// TODO: when_all from reductions.

#endif
