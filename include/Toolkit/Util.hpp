/*
 * =====================================================================================
 *
 *       Filename:  Util.hpp
 *
 *    Description:  Util
 *
 *        Version:  1.0
 *        Created:  2022年08月10日 16时03分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (vz), vyouzhi@gmail.com
 *   Organization:  Etomc2.com
 *        LICENSE:  BSD-3-Clause license
 *
 *  Copyright (c) 2019-2022, vyouzhi
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *  * Neither the name of vyouzhi and/or the DataFrame nor the
 *  names of its contributors may be used to endorse or promote products
 *  derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL vyouzhi BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =====================================================================================
 */

#ifndef UTIL_INC
#define UTIL_INC

#include <thread>
#include <tuple>
#include <utility>
namespace e2q {

/**
 * for each tuple
 *   tuple<Ts...> args_tuple
 *   auto fe = [this]<typename T, std::size_t I = 0>(T t) { add(t, I); };
 *
 *   for_each_in_tuple(fe, args_tuple);
 */
template <std::size_t I = 0, class Fn, class Tuple, class... Tuples>
constexpr
    typename std::enable_if<I == std::tuple_size<Tuple>::value, void>::type
    for_each_in_tuple(Fn &&, Tuple &, Tuples &&...)
{
}

template <std::size_t I = 0, class Fn, class Tuple, class... Tuples>
constexpr
    typename std::enable_if<I != std::tuple_size<Tuple>::value, void>::type
    for_each_in_tuple(Fn &&fn, Tuple &tup, Tuples &&...tuples)
{
    using ttype = std::tuple_element_t<I, Tuple>;
    fn.template operator()<ttype, I>(
        std::get<I>(tup), std::get<I>(std::forward<Tuples>(tuples))...);
    for_each_in_tuple<I + 1>(fn, tup, std::forward<Tuples>(tuples)...);
}

/**
 * 检查 fun 参数类型是不是指定的这样
 */
/* std::cout << "type:"
 *                  << std::is_same<fun_argument<decltype(_in_fun)>,
 *                                  std::tuple<int, float, double>>::value
 *                  << std::endl;
 */
template <typename Ret, typename... Rest>
std::tuple<Rest...> fun_argument_helper(Ret (*)(Rest...));

template <typename Ret, typename F, typename... Rest>
std::tuple<Rest...> fun_argument_helper(Ret (F::*)(Rest...));

template <typename Ret, typename F, typename... Rest>
std::tuple<Rest...> fun_argument_helper(Ret (F::*)(Rest...) const);

template <typename F>
decltype(fun_argument_helper(&F::operator())) fun_argument_helper(F);

template <typename T>
using fun_argument = decltype(fun_argument_helper(std::declval<T>()));

// check  is_base_of template class

template <template <typename...> class BaseTemplate, typename Derived,
          typename TCheck = void>
struct test_base_template;

template <template <typename...> class BaseTemplate, typename Derived>
using is_base_template_of =
    typename test_base_template<BaseTemplate, Derived>::is_base;

// Derive - is a class. Let inherit from Derive, so it can cast to its protected
// parents
/* template <template <typename...> class BaseTemplate, typename Derived> */
/* struct test_base_template<BaseTemplate, Derived, */
/*                           std::enable_if_t<std::is_class_v<Derived>>> */
/*     : Derived { */
/*     template <typename... T> */
/*     static constexpr std::true_type test(BaseTemplate<T...> *); */
/*     static constexpr std::false_type test(...); */
/*     using is_base = decltype(test((test_base_template *)nullptr)); */
/* }; */

// Derive - is not a class, so it is always false_type
template <template <typename...> class BaseTemplate, typename Derived>
struct test_base_template<BaseTemplate, Derived,
                          std::enable_if_t<!std::is_class_v<Derived>>> {
    using is_base = std::false_type;
};

template <template <typename...> class BaseTemplate, typename Derived>
struct test_base_template<BaseTemplate, Derived,
                          std::enable_if_t<std::is_class_v<Derived>>>
    : Derived {
    template <typename... T>
    static constexpr std::true_type test(BaseTemplate<T...> *);
    static constexpr std::false_type test(...);
    using is_base = decltype(test((Derived *)nullptr));
};

// From Google C++ Standard, modified to use C++11 deleted functions.
// A macro to disallow the copy constructor and operator= functions.
#define DISALLOW_COPY_MOVE_AND_ASSIGN(TypeName) \
    TypeName(const TypeName &) = delete;        \
    TypeName(const TypeName &&) = delete;       \
    void operator=(const TypeName &) = delete

/**
 * example
 *  std::thread tunnel_thread(&Tunnel::take_data, this);
 */
#define THREAD_CLASS_FUN(fun, cls)              \
    ({                                          \
        do {                                    \
            std::thread clazz_thread(fun, cls); \
            clazz_thread.detach();              \
        } while (0);                            \
    })

/**
 * https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html
 */
#define THREAD_FUN(fun, ...)                                  \
    ({                                                        \
        do {                                                  \
            std::thread fun##_thread_arg(fun, ##__VA_ARGS__); \
            fun##_thread_arg.detach();                        \
        } while (0);                                          \
    })

#define THREAD_FUN_JOIN(fun, ...)                              \
    ({                                                         \
        do {                                                   \
            std::thread fun##_thread_join(fun, ##__VA_ARGS__); \
            fun##_thread_join.join();                          \
        } while (0);                                           \
    })

#define TSleep(t)                                       \
    ({                                                  \
        do {                                            \
            int sleep_time = (1 + (rand() % 20)) * 10;  \
            if (t > 0) {                                \
                sleep_time = t;                         \
            }                                           \
            std::this_thread::sleep_for(                \
                std::chrono::milliseconds(sleep_time)); \
        } while (0);                                    \
    })

}  // namespace e2q
#endif /* ----- #ifndef UTIL_INC  ----- */
