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

Requests retain non-owning text/title/font pointers, `ValueEditorBinding`
contexts, `NumericFormatter` contexts, and any references inside callback
captures. These targets must remain valid while the request is pending and
throughout its active lifetime. An injected `ValueEditSession` must likewise
outlive a directly constructed Popup. Queueing extends that required lifetime
compared with immediate construction. Calling `clearPopups()` ends the pending
and active use before a shorter-lived owner is destroyed.

`ViewManager` clears the active Popup and all pending requests before an App is
destroyed. Code outside that lifecycle boundary must apply the same rule when
request data belongs to a shorter-lived owner.

## Reentrancy

Structural changes are rejected while `PopupManager` is constructing,
destroying, updating, drawing, or dispatching input to a Popup. A Popup callback
cannot synchronously enqueue or clear Popups. Such work must be requested after
the current dispatch returns.

## Animation cleanup

`PopupValueDigits`, its `NumScroll` children, `ListView`, `IconView`, and
`FocusManager` retain only their own animation handles. Destruction and state
replacement cancel those handles without clearing animations owned by other
components. If animation capacity is exhausted, each component synchronizes
its affected geometry to the stable final state.

Use `showPopupValueDigits()` and pass a digit count from `1` through
`MAX_INT_FIXED_WIDTH`. SELECT on an active digit only finishes editing that
digit, so focus can move to another digit. The popup commits only through its
OK button. CANCEL, BACK, timeout, and destruction cancel an unfinished session;
for `ValueEditPolicy::Live`, cancellation restores the original value.
