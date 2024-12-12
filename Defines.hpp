#ifndef DEFINES
#define DEFINES

#include <filesystem>

namespace Defines {
    static constexpr auto kResourcesPathname{"/home/og/coding/study/cv/labs/Media"};
    static constexpr auto kQuitKey {'q'};
    static inline std::filesystem::path resourcesPath(kResourcesPathname); 
}

#endif /* DEFINES */
