#pragma once

#include <windows/Registry/Registry.h>

namespace registry {

template <Store store, class Parent>
class Settings : public Key<store, "Settings", Parent> {
public:
   Settings() = default;

   static constexpr Values values_{
   };

   static constexpr KeysPtr<> keys_{};
};

}  // namespace registry