#pragma once

#include "PlatformDetection.h"

//tmp
#define RE_DEBUG true

namespace RE {

     // runtime type
    template <typename T>
    inline constexpr uint32_t TypeID()
    {
        return static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&typeid(T)));
    }

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