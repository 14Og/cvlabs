#ifndef DEFINES
#define DEFINES

#include <filesystem>

namespace Defines {
    static constexpr auto kResourcesPathname{MEDIA_DIR};
    static constexpr auto kQuitKey {'q'};
    static constexpr auto kMakePhotoKey {'p'};
    static constexpr auto kSpaceKey {' '};
    static inline std::filesystem::path resourcesPath(kResourcesPathname); 
}

#endif /* DEFINES */
