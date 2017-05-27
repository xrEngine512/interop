//
// Created by islam on 07.05.17.
//

#include "module.h"
#include "node.h"
#include "local_function_caller.h"

#include "module_definitions.h"

#include <exceptions.h>
#include <logger.h>

#include <algorithm>

using namespace std;


namespace mosaic {
    module::module(shared_library && _library): library(move(_library)) {
        /** registration stage begin **/
        auto register_module = reinterpret_cast<register_module_function>(library.symbol("_mosaic_module_register"));
        register_module();

        metadata = *reinterpret_cast<module_metadata*>(library.symbol("_mosaic_module_metadata"));
        if (metadata.name.empty()) {
            metadata.name = library.name();
        }
        validate_metadata();
        /** registration stage end **/
    }

    void module::link(module_context & context) const {
        try {
            auto initialize_module = reinterpret_cast<initialize_module_function>(library.symbol("_mosaic_module_initialize"));
            initialize_module(context);
        } catch (symbol_loading_error & e) {
            mosaic_logger(warning, e.what());
        }
    }

    module::module(module && other): library(move(other.library)) {
        metadata = other.metadata;

        other.metadata = module_metadata();
    }

    module::~module() {

    }

    void module::unload() {
        library.unload();
    }

    const std::string & module::name() const {
        return metadata.name;
    }

    void module::validate_metadata() const {
        if (metadata.name.empty()) {
            throw module_validation_error("module name is empty");
        }
    }

    function_view & module::function(const std::string_view & name) const {
        auto function_iterator = used_functions.find(name.to_string());
        if (function_iterator == used_functions.end()) {
            auto it = find_if(metadata.functions.begin(), metadata.functions.end(), [&](const function_metadata & fn_metadata){
                return name == fn_metadata.name;
            });
            if (it == metadata.functions.end()) {
                throw function_lookup_error("function with name \"" + name.to_string() + "\" not found in module " + metadata.name);
            }
            return used_functions[name.to_string()] = function_view(make_unique<local_function_caller>(&*it));
        }

        return function_iterator->second;
    }

    void module::listen(const std::string_view & module_name, const std::function<void()> handler) {

    }
}