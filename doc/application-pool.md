# Fixed Application Pool

PixelUI constructs `IApplication` objects in storage owned by `ViewManager`.
The storage size is fixed at compile time and does not use `new` or `delete` for
application objects.

## Configuration

The three constants in `include/config.h` define the memory budget:

```cpp
constexpr size_t APPLICATION_POOL_SLOT_SIZE = 1536;
constexpr size_t APPLICATION_POOL_SLOT_ALIGNMENT = 16;
constexpr size_t APPLICATION_POOL_CAPACITY = MAX_VIEW_DEPTH;
```

The application pool reserves approximately
`APPLICATION_POOL_SLOT_SIZE * APPLICATION_POOL_CAPACITY` bytes, plus alignment
and small pool metadata. Every live application consumes one complete slot.
The default 1536-byte slot covers the current largest example, `ListViewDemo`,
which is 1432 bytes in the 64-bit simulator build. Object sizes must be measured
again for the target compiler and ABI.

This is a determinism and fragmentation tradeoff, not an average-RAM
optimization. The 64-bit simulator reports `sizeof(ApplicationPool) == 12320`
and `sizeof(ViewManager) == 12576`; all slots are reserved even if only one app
is live. Lower `APPLICATION_POOL_CAPACITY` when the required nesting depth is
smaller. To reduce the slot size itself, first reduce the large inline state in
`ListView`; otherwise every slot must accommodate that largest type. Separate
small/large size-class pools are a possible follow-up when application sizes
remain widely different.

Each registered type is checked at compile time. A type that is too large, has
too strict an alignment, or does not derive from `IApplication` produces a
`static_assert` at its registration/factory call site.

## Registration

Applications with the conventional `(PixelUI&, void*)` constructor use the
short form:

```cpp
AppItem aboutApp = AppItem::make<AboutApp>("About", aboutIcon);
```

An application with different constructor arguments uses a non-capturing
factory function or lambda:

```cpp
AppItem listApp{
    .title = "List",
    .bitmap = listIcon,
    .createApp = [](ApplicationPool& pool, PixelUI& ui, void*) -> ApplicationPtr {
        return pool.make<ListApp>(ui, items, itemCount);
    },
};
```

Factories are plain function pointers. Capturing lambdas cannot be registered;
runtime state should be passed through the `void* parameters` argument and must
remain valid for the application's required lifetime.

Launch through `ViewManager` so both stack and pool limits are checked before
construction:

```cpp
ViewManager::LaunchResult result = viewManager.launch(aboutApp, parameters);
```

`LaunchResult` reports `Ok`, `StackFull`, `PoolFull`, or
`ConstructionFailed`. `ViewManager::push()` accepts only `ApplicationPtr`, so a
pooled object cannot accidentally be converted to a default-deleter pointer.
It also rejects a handle created by a different pool, because that pool may not
outlive the view stack.

## Ownership and limits

`ApplicationPtr` is the only owning application handle. Its deleter stores the
pool address and a small type-specific destroy function, allowing the runtime
stack to remain non-template and hold mixed application types. Do not call
`release()` or keep a handle beyond the lifetime of its `ViewManager`.

Pool allocation, launch, push, and pop are UI-thread operations. The pool is not
thread-safe. `ViewManager::pop()` clears drawable, focus, coroutine, and
animation references before returning the application slot to the pool.

This removes heap allocation only for concrete `IApplication` objects. PixelUI
subsystems, popups, animations, and widget-owned buffers may still allocate.
In the current implementation, `PixelUI` itself creates `ViewManager` with one
heap allocation, so the inline pool is part of that fixed-size manager block.
Moving all manager ownership inline is a separate whole-framework heap-removal
change.

The templates are restricted to construction and the concrete destroy thunk.
Each application type adds one factory specialization and one small destroy
function; rendering, event dispatch, and the view stack remain non-template.
This is not expected to cause significant binary growth, but the final ELF/map
should be compared with identical optimization and linker garbage-collection
settings for the target toolchain.
