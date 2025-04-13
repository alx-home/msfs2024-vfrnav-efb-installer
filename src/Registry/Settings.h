#pragma once

#include <windows/Registry/Registry.h>

namespace registry {

template <Store store, class Parent>
class Settings : public Key<store, "MSFS.VFRNav.server", Parent, true> {
public:
   Settings() = default;

   Value<std::string, Settings, "LaunchMode"> launch_mode_;
   Value<std::string, Settings, "Community">  community_;
   Value<std::string, Settings, "Install">    destination_;

   static constexpr Values values_{
      &Settings::launch_mode_
   };

   static constexpr KeysPtr<> keys_{};
};

}  // namespace registry