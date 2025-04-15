#pragma once

#include "AlxHome.h"
#include "Install.h"
#include "Run.h"

namespace registry {

template <Store store, class Parent>
class CurrentVersion : public Key<store, "Software\\Microsoft\\Windows\\CurrentVersion", Parent, false> {
public:
   CurrentVersion() = default;

   KeyPtr<Uninstall<store, CurrentVersion>> uninstall_;
   KeyPtr<Run<store, CurrentVersion>>       run_;

   static constexpr Values values_{
   };

   static constexpr KeysPtr keys_{
      &CurrentVersion::uninstall_,
      &CurrentVersion::run_
   };
};

namespace details {

template <Store store>
class Registry : public registry::Key<store, "", void> {
public:
   KeyPtr<CurrentVersion<store, Registry>> current_version_;
   KeyPtr<AlxHome<store, Registry>>        alx_home_;

protected:
   Registry() = default;

public:
   static constexpr KeysPtr keys_{
      &Registry::current_version_,
      &Registry::alx_home_
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