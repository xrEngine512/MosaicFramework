#pragma once
//
// Created by islam on 27.05.17.
//

#include "function_caller.h"
#include "module_metadata.h"
#include "type_subsystem/mappings.h"

#include <exceptions.h>

namespace interop {
class local_function_caller: public function_caller {
    static void * vm;
    static uint32_t count;

    const function_metadata_t * metadata;
    uint8_t argument_index;

  public:
    void push_arg(double arg) override;

    void push_arg(int arg) override;

    void push_arg(bool arg) override;

    void push_arg(char arg) override;

    void push_arg(float arg) override;

    void push_arg(long arg) override;

    void push_arg(long long arg) override;

    void push_arg(void * arg) override;

    void push_arg(short arg) override;

    void call_void() override;

    double call_double() override;

    int call_int() override;

    bool call_bool() override;

    char call_char() override;

    float call_float() override;

    long call_long() override;

    long long call_long_long() override;

    void * call_ptr() override;

    short call_short() override;

    local_function_caller(const function_metadata_t * metadata);

    virtual ~local_function_caller();

  private:
    template <typename T>
    bool cast_argument(T arg)
    {
        if (metadata->arguments.size() <= argument_index) {
            throw function_call_error_t("while calling function \"" + metadata->name +
                                        "\": too many arguments");
        }

        const auto & argument_metadata = metadata->arguments[argument_index];
        if (argument_metadata.type != type_subsystem::enumerate_type<T>()) {
            switch (argument_metadata.type) {
            case type_e::Bool:
                if constexpr (std::is_convertible<T, bool>::value) {
                    push_arg(static_cast<bool>(arg));
                }
                break;
            case type_e::Char:
                if constexpr (std::is_convertible<T, char>::value) {
                    push_arg(static_cast<char>(arg));
                }
                break;
            case type_e::Short:
                if constexpr (std::is_convertible<T, short>::value) {
                    push_arg(static_cast<short>(arg));
                }
                break;
            case type_e::Float:
                if constexpr (std::is_convertible<T, float>::value) {
                    push_arg(static_cast<float>(arg));
                }
                break;
            case type_e::Double:
                if constexpr (std::is_convertible<T, double>::value) {
                    push_arg(static_cast<double>(arg));
                }
                break;
            case type_e::Int:
                if constexpr (std::is_convertible<T, int>::value) {
                    push_arg(static_cast<int>(arg));
                }
                break;
            case type_e::Long:
                if constexpr (std::is_convertible<T, long>::value) {
                    push_arg(static_cast<long>(arg));
                }
                break;
            case type_e::LongLong:
                if constexpr (std::is_convertible<T, long long>::value) {
                    push_arg(static_cast<long long>(arg));
                }
                break;
            case type_e::Pointer:
                if constexpr (std::is_convertible<T, void *>::value) {
                    push_arg(static_cast<void *>(arg));
                }
                break;
            default:
                break;
            }
            return true;
        }
        return false;
    }
};
} // namespace interop
