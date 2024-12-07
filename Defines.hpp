#ifndef DEFINES
#define DEFINES

#include <filesystem>

namespace Defines {
    static constexpr auto kResourcesPathname{"/home/kolya/code/study/cv/cvlabs/resources"};
    static constexpr auto kQuitKey {'q'};
    static inline std::filesystem::path resourcesPath(kResourcesPathname); 
}

#endif /* DEFINES */
