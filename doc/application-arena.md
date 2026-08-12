# Fixed Application Arena

PixelUI constructs every live `IApplication` directly inside one compile-time
fixed byte arena owned by `ViewManager`. Application objects do not use heap
`new` or `delete`, and no owning application handle can leave `ViewManager`.

## Configuration and layout

`include/config.h` defines the application memory budget:

```cpp
constexpr size_t APPLICATION_ARENA_SIZE = 4096;
constexpr size_t APPLICATION_ARENA_MAX_ALIGNMENT = 64;
constexpr int MAX_VIEW_DEPTH = 8;
```

`ApplicationStack` contains the aligned byte arena, a current offset, and a
fixed `MAX_VIEW_DEPTH` metadata array. Each metadata entry stores the
`IApplication*`, the marker from before construction, the allocation end, and
the concrete destruction thunk. Object bytes and metadata have separate roles;
object boundaries are never inferred by scanning arena bytes.

Each push aligns the current offset to `alignof(T)`, includes padding in its
capacity check, constructs `T` in place, and commits metadata only after
construction succeeds. A pop invokes the concrete destructor and restores the
saved marker. The bottom of the arena is aligned to
`APPLICATION_ARENA_MAX_ALIGNMENT`; types requesting stricter alignment fail at
compile time.

## Registration and launch

The normal registration form is unchanged:

```cpp
AppItem aboutApp = AppItem::make<AboutApp>("About", aboutIcon);
```

It expects the conventional constructor `T(PixelUI&, void*)`. `parameters` is
non-owning: it must be null or point to the type expected by `T`, and any state
retained by the App must outlive that App.

An App with unusual constructor arguments supplies a non-capturing construction
thunk. The thunk must either placement-construct exactly one `T` in `storage`
and return its `IApplication*`, or return null without constructing anything:

```cpp
AppItem listApp = AppItem::make<ListApp>(
    "List",
    listIcon,
    [](void* storage, PixelUI& ui, void*) -> IApplication* {
        return ::new (storage) ListApp(ui, items, itemCount);
    });
```

Launch registered Apps with `viewManager.launch(item, parameters)`. Direct
construction uses `viewManager.push<T>(args...)`. Both APIs perform capacity
checks, construction, metadata commit, pause, and enter under one transition
guard. They return `Ok`, `StackFull`, `ArenaFull`, `ConstructionFailed`, or
`TransitionInProgress`. Callers may query `getCurrentApp()` as a non-owning
pointer; there is no public owning application pointer.

## Lifecycle invariants

- Only the arena top may be destroyed. Replacing or deleting a middle App is
  unsupported.
- A live App keeps every App below it alive. Push transitions therefore retain
  both old and new Apps.
- Transition callbacks, constructors, and destructors may not navigate
  synchronously. Nested requests are rejected until the current transition
  finishes.
- Before a popped App is destroyed, `ViewManager` clears drawable, exit
  callback, animations, coroutines, focus references, and popups. Popups are
  included because value popups store references and callbacks that may point
  into an App.
- `ViewManager` explicitly destroys remaining Apps from top to bottom. At
  framework shutdown, `PixelUI` destroys `ViewManager` before the other
  managers so App destructors cannot observe already-destroyed UI services.

App constructors and lifecycle hooks must report recoverable failures through
explicit return values or state established before construction.

## 64-bit simulator measurement

The current Apple arm64 Debug build reports:

| Item | Bytes | Alignment |
| --- | ---: | ---: |
| `IApplication` | 56 | 8 |
| `AboutApp` | 64 | 8 |
| `IconView` / launcher | 544 | 8 |
| `ListView` / `ListViewDemo` | 1432 | 8 |
| arena bytes | 4096 | 64 |
| complete `ApplicationStack` | 4416 | 64 |
| complete `ViewManager` | 4544 | 64 |

A launcher plus `AboutApp` uses 608 arena bytes; launcher plus `ListViewDemo`
uses 1976 bytes; launcher, `ListViewDemo`, and `AboutApp` use 2040 bytes.
Re-run `tests/memory_metrics` for every target compiler and ABI.

The arena removes heap use only for application objects. Existing animations,
popups, PixelUI manager instances, and some widget buffers still use heap
allocation and are outside this refactor.
