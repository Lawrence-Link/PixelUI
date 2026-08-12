#include "core/app/ApplicationStack.h"

#include <stdint.h>

namespace {

size_t alignUp(size_t value, size_t alignment) {
    return (value + alignment - 1U) & ~(alignment - 1U);
}

struct DestructionLog {
    int values[32]{};
    size_t size = 0;

    void add(int value) { values[size++] = value; }
};

class SmallApplication : public IApplication {
public:
    SmallApplication(DestructionLog& log, int id) : log_(log), id_(id) {}
    ~SmallApplication() override { log_.add(id_); }
    void draw() override {}
    bool handleInput(InputEvent) override { return false; }

private:
    DestructionLog& log_;
    int id_;
    uint8_t payload_[3]{};
};

class alignas(32) AlignedApplication : public IApplication {
public:
    AlignedApplication(DestructionLog& log, int id) : log_(log), id_(id) {}
    ~AlignedApplication() override { log_.add(id_); }
    void draw() override {}
    bool handleInput(InputEvent) override { return false; }

private:
    DestructionLog& log_;
    int id_;
    uint8_t payload_[17]{};
};

class LargeApplication : public IApplication {
public:
    explicit LargeApplication(DestructionLog& log) : log_(log) {}
    ~LargeApplication() override { log_.add(99); }
    void draw() override {}
    bool handleInput(InputEvent) override { return false; }

private:
    DestructionLog& log_;
    uint8_t payload_[APPLICATION_ARENA_SIZE - 128U]{};
};

} // namespace

int main() {
    DestructionLog log;

    {
        ApplicationStack stack;
        SmallApplication* first = nullptr;
        AlignedApplication* second = nullptr;
        SmallApplication* third = nullptr;

        if (stack.emplace(first, log, 1) != ApplicationStackResult::Ok) return 1;
        const size_t afterFirst = sizeof(SmallApplication);
        if ((stack.used() != afterFirst) || (stack.depth() != 1U)) return 2;

        if (stack.emplace(second, log, 2) != ApplicationStackResult::Ok) return 3;
        const size_t secondOffset = alignUp(afterFirst, alignof(AlignedApplication));
        const size_t afterSecond = secondOffset + sizeof(AlignedApplication);
        if ((reinterpret_cast<uintptr_t>(second) % alignof(AlignedApplication)) != 0U) return 4;
        if (stack.used() != afterSecond) return 5;

        if (stack.emplace(third, log, 3) != ApplicationStackResult::Ok) return 6;
        if ((third == first) || (stack.depth() != 3U) ||
            (stack.used() != afterSecond + sizeof(SmallApplication))) return 7;

        const void* thirdAddress = third;
        stack.pop();
        if ((log.size != 1U) || (log.values[0] != 3) || (stack.used() != afterSecond)) return 8;

        SmallApplication* reused = nullptr;
        if (stack.emplace(reused, log, 4) != ApplicationStackResult::Ok) return 9;
        if (reused != thirdAddress) return 10;

        stack.clear();
        if ((log.size != 4U) || (log.values[1] != 4) ||
            (log.values[2] != 2) || (log.values[3] != 1)) return 11;
        if (!stack.empty() || (stack.used() != 0U)) return 12;
        stack.pop();
    }

    {
        ApplicationStack stack;
        SmallApplication* apps[MAX_VIEW_DEPTH]{};
        for (size_t i = 0; i < MAX_VIEW_DEPTH; ++i) {
            if (stack.emplace(apps[i], log, static_cast<int>(10 + i)) != ApplicationStackResult::Ok) return 13;
        }
        const size_t usedBeforeFailure = stack.used();
        SmallApplication* rejected = reinterpret_cast<SmallApplication*>(1U);
        if (stack.emplace(rejected, log, 42) != ApplicationStackResult::StackFull) return 14;
        if ((rejected != nullptr) || (stack.used() != usedBeforeFailure) ||
            (stack.depth() != MAX_VIEW_DEPTH)) return 15;
    }

    {
        ApplicationStack stack;
        LargeApplication* large = nullptr;
        if (stack.emplace(large, log) != ApplicationStackResult::Ok) return 16;
        const size_t usedBeforeFailure = stack.used();
        SmallApplication* rejected = reinterpret_cast<SmallApplication*>(1U);
        if (stack.emplace(rejected, log, 50) != ApplicationStackResult::ArenaFull) return 17;
        if ((rejected != nullptr) || (stack.used() != usedBeforeFailure) || (stack.depth() != 1U)) return 18;
    }

    return 0;
}
