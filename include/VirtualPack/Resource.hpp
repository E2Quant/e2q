/*
 * =====================================================================================
 *
 *       Filename:  Resource.hpp
 *
 *    Description:  Resource
 *
 *        Version:  1.0
 *        Created:  2023年09月19日 09时06分34秒
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

#ifndef RESOURCE_INC
#define RESOURCE_INC
#include <cstddef>
#include <memory>
#include <vector>

#include "VirtualPack/Silkworm.hpp"
#include "VirtualPack/foreign.hpp"
namespace e2q {

namespace MessageShare {

/*
 * =====================================================================================
 *        Class:  StashPack
 *  Description:  SilkPermit 类型,专为数据共享，一写多读使用的类型,
 *              使用的时候，可以按需求继承
 *, typename = typename std::enable_if<
                          is_base_template_of<SilkPermit, T>::value>::type
 * =====================================================================================
 */

template <typename T>
struct StashPack {
    StashPack& operator=(const StashPack& _other);
    size_t id;        // beam assign return id
    size_t type = 0;  //  messagepack set add here, 通过查找，可以找出这个ID
                      //  对应的内存，进行不同的操作
    std::shared_ptr<T> data_ptr;

}; /* ----------  end of struct StashPack  ---------- */

}  // namespace MessageShare

/*
 * ================================
 *        Class:  Resource
 *  Description:  提供全局内存管理
 * ================================
 */
class Resource {
public:
    /* =============  LIFECYCLE     =================== */
    Resource()
    {
        _source_ptr = std::make_shared<Silkworm<SeqType>>(_row, _column);
    }; /* constructor */
    ~Resource() {}
    /* =============  ACCESSORS     =================== */

    /**
     * 分配一块内存
     */
    template <typename T>
    std::shared_ptr<T> MemPtr(const size_t column)
    {
        if (_source_ptr == nullptr) {
            elog::bug(" _source_ptr not init");
        }
        return _source_ptr->make_share<T>(column);
    } /* -----  end of function ResourcePtr  ----- */

    template <typename T>
    T ResourceObj(const size_t column)
    {
        if (_source_ptr == nullptr) {
            elog::bug(" _source_ptr not init");
        }
        return _source_ptr->apply<T>(column);
    } /* -----  end of function ResourcePtr  ----- */

    /* =============  MUTATORS      =================== */
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  ResourcePtr
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *  内存资源操作
     * ============================================
     */
    template <typename T, typename MT,
              typename = typename std::enable_if<
                  is_base_template_of<MessageShare::StashPack, T>::value>::type>
    std::shared_ptr<T> ResourcePtr(size_t column)
    {
        if (_source_ptr == nullptr) {
            elog::bug(" _source_ptr not init");
            return nullptr;
        }

        std::shared_ptr<T> _msg_share_ptr = std::make_shared<T>();
        _msg_share_ptr->id = 0;
        _msg_share_ptr->data_ptr = _source_ptr->make_share<MT>(column);

        _message_list.push_back(_msg_share_ptr);
        return _msg_share_ptr;
    }

    template <typename T, typename MT,
              typename = typename std::enable_if<
                  is_base_template_of<MessageShare::StashPack, T>::value>::type>
    std::shared_ptr<T> StashPtr()
    {
        if (_source_ptr == nullptr) {
            elog::bug(" _source_ptr not init");
            return nullptr;
        }
        std::shared_ptr<T> _msg_share_ptr = std::make_shared<T>();
        _msg_share_ptr->id = 0;
        _msg_share_ptr->data_ptr = std::make_shared<MT>();

        _message_list.push_back(_msg_share_ptr);
        return _msg_share_ptr;
    }
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  fetch
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *   找出某 type value 的内存
     * ============================================
     */
    template <typename T>
    std::shared_ptr<T> fetch(size_t type)
    {
        std::vector<std::shared_ptr<T>> mt =
            _message_list.get_vector<std::shared_ptr<T>>();
        for (auto it : mt) {
            if (it->type == type) {
                //  elog::info("found");
                return it;
            }
        }
        elog::bug("mt size:", mt.size(), " type:", type);
        for (auto it : mt) {
            elog::info("not found:", it->type);
        }
        return nullptr;
    } /* -----  end of function fetch  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  rows
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    size_t rows() { return _row; } /* -----  end of function rows  ----- */
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    size_t _row = SilkWormRow;
    size_t _column = SilkWormColumn;

    /**
     * 内存资源操作
     */
    std::shared_ptr<Silkworm<SeqType>> _source_ptr = nullptr;

    Bucket _message_list;
}; /* -----  end of class Resource  ----- */

using _Resource_ptr = std::shared_ptr<Resource>;
}  // namespace e2q
#endif /* ----- #ifndef RESOURCE_INC  ----- */
