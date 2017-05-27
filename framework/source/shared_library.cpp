//
// Created by islam on 06.05.17.
//

#include "shared_library.h"

#include "os.h"
#include "exceptions.h"
#include "logger.h"

#include <boost/filesystem/path.hpp>


using namespace std;

using namespace boost::filesystem;

namespace mosaic {
    shared_library::shared_library(const std::string_view &path, const std::string_view &name) {
        this->handle = os::load_library(path);
        this->library_name = name.empty() ? boost::filesystem::path(path.data()).stem().string() : name.data();
        mosaic_logger(log, "Loaded shared library \"" + this->library_name + "\" by path \"" + path.data() + "\"");
    }

    shared_library::shared_library(shared_library && other) {
        this->handle = other.handle;
        this->library_name = other.library_name;

        other.reset();
    }

    shared_library::~shared_library() {
        try {
            if (handle) {
                mosaic_logger(warning, "Shared library unload from destructor. Call unload() before destroying object.");
                unload();
            }
        }
        catch (error &e) {
            mosaic_logger(error, e.what());
        }
    }

    library_symbol_handle shared_library::symbol(const string_view &name) const {
        return os::load_symbol(handle, name);
    }

    const std::string & shared_library::name() const {
        return library_name;
    }

    void shared_library::unload() {
        if (handle) {
            try {
                os::unload_library(handle);
                mosaic_logger(log, "Unloaded shared library \"" + this->library_name + "\"");
                reset();
            } catch (library_unloading_error &error) {
                if (!library_name.empty()) {
                    error.append("\nLibrary name: " + library_name);
                }
                throw;
            }
        }
    }

    void shared_library::reset() {
        handle = nullptr;
        library_name = "unknown";
    }
}