# Fixed animation storage

`AnimationManager` stores `CallbackAnimation` objects directly in a fixed
`etl::vector<CallbackAnimation, MAX_ANIMATION_COUNT>`. Adding an animation uses
`emplace_back`; individual animations do not use `new`, `delete`, or owning
pointers.

The manager accepts the protection state and callback as construction inputs.
It does not return an element pointer because vector compaction can move
animations when completed or unprotected entries are removed. Callers use
`PixelUI::animate()` for reference targets or `PixelUI::animateCallback()` for
custom update callbacks.

`AnimationManager` is an inline private member of `PixelUI`. Callers cannot get
the manager or animation element addresses. Creation, cleanup, protection reset,
and active-count queries go through the narrow `PixelUI` animation API so timing,
capacity, and application-lifetime policies remain centralized.

This design relies on the current closed animation set: every managed animation
is a `CallbackAnimation`. Adding another concrete animation kind requires
changing the inline element representation, for example to an ETL variant; it
must not be stored as `Animation` by value because that would slice the derived
object.

On the current macOS simulator ABI, `sizeof(CallbackAnimation)` is 72 bytes and
`sizeof(AnimationManager)` is 2544 bytes. The manager therefore reserves all 35
slots as fixed RAM, but requires no per-animation heap allocation or allocator
metadata. These values must be remeasured for each embedded compiler and ABI.
