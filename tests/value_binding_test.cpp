#include "core/ScaledInteger.h"
#include "core/ValueBinding.h"
#include "ui/ListView/ListView.h"

#include <stdint.h>
#include <string.h>

namespace {

bool matches(
    const PixelUIValue::Binding& binding,
    const char* expected) {
    char buffer[32]{};
    return binding.format(buffer, sizeof(buffer)) &&
           strcmp(buffer, expected) == 0;
}

bool formatHex(
    const void* object,
    const char* suffix,
    char* buffer,
    size_t bufferSize) {
    (void)suffix;
    if (object == nullptr || buffer == nullptr || bufferSize < 3U) {
        if (buffer != nullptr && bufferSize != 0U) buffer[0] = '\0';
        return false;
    }
    const uint8_t value = *static_cast<const uint8_t*>(object);
    constexpr char digits[] = "0123456789ABCDEF";
    buffer[0] = digits[value >> 4U];
    buffer[1] = digits[value & 0x0FU];
    buffer[2] = '\0';
    return true;
}

bool failAfterPartialWrite(
    const void*,
    const char*,
    char* buffer,
    size_t bufferSize) {
    if (buffer != nullptr && bufferSize > 1U) {
        buffer[0] = 'x';
        buffer[1] = '\0';
    }
    return false;
}

} // namespace

int main() {
    static_assert(sizeof(ScaledInt32<10>) == sizeof(int32_t));
    static_assert(sizeof(ListItemAccessory) <= sizeof(void*) * 4U);

    int32_t integer = INT32_MIN;
    if (!matches(PixelUIValue::Binding::integer(integer), "-2147483648")) {
        return 1;
    }

    const auto decimal = ScaledInt32<10>::fromRaw(-7);
    if (!matches(
            PixelUIValue::Binding::decimal<1>(decimal, " V"),
            "-0.7 V")) {
        return 2;
    }

    const auto q15 = ScaledInt32<32768>::fromRaw(49152);
    if (!matches(PixelUIValue::Binding::decimal<3>(q15), "1.500")) {
        return 3;
    }

    const auto carry = ScaledInt32<1000>::fromRaw(1999);
    if (!matches(PixelUIValue::Binding::decimal<2>(carry), "2.00")) {
        return 4;
    }

    uint8_t hexValue = 0xAFU;
    if (!matches(
            PixelUIValue::Binding::custom(&hexValue, &formatHex),
            "AF")) {
        return 5;
    }

    char tooSmall[4] = {'x', 'x', 'x', '\0'};
    if (PixelUIValue::formatScaledInteger(
            tooSmall,
            sizeof(tooSmall),
            123,
            10U,
            1U,
            " V") ||
        tooSmall[0] != '\0') {
        return 6;
    }

    char failedCustom[4] = {'x', 'x', 'x', '\0'};
    if (PixelUIValue::Binding::custom(nullptr, &failAfterPartialWrite)
            .format(failedCustom, sizeof(failedCustom)) ||
        failedCustom[0] != '\0') {
        return 7;
    }

    bool toggleValue = true;
    const ListItemAccessory toggle = ListItemAccessory::toggle(toggleValue);
    const ListItemAccessory text = ListItemAccessory::text("ready");
    const ListItemAccessory value = ListItemAccessory::value(
        PixelUIValue::Binding::integer(integer));
    if (toggle.kind() != ListItemAccessory::Kind::Toggle ||
        toggle.toggleValue() != &toggleValue || toggle.textValue() != nullptr ||
        text.kind() != ListItemAccessory::Kind::Text ||
        strcmp(text.textValue(), "ready") != 0 || text.toggleValue() != nullptr ||
        value.kind() != ListItemAccessory::Kind::Value ||
        value.toggleValue() != nullptr) {
        return 8;
    }

    char accessoryBuffer[16]{};
    if (!value.formatValue(accessoryBuffer, sizeof(accessoryBuffer)) ||
        strcmp(accessoryBuffer, "-2147483648") != 0) {
        return 9;
    }

    const ListItemAccessory failedValue = ListItemAccessory::value(
        PixelUIValue::Binding::custom(nullptr, &failAfterPartialWrite));
    accessoryBuffer[0] = 'x';
    accessoryBuffer[1] = '\0';
    if (failedValue.formatValue(accessoryBuffer, sizeof(accessoryBuffer)) ||
        accessoryBuffer[0] != '\0') {
        return 10;
    }

    return 0;
}
