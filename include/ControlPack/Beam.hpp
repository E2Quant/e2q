/*
 * =====================================================================================
 *
 *       Filename:  Beam.hpp
 *
 *    Description:  Beam
 *
 *        Version:  1.0
 *        Created:  2023年07月27日 15时23分37秒
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

#ifndef BEAM_INC
#define BEAM_INC

#include <cmath>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ControlPack/Bucket.hpp"
#include "ControlPack/Shuttle.hpp"
#include "ControlPack/ShuttleHeader.hpp"
#include "ControlPack/foreign.hpp"
namespace e2q {
namespace bridge {

template <typename T>
struct TypeTask {
};

/**
 * Beam receive function type
 */

template <typename StashRetArg>
using ReceiveType = Slot<void, StashRetArg>;

/**
 * Beam stash callback type
 */
template <typename RetType, typename... Funtype>
using StashType = Slot<RetType, Funtype...>;

/**
 *  bucket slot visitor
 */
template <typename RetType, typename... FunArg>
struct Stash : SlotVisitor<void, RetType> {
    Stash(StashType<RetType, FunArg...> ptr, FunArg... arg) : _fun_ptr(ptr)
    {
        _tups = std::make_tuple(arg...);
    };

    template <class T>
    void operator[](T& _in)
    {
        RetType _ret = std::apply(_fun_ptr, _tups);
        (_in)(std::move(_ret));
    }

private:
    StashType<RetType, FunArg...> _fun_ptr;
    std::tuple<FunArg...> _tups;
}; /* ----------  end of struct Stash  ---------- */

}  // namespace bridge

class BeamSignal {
public:
    BeamSignal(){};
    BeamSignal(std::shared_ptr<Shuttle> other) : _shuttle_obj(other) {}
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  receive
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *  给接受方使用的
     *   MessageShare::StashPack  => MessageSharePtr
     *      bridge::ReceiveType<MessageSharePtr> rfun =
     *      [](MessageSharePtr p){
     *          do(...);
     *      }
     * ============================================
     */
    template <typename RetType>
    void receive(bridge::ReceiveType<RetType>& recfun)
    {
        if (_shuttle_obj != nullptr) {
            _shuttle_obj->connect(recfun);
        }
    }

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  stash
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *  给发送方使用的
     * ============================================
     */
    template <typename RetType, typename... FunArg>
    void stash(bridge::StashType<RetType, FunArg...> callback, FunArg... args)
    {
        if (_shuttle_obj != nullptr) {
            _shuttle_obj->run<bridge::Stash<RetType, FunArg...>, void, RetType>(
                bridge::Stash<RetType, FunArg...>(
                    callback, std::forward<FunArg>(args)...));
        }
    } /* -----  end of function stash  ----- */
private:
    // singal for stash and receive message disruptor
    std::shared_ptr<Shuttle> _shuttle_obj = nullptr;
}; /* -----  end of class BeamSignal  ----- */

/**
 * base for beam assign and signal
 * id is Signal type
 */
#define BuildSignal(arg)                                                     \
    struct arg##Signal : public BeamSignal {                                 \
        arg##Signal(){};                                                     \
        arg##Signal(std::shared_ptr<Shuttle> other) : BeamSignal(other) {}   \
        SigId id;                                                            \
    };                                                                       \
    template <typename T, typename = typename std::enable_if<                \
                              std::is_base_of<arg##Signal, T>::value>::type> \
    struct arg##BeamClass {                                                  \
        void callback(std::shared_ptr<T> beam);                              \
        void shareptr(_Resource_ptr ptr);                                    \
                                                                             \
    protected:                                                               \
        _Resource_ptr _source_ptr{nullptr};                                  \
    };                                                                       \
    using arg##_beam = func_type<std::shared_ptr<arg##Signal>>;

/**
 * Connect class
 */
BuildSignal(Connect);

/**
 * Function class
 */
BuildSignal(Func);

/**
 *  call Signal
 *  beam_data
 *  shu_ptr
 *

 */
#define SignalThread(self, sigid)                                     \
    do {                                                              \
        auto _fun_Signal = [self](SigId id) {                         \
            BasePtr<FuncSignal> fun_sig =                             \
                std::make_shared<FuncSignal>(self->_shu_ptr);         \
            fun_sig->id = id;                                         \
            for (auto fn : self->_beam_data->fetch<Func_beam>()) {    \
                fn(fun_sig);                                          \
            }                                                         \
            BasePtr<ConnectSignal> st_sig =                           \
                std::make_shared<ConnectSignal>(self->_shu_ptr);      \
            st_sig->id = id;                                          \
            for (auto cn : self->_beam_data->fetch<Connect_beam>()) { \
                cn(st_sig);                                           \
            }                                                         \
        };                                                            \
        std::thread _fun_Signal_thread_arg(_fun_Signal, sigid);       \
        _fun_Signal_thread_arg.detach();                              \
    } while (0)

/**
 * build bind callback
 */
#define BeamBind(ptype, obj)                                                \
    ({                                                                      \
        ptype _ret = nullptr;                                               \
        do {                                                                \
            using clazz = decltype(obj);                                    \
            _ret = std::bind(&clazz::callback, obj, std::placeholders::_1); \
        } while (0);                                                        \
        _ret;                                                               \
    })

/*
 * =====================================================================================
 *        Class:  Beam
 *  Description:
 *    保存不同的数据进入接口
 *    分发不同的数据给不同的接口读取
 * =====================================================================================
 */

class BeamData {
public:
    BeamData() { _bucket_ptr = std::make_shared<Bucket>(); }
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  assign
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *  底层指针类型桶
     * ============================================
     */

    template <class SubType, typename BBT>
    size_t assign(SubType sub)
    {
        BBT cfun = BeamBind(BBT, sub);

        size_t id = _bucket_ptr->push_back(cfun);

        return id;
    }

    /* -----  end of function assign  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  fetch
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *  底层指针类型桶
     * ============================================
     */
    template <typename T>
    std::vector<T>& fetch()
    {
        return _bucket_ptr->get_vector<T>();
    } /* -----  end of function fetch  ----- */

private:
    std::shared_ptr<Bucket> _bucket_ptr = nullptr;
}; /* ----------  end of class BeamData  ---------- */

}  // namespace e2q
#endif /* ----- #ifndef BEAM_INC  ----- */
