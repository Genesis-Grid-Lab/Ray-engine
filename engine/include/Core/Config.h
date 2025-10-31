#pragma once

#include "PlatformDetection.h"

//tmp
#define RE_DEBUG true

namespace RE {

    //--------------------- Scope = unique pointer --------------------
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args){

        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    //--------------------- Ref = shared pointer -----------------------
    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args){

        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}