//
// Created by islam on 26.10.17.
//

#include "object_view.h"
#include "function_view.h"

#include <algorithm>

using namespace std;

namespace interop {
void * object_view_t::get_pointer() { return pointer; }

function_ptr_t object_view_t::function(const string & name) const
{

    auto it =
        find_if(metadata.methods.begin(), metadata.methods.end(),
                [&](const function_metadata_t & fn_metadata) { return name == fn_metadata.name; });
    if (it == metadata.methods.end()) {
        throw function_lookup_error_t("function with name \"" + name + "\" not found in module " +
                                      metadata.name);
    }

    return make_shared<function_view_t>(const_cast<object_view_t &>(*this), *it);
}

object_view_t::~object_view_t()
{
    reinterpret_cast<void (*)(void *)>(metadata.destructor.pointer)(pointer);
}
} // namespace interop
