#include "core/app/ApplicationStack.h"

namespace {

class ThrowingApplication : public IApplication {
public:
    ThrowingApplication() { throw 7; }
    void draw() override {}
    bool handleInput(InputEvent) override { return false; }
};

} // namespace

int main() {
    ApplicationStack stack;
    ThrowingApplication* application = reinterpret_cast<ThrowingApplication*>(1U);
    const ApplicationStackResult result = stack.emplace(application);
    return ((result == ApplicationStackResult::ConstructionFailed) &&
            (application == nullptr) && stack.empty() && (stack.used() == 0U)) ? 0 : 1;
}
