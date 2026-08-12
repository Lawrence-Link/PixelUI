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

#include <etl/placement_new.h>
#include <etl/type_traits.h>
#include <etl/utility.h>

static_assert(APPLICATION_ARENA_SIZE >= APPLICATION_ARENA_MAX_ALIGNMENT,
              "Application arena must be at least as large as its maximum alignment");
static_assert((APPLICATION_ARENA_MAX_ALIGNMENT & (APPLICATION_ARENA_MAX_ALIGNMENT - 1U)) == 0U,
              "APPLICATION_ARENA_MAX_ALIGNMENT must be a power of two");

enum class ApplicationStackResult {
    Ok,
    StackFull,
    ArenaFull,
    ConstructionFailed,
};

class ApplicationFactory {
    friend class ApplicationStack;

public:
    using ConstructFunction = IApplication* (*)(void* storage, PixelUI& ui, void* parameters);
    using DestroyFunction = void (*)(IApplication* application);

    constexpr ApplicationFactory() = default;

    template <typename T>
    static constexpr ApplicationFactory make() {
        validateType<T>();
        return ApplicationFactory{
            sizeof(T),
            alignof(T),
            &constructDefault<T>,
            &destroyConcrete<T>,
        };
    }

    template <typename T>
    static constexpr ApplicationFactory makeCustom(ConstructFunction construct) {
        validateType<T>();
        return ApplicationFactory{sizeof(T), alignof(T), construct, &destroyConcrete<T>};
    }

private:
    constexpr ApplicationFactory(
        size_t objectSize,
        size_t objectAlignment,
        ConstructFunction construct,
        DestroyFunction destroy)
        : objectSize_(objectSize), objectAlignment_(objectAlignment),
          construct_(construct), destroy_(destroy) {}

    template <typename T>
    static constexpr void validateType() {
        static_assert(etl::is_base_of<IApplication, T>::value,
                      "Application type must derive from IApplication");
        static_assert((alignof(T) & (alignof(T) - 1U)) == 0U,
                      "Application alignment must be a power of two");
        static_assert(alignof(T) <= APPLICATION_ARENA_MAX_ALIGNMENT,
                      "Application alignment exceeds APPLICATION_ARENA_MAX_ALIGNMENT");
        static_assert(sizeof(T) <= APPLICATION_ARENA_SIZE,
                      "Application is larger than APPLICATION_ARENA_SIZE");
    }

    template <typename T>
    static IApplication* constructDefault(void* storage, PixelUI& ui, void* parameters) {
        return ::new (storage) T(ui, parameters);
    }

    template <typename T>
    static void destroyConcrete(IApplication* application) {
        static_cast<T*>(application)->~T();
    }

    size_t objectSize_ = 0;
    size_t objectAlignment_ = 0;
    ConstructFunction construct_ = nullptr;
    DestroyFunction destroy_ = nullptr;
};

class ApplicationStack {
public:
    ApplicationStack() = default;
    ~ApplicationStack() { clear(); }

    ApplicationStack(const ApplicationStack&) = delete;
    ApplicationStack& operator=(const ApplicationStack&) = delete;

    template <typename T, typename... Args>
    ApplicationStackResult emplace(T*& application, Args&&... args) {
        static_assert(etl::is_base_of<IApplication, T>::value,
                      "Application type must derive from IApplication");
        static_assert((alignof(T) & (alignof(T) - 1U)) == 0U,
                      "Application alignment must be a power of two");
        static_assert(alignof(T) <= APPLICATION_ARENA_MAX_ALIGNMENT,
                      "Application alignment exceeds APPLICATION_ARENA_MAX_ALIGNMENT");
        static_assert(sizeof(T) <= APPLICATION_ARENA_SIZE,
                      "Application is larger than APPLICATION_ARENA_SIZE");

        Layout layout{};
        const ApplicationStackResult layoutResult = prepareLayout(sizeof(T), alignof(T), layout);
        if (layoutResult != ApplicationStackResult::Ok) {
            application = nullptr;
            return layoutResult;
        }

        T* constructed =
            ::new (arena_ + layout.alignedOffset) T(etl::forward<Args>(args)...);

        commit(constructed, layout, &destroyConcrete<T>);
        application = constructed;
        return ApplicationStackResult::Ok;
    }

    ApplicationStackResult emplace(
        const ApplicationFactory& factory,
        PixelUI& ui,
        void* parameters,
        IApplication*& application);

    void pop();
    void clear();

    IApplication* top() const noexcept;
    IApplication* previous() const noexcept;
    bool empty() const noexcept { return depth_ == 0U; }
    bool full() const noexcept { return depth_ == MAX_VIEW_DEPTH; }
    size_t depth() const noexcept { return depth_; }
    size_t used() const noexcept { return offset_; }
    static constexpr size_t capacity() noexcept { return APPLICATION_ARENA_SIZE; }

private:
    using DestroyFunction = ApplicationFactory::DestroyFunction;

    struct Entry {
        IApplication* application = nullptr;
        size_t marker = 0;
        size_t endOffset = 0;
        DestroyFunction destroy = nullptr;
    };

    struct Layout {
        size_t marker = 0;
        size_t alignedOffset = 0;
        size_t endOffset = 0;
    };

    ApplicationStackResult prepareLayout(size_t objectSize, size_t alignment, Layout& layout) const;
    void commit(IApplication* application, const Layout& layout, DestroyFunction destroy);

    template <typename T>
    static void destroyConcrete(IApplication* application) {
        static_cast<T*>(application)->~T();
    }

    alignas(APPLICATION_ARENA_MAX_ALIGNMENT) uint8_t arena_[APPLICATION_ARENA_SIZE]{};
    Entry entries_[MAX_VIEW_DEPTH]{};
    size_t offset_ = 0;
    size_t depth_ = 0;
};
