//
// Created by islam on 14.01.18.
//

#include "metadata_backward_compatibility.h"
#include "exceptions.h"

namespace interop {
module_metadata_t convert_metadata_to_current(const version_t & version, const void * metadata)
{
    /** version of module's interop ABI matches version of node **/
    if (version == INTEROP_FRAMEWORK_ABI_VERSION) {
        return *reinterpret_cast<const module_metadata_t *>(metadata);
    } else {
        throw error_t("unable to convert metadata of version " + version.to_string() +
                      " to current format version (" + INTEROP_FRAMEWORK_ABI_VERSION.to_string() +
                      ")");
    }
}
} // namespace interop
