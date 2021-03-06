//
// Created by islam on 09.04.18.
//

#include "function.h"
#include "common.hpp"
#include "module.h"
#include "object.hpp"

using namespace std;
using namespace v8;
using namespace interop::helpers;

namespace interop::platform_v8 {
namespace {
/*-----------<< utility >>------------*/

void exposed_function_callback(const FunctionCallbackInfo<Value> & info)
{
    auto isolate = info.GetIsolate();
    HandleScope scope(isolate);
    // FIXME: no lifetime guarantees whatsoever - options:
    // 1) create object whose lifetime is same as v8 function that will check weak function pointer
    //    and if needed will refetch it.
    // 2) same as 1 but refetching will be done by framework
    // 3) make function views persistent and update them instead of recreation
    // 4) use embedder field as universal entry point
    // Analysis:
    // 1 - some overhead with unpredictable execution time in case of refetch: forget about RT case
    //     function fetch must be thread-safe
    // 2 - needs sync
    // 3 - needs sync, but simpler, probably best current solution
    auto fn = static_cast<function_view_t *>(info.Data().As<External>()->Value());

    arg_pack_t args;
    args.reserve(info.Length());

    for (int i = 0; i < info.Length(); ++i) {
        args.push_back(from_v8(info[i]));
    }

    const auto & ret_val = fn->call_dynamic(std::move(args));
    if (!ret_val.empty()) {
        info.GetReturnValue().Set(to_v8(ret_val));
    }
}

/*----------->> utility <<------------*/
} // namespace

function_t::function_t(std::string name, Local<Function> handle, const module_t & module)
  : platform_function_t(std::move(name))
  , handle(module.get_isolate(), handle)
  , module(module)
{}

function_t::function_t(std::string name, Local<Function> handle, const object_t & object)
  : function_t(std::move(name), handle, object.get_module())
{
    this->object = &object;
}

val_t function_t::call_dynamic(arg_pack_t args) const
{
    auto isolate = module.get_isolate();

    // Enter the isolate
    Isolate::Scope isolate_scope(isolate);
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(isolate);

    auto local_context = module.get_context();

    // Enter the context
    Context::Scope context_scope(local_context);

    Local<Value> recv = object ? object->get_handle() : local_context->Global();

    return call_v8(recv, handle.Get(isolate), helpers::to_v8(std::move(args)));
}

Local<Object> function_t::constructor_call(arg_pack_t args) const
{
    auto isolate = module.get_isolate();

    // Enter the isolate
    Isolate::Scope isolate_scope(isolate);
    // Create a stack-allocated handle scope.
    EscapableHandleScope handle_scope(isolate);

    auto local_context = module.get_context();

    // Enter the context
    Context::Scope context_scope(local_context);

    return handle_scope.Escape(
        call_v8_as_constructor(handle.Get(isolate), helpers::to_v8(std::move(args))));
}

v8::Local<v8::Function> function_t::to_v8(const function_ptr_t & function)
{
    auto isolate = helpers::current_isolate();

    return Function::New(isolate, exposed_function_callback,
                         External::New(isolate, function.get()));
}

} // namespace interop::platform_v8
