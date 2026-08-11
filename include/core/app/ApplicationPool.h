/*
 * Copyright (C) 2025 Lawrence Link
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "IApplication.h"
#include "config.h"

#include <etl/generic_pool.h>
#include <etl/memory.h>
#include <etl/type_traits.h>
#include <etl/utility.h>

class ApplicationPool;

struct ApplicationDeleter {
    using DestroyFunction = void (*)(ApplicationPool&, IApplication*);

    ApplicationPool* pool = nullptr;
    DestroyFunction destroy = nullptr;

    void operator()(IApplication* application) const noexcept;
};

using ApplicationPtr = etl::unique_ptr<IApplication, ApplicationDeleter>;

class ApplicationPool {
public:
    template <typename T, typename... Args>
    ApplicationPtr make(Args&&... args) {
        static_assert(etl::is_base_of<IApplication, T>::value,
                      "Application type must derive from IApplication");
        static_assert(sizeof(T) <= APPLICATION_POOL_SLOT_SIZE,
                      "Application is larger than APPLICATION_POOL_SLOT_SIZE");
        static_assert(alignof(T) <= APPLICATION_POOL_SLOT_ALIGNMENT,
                      "Application alignment exceeds APPLICATION_POOL_SLOT_ALIGNMENT");

        if (pool_.full()) {
            return ApplicationPtr{};
        }

        T* application = pool_.template create<T>(etl::forward<Args>(args)...);
        if (application == nullptr) {
            return ApplicationPtr{};
        }

        return ApplicationPtr(
            application,
            ApplicationDeleter{this, &ApplicationPool::destroyConcrete<T>});
    }

    bool full() const noexcept { return pool_.full(); }
    bool empty() const noexcept { return pool_.empty(); }
    size_t available() const noexcept { return pool_.available(); }
    size_t size() const noexcept { return pool_.size(); }

private:
    template <typename T>
    static void destroyConcrete(ApplicationPool& pool, IApplication* application) {
        pool.pool_.template destroy<T>(static_cast<T*>(application));
    }

    etl::generic_pool<
        APPLICATION_POOL_SLOT_SIZE,
        APPLICATION_POOL_SLOT_ALIGNMENT,
        APPLICATION_POOL_CAPACITY> pool_;
};

inline void ApplicationDeleter::operator()(IApplication* application) const noexcept {
    if ((application != nullptr) && (pool != nullptr) && (destroy != nullptr)) {
        destroy(*pool, application);
    }
}
