# Popup storage and lifetime

`PopupManager` is a fixed-capacity FIFO with one active Popup. It contains:

- one `etl::variant_pool<1, PopupInfo, PopupProgress, PopupValueDigits>` slot;
- up to `MAX_POPUP_NUM - 1` compact pending request descriptors;
- one non-owning `IPopup*` pointing at the active pool object.

The first accepted request is constructed immediately. Later requests stay in
FIFO order and do not become complete Popup objects until every earlier Popup
has closed. Only the active Popup is drawn, updated, or given input. When it
finishes closing, its concrete destructor runs, the stable pool slot is
released, and the next request is constructed in that slot.

`MAX_POPUP_NUM` counts the active Popup plus pending requests. A full queue
rejects a new request and leaves existing work unchanged. Public
`showPopup...()` functions return `bool` so callers can observe rejection.
There is no Popup priority or eviction policy.

Popup objects and request descriptors do not use heap allocation. The active
slot is sized and aligned for the largest registered concrete Popup. Adding a
larger Popup type increases this one slot, rather than every queue entry.

## Non-owning request data

Requests retain non-owning text/title pointers, referenced values, font
pointers, and callback captures. These targets must remain valid while the
request is pending and throughout its active lifetime. Queueing extends that
required lifetime compared with immediate construction.

`ViewManager` clears the active Popup and all pending requests before an App is
destroyed. Code outside that lifecycle boundary must apply the same rule when
request data belongs to a shorter-lived owner.

## Reentrancy

Structural changes are rejected while `PopupManager` is constructing,
destroying, updating, drawing, or dispatching input to a Popup. A Popup callback
cannot synchronously enqueue or clear Popups. Such work must be requested after
the current dispatch returns.

## Animation cleanup

`PopupValueDigits` can start animations that reference internal `NumScroll`
members. Its destructor currently clears all animations before its pool slot is
released. This prevents stale member references, but may also stop unrelated
application animations. Owner-scoped animation cancellation would remove that
side effect at the cost of additional animation metadata.

Use `showPopupValueDigits()` and pass a digit count from `1` through
`MAX_INT_FIXED_WIDTH`.
