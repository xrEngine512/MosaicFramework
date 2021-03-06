#pragma once

#include "exceptions.h"
#include "function_metadata.h"
#include "internals/common.h"

#include <functional>

/**
 * @brief represent any c++ callable object as c function
 */

namespace interop {
namespace internals {
namespace function_reflection {
namespace details {

template <typename CallablePtr, typename Return, typename... Args, size_t... Indices>
universal_wrapper_t wrap_universally(std::index_sequence<Indices...>)
{
    return [](void * callable_ptr, arg_pack_t args) -> val_t {
        const auto & callable = *reinterpret_cast<CallablePtr>(callable_ptr);
        if constexpr (std::is_void<Return>::value) {
            callable(args[Indices].as<Args>()...);
            return {};
        } else {
            return callable(args[Indices].as<Args>()...);
        }
    };
}
template <typename F, typename R, typename... Args>
universal_wrapper_t wrap_universally()
{
    return wrap_universally<F, R, Args...>(std::index_sequence_for<Args...>());
}
} // namespace details

template <typename... Args>
struct arguments_reflector_t {
    static std::vector<type_metadata_t> arguments()
    {
        std::vector<type_metadata_t> arguments_metadata;
        arguments_metadata.reserve(sizeof...(Args));
        unpack_m(meta::describe_type<Args>(arguments_metadata));
        return arguments_metadata;
    }
};

template <typename R, typename... Args>
struct signature_reflector_t: arguments_reflector_t<Args...> {
    static type_metadata_t return_type() { return meta::describe_type<R>(); }
};

template <typename Callable>
struct function_reflector_t;

template <typename R, typename... Args>
struct function_reflector_t<R (*)(Args...)>: signature_reflector_t<R, Args...> {
    using this_t       = function_reflector_t;
    using c_function_t = R (*)(Args...);

    static universal_wrapper_t wrap_universally()
    {
        return details::wrap_universally<c_function_t, R, Args...>();
    }

    static function_metadata_t reflect(std::string name, c_function_t c_function)
    {
        return {
            reinterpret_cast<void *>(c_function),
            nullptr,
            wrap_universally(),
            std::move(name),
            this_t::arguments(),
            this_t::return_type(),
        };
    }
};

struct functor_reflector_t {
    template <typename R, typename... Args>
    static function_metadata_t reflect(std::string name, std::function<R(Args...)> functor)
    {
        using c_function_t  = R (*)(void *, Args...);
        using cpp_functor_t = decltype(functor);
        using reflected_t   = signature_reflector_t<R, Args...>;
        static std::unordered_map<std::string, cpp_functor_t> storage; // FIXME: this is just lame

        auto [iter, inserted] = storage.emplace(name, std::move(functor));

        if (!inserted) {
            throw register_error_t("name collision: " + name);
        }

        c_function_t proxy = [](void * context, Args... args) -> R {
            auto ctx = static_cast<cpp_functor_t *>(context);
            return (*ctx)(std::forward<Args>(args)...);
        };

        return {
            reinterpret_cast<void *>(proxy),
            &iter->second,
            details::wrap_universally<cpp_functor_t *, R, Args...>(),
            std::move(name),
            reflected_t::arguments(),
            reflected_t::return_type(),
        };
    }
};

} // namespace function_reflection
} // namespace internals
} // namespace interop
