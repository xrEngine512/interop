//
// Created by islam on 14.01.18.
//

#include "metadata_backward_compatibility.h"
#include "exceptions.h"

namespace mosaic {
    module_metadata convert_metadata_to_current(const version_t & version, const void * metadata) {
        if (version == MOSAIC_FRAMEWORK_API_VERSION) {
            return *reinterpret_cast<const module_metadata *>(metadata);
        } else {
            throw error_t("unable to convert metadata of version " + version.to_string() + " to current format version (" + MOSAIC_FRAMEWORK_API_VERSION.to_string() + ")");
        }
    }
}
