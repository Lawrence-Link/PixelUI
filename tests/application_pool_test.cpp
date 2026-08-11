#include "core/app/ApplicationPool.h"

class TestApplication : public IApplication {
public:
    explicit TestApplication(int& destructionCount)
        : destructionCount_(destructionCount) {
    }

    ~TestApplication() override {
        ++destructionCount_;
    }

    void draw() override {
    }

    bool handleInput(InputEvent) override {
        return false;
    }

private:
    int& destructionCount_;
};

class OtherApplication : public TestApplication {
public:
    explicit OtherApplication(int& destructionCount)
        : TestApplication(destructionCount) {
    }

private:
    uint8_t payload_[128]{};
};

int main() {
    ApplicationPool pool;
    int destructionCount = 0;

    if (!pool.empty() || (pool.available() != APPLICATION_POOL_CAPACITY)) {
        return 1;
    }

    {
        ApplicationPtr applications[APPLICATION_POOL_CAPACITY];
        for (size_t i = 0; i < APPLICATION_POOL_CAPACITY; ++i) {
            applications[i] = pool.make<TestApplication>(destructionCount);
            if (!applications[i]) {
                return 2;
            }
        }

        if (!pool.full() || pool.make<TestApplication>(destructionCount)) {
            return 3;
        }

        applications[2].reset();
        if ((destructionCount != 1) || (pool.available() != 1)) {
            return 4;
        }

        applications[2] = pool.make<OtherApplication>(destructionCount);
        if (!applications[2] || !pool.full()) {
            return 5;
        }
    }

    if (!pool.empty() || (destructionCount != APPLICATION_POOL_CAPACITY + 1)) {
        return 6;
    }

    {
        ApplicationPtr first = pool.make<TestApplication>(destructionCount);
        IApplication* address = first.get();
        ApplicationPtr second(etl::move(first));
        if (first || (second.get() != address)) {
            return 7;
        }
    }

    return (pool.empty() &&
            (destructionCount == APPLICATION_POOL_CAPACITY + 2)) ? 0 : 8;
}
