#include "PixelUI.h"
#include <etl/atomic.h>

// RAII
class AtomicGuard {
public:
    AtomicGuard(etl::atomic<bool>& flag) : _flag(flag) {
        _flag.store(true, etl::memory_order_relaxed);
    }
    ~AtomicGuard() {
        _flag.store(false, etl::memory_order_relaxed);
    }
private:
    etl::atomic<bool>& _flag;
};
