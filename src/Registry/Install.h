#pragma once

#include "windows/Registry/impl/Registry.h"
#include <windows/Registry/Registry.h>

#include <string>

namespace registry {

template <Store store, class Parent>
class Uninstall : public Key<store, "Uninstall\\MSFS.VFRNav.server", Parent> {
public:
   Uninstall() = default;

   Value<std::string, Uninstall, "DisplayIcon">     icon_{};
   Value<std::string, Uninstall, "DisplayName">     name_{};
   Value<std::string, Uninstall, "Publisher">       publisher_{};
   Value<std::string, Uninstall, "DisplayVersion">  version_{};
   Value<std::string, Uninstall, "UninstallString"> uninstall_{};

   static constexpr Values values_{
      &Uninstall::icon_,
      &Uninstall::name_,
      &Uninstall::publisher_,
      &Uninstall::version_,
      &Uninstall::uninstall_,
   };

   static constexpr KeysPtr<> keys_{};
};

}  // namespace registry