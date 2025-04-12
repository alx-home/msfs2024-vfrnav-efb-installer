#pragma once

#include "Install.h"
#include "Settings.h"

namespace registry {

template <Store store, class Parent>
class CurrentVersion : public Key<store, R"(Software\Microsoft\Windows\CurrentVersion)", Parent, false> {
public:
   CurrentVersion() = default;

   KeyPtr<Uninstall<store, CurrentVersion>> uninstall_;

   static constexpr Values values_{
   };

   static constexpr KeysPtr keys_{
      &CurrentVersion::uninstall_
   };
};

namespace details {

template <Store store>
class Registry : public registry::Key<store, "", void> {
public:
   KeyPtr<CurrentVersion<store, Registry>> current_version_;
   KeyPtr<Settings<store, Registry>>       settings_;

protected:
   Registry() = default;

public:
   static constexpr KeysPtr keys_{
      &Registry::current_version_,
      &Registry::settings_
   };
};

}  // namespace details

template <Store store>
static constexpr auto&
get() {
   return Registry<store, details::Registry<store>>::get();
}

}  // namespace registry

template <Store store>
using Registry = registry::Registry<store, registry::details::Registry<store>>;