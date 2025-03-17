/*
 * =====================================================================================
 *
 *       Filename:  Bucket.hpp
 *
 *    Description:  Bucket
 *
 *        Version:  1.0
 *        Created:  2022年10月27日 17时09分34秒
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

#ifndef BUCKET_INC
#define BUCKET_INC
#include <cstddef>
#include <experimental/type_traits>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Toolkit/pack.hpp"
namespace e2q {

template <class...>
struct type_list {
};

struct visitor_tample {
}; /* ----------  end of struct visitor_tample  ---------- */

template <class... TYPES>
struct visitor_base : visitor_tample {
    using types = type_list<TYPES...>;
};

/*
 * =====================================================================================
 *        Class:  Bucket
 *  Description:
 * https://gieseanw.wordpress.com/2017/05/03/a-true-heterogeneous-container-in-c/
 * =====================================================================================
 */
class Bucket {
public:
    /* ====================  LIFECYCLE =======================================
     */
    Bucket() = default; /* constructor */
    Bucket(const Bucket& _other) { *this = _other; }
    ~Bucket() { clear(); }
    /* ====================  ACCESSORS =======================================
     */

    /**
     *
     */
    Bucket& operator=(const Bucket& _other)
    {
        if (this != &_other) {
            clear();
            clear_functions = _other.clear_functions;
            copy_functions = _other.copy_functions;
            for (auto&& copy_function : copy_functions) {
                copy_function(_other, *this);
            }
            size_functions = _other.size_functions;
        }
        return *this;
    }
    /* ====================  MUTATORS =======================================
     */

    /* ====================  OPERATORS =======================================
     */

    /**
     * 取某一个类型的 vector ,如果不存在，就创建一个
     *
     */
    template <class T>
    std::vector<T>& get_vector()
    {
        auto iter = items<T>.find(this);
        if (iter == items<T>.end()) {
            clear_functions.emplace_back(
                [](Bucket& _c) { items<T>.erase(&_c); });

            // if someone copies me, they need to call each copy_function and
            // pass themself
            copy_functions.emplace_back([](const Bucket& _from, Bucket& _to) {
                items<T>[&_to] = items<T>[&_from];
            });
            /* move_functions.emplace_back([](Bucket& from, Bucket& to) { */
            /*     items<T>[&to] = std::move(items<T>[&from]); */
            /* }); */
            size_functions.emplace_back(
                [](const Bucket& _c) { return items<T>[&_c].size(); });
            iter = items<T>.emplace(this, std::vector<T>()).first;
        }
        return iter->second;
    };

    template <class T>
    const std::vector<T>& get_vector() const
    {
        return (const_cast<Bucket*>(this)->get_vector<T>());
    };

    /**
     *  存放某一个类型的值，如果找不到就创建一个
     */
    template <class T>
    size_t push_back(const T& _t)
    {
        // don't have it yet, so create functions for destroying
        if (items<T>.find(this) == std::end(items<T>)) {
            clear_functions.emplace_back(
                [](Bucket& _c) { items<T>.erase(&_c); });

            // if someone copies me, they need to call each copy_function and
            // pass themself
            copy_functions.emplace_back([](const Bucket& _from, Bucket& _to) {
                items<T>[&_to] = items<T>[&_from];
            });

            size_functions.emplace_back(
                [](const Bucket& _c) { return items<T>[&_c].size(); });
        }
        items<T>[this].push_back(_t);
        size_t id = items<T>[this].size() - 1;
        return id;
    };

    /**
     * 某一个类型在保存中的大小
     */
    template <class T>
    size_t number_of() const
    {
        auto iter = items<T>.find(this);
        if (iter != items<T>.cend()) return items<T>[this].size();
        return 0;
    }

    /**
     * 清除第一个类型值
     */
    template <class T>
    void clear_head()
    {
        auto iter = items<T>.find(this);
        if (iter != items<T>.cend()) {
            items<T>[this].erase(items<T>[this].cbegin());
        }
    }

    /*
     *  历遍 内部类型的所有数据，主要是print 使用
     */
    template <class T>
    void visit(T&& visitor)
    {
        visit_impl(visitor, typename std::decay_t<T>::types{});
    }
    /**
     * Return an int representing the number of elements in this object.
     */
    size_t size() const;
    size_t length() const;

protected:
    /* ====================  METHODS =======================================
     */
    void clear();
    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

    /**
     * 保存不同类型的 map
     */
    template <class T>
    static std::unordered_map<const Bucket*, std::vector<T>> items;

    /**
     * 类型所包括的属性
     */
    std::vector<std::function<void(Bucket&)>> clear_functions;
    std::vector<std::function<void(const Bucket&, Bucket&)>> copy_functions;
    std::vector<std::function<void(Bucket&, Bucket&)>> move_functions;
    std::vector<std::function<size_t(const Bucket&)>> size_functions;

    ////  以下模板主要是 print, 或者对某一类型进行修改修值 的使用
    /*
     *  自动推导返回值，生成一个右值引用的函数
     */
    template <class T, class U>
    using visit_function =
        decltype(std::declval<T>().operator[](std::declval<U&>()));

    /**
     * 检查函数 是否存在 参数 或返回值之类的
     */
    template <class T, class U>
    static constexpr bool has_visit_v =
        std::experimental::is_detected<visit_function, T, U>::value;

    /**
     * 历遍函数
     */
    template <class T, template <class...> class TLIST, class... TYPES>
    void visit_impl(T&& visitor, TLIST<TYPES...>)
    {
        (..., visit_impl_help<std::decay_t<T>, TYPES>(visitor));
    }

    /**
     * 检查 T 函数参数是不是 U
     */
    template <class T, class U>
    void visit_impl_help(T& visitor)
    {
        static_assert(has_visit_v<T, U>,
                      "Visitors must provide a visit function accepting a "
                      "reference for each type");
        bool hasVisit = true;
        for (auto&& element : items<U>[this]) {
            visitor[element];
            hasVisit = false;
        }
        if (hasVisit) {
            log::bug("has nothing visit!!");
        }
    }

}; /* -----  end of class Bucket  ----- */
template <class T>
std::unordered_map<const Bucket*, std::vector<T>> Bucket::items;

using _ptr_bucket_block = std::shared_ptr<Bucket>;

}  // namespace e2q
#endif /* ----- #ifndef BUCKET_INC  ----- */
