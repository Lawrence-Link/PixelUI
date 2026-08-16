#pragma once

#include <stdint.h>

enum class ValueEditPolicy : uint8_t {
    Live,
    CommitOnConfirm,
};

class ValueEditorBinding {
public:
    using ReadFunction = bool (*)(const void* context, int32_t& value);
    using WriteFunction = bool (*)(void* context, int32_t value);
    using ChangedFunction = void (*)(void* context, int32_t value);

    constexpr ValueEditorBinding() = default;

    // valueContext and changedContext are non-owning. They must outlive every
    // read, write and notify performed through this binding.
    static constexpr ValueEditorBinding custom(
        void* valueContext,
        ReadFunction read,
        WriteFunction write,
        ChangedFunction changed = nullptr,
        void* changedContext = nullptr) {
        return ValueEditorBinding(
            valueContext, read, write, changed, changedContext);
    }

    static constexpr ValueEditorBinding reference(
        int32_t& value,
        ChangedFunction changed = nullptr,
        void* changedContext = nullptr) {
        return ValueEditorBinding(
            &value, &readReference, &writeReference, changed, changedContext);
    }

    bool read(int32_t& value) const;
    bool write(int32_t value) const;
    void notify(int32_t value) const;
    constexpr bool valid() const {
        return read_ != nullptr && write_ != nullptr;
    }

private:
    constexpr ValueEditorBinding(
        void* valueContext,
        ReadFunction read,
        WriteFunction write,
        ChangedFunction changed,
        void* changedContext)
        : valueContext_(valueContext),
          changedContext_(changedContext),
          read_(read),
          write_(write),
          changed_(changed) {}

    static bool readReference(const void* context, int32_t& value);
    static bool writeReference(void* context, int32_t value);

    void* valueContext_ = nullptr;
    void* changedContext_ = nullptr;
    ReadFunction read_ = nullptr;
    WriteFunction write_ = nullptr;
    ChangedFunction changed_ = nullptr;
};

class ValueEditSession {
public:
    explicit ValueEditSession(
        int32_t initialValue = 0,
        ValueEditPolicy policy = ValueEditPolicy::CommitOnConfirm);
    // A copied binding remains non-owning; its contexts must outlive the session.
    ValueEditSession(
        ValueEditorBinding binding,
        ValueEditPolicy policy = ValueEditPolicy::CommitOnConfirm);

    bool setDraftValue(int32_t value);
    bool commit();
    bool cancel();

    int32_t originalValue() const { return originalValue_; }
    int32_t draftValue() const { return draftValue_; }
    ValueEditPolicy policy() const { return policy_; }
    bool valid() const { return valid_; }
    bool hasExternalBinding() const { return binding_.valid(); }

private:
    ValueEditorBinding binding_{};
    int32_t originalValue_ = 0;
    int32_t draftValue_ = 0;
    ValueEditPolicy policy_ = ValueEditPolicy::CommitOnConfirm;
    bool valid_ = true;
};
