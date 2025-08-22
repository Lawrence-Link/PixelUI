#include "../widgets.h"

class Texts : public Widget {
public:
    Texts(const std::string& text) : text_(text) {};
private:
    const std::string& text_;
};