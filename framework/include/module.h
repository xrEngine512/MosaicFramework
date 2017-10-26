#pragma once
//
// Created by islam on 07.05.17.
//


#include "shared_library.h"

#include "module_metadata.h"
#include "module_view.h"

#include <unordered_map>


namespace mosaic {
    class module_context;

    class module: public module_view {
        shared_library  library;
        module_metadata metadata;

        mutable std::unordered_map<std::string, function_ptr> used_functions;

        const module_metadata & get_metadata() const override;
    public:
        module(shared_library && library);
        module(const module &)  = delete;
        module(module &&);

        void link(module_context & context) const;

        function_ptr function(const std::string & name) const override;




        void listen(const std::string_view & module_name, const std::function<void()> handler) override;

        const std::string& name() const override;

        ~module();

        void unload();

    private:
        void validate_metadata() const;
    };
}


