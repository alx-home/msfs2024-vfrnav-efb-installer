#pragma once

#include "Settings.h"

#include <windows/Registry/Registry.h>

namespace registry {

template <Store store, class Parent>
class AlxHome : public Key<store, "Software\\alx-home", Parent> {
public:
   using key_t = Key<store, "Software\\alx-home", Parent>;

   AlxHome() = default;

   KeyPtr<Settings<store, AlxHome>> settings_;

   void clear() {
      this->key_t::clear();

      if (auto [keys, values] = this->info(); !keys && !values) {
         this->deleteKey();
      }
   }

   static constexpr Values values_{
   };

   static constexpr KeysPtr keys_{
      &AlxHome::settings_
   };
};

}  // namespace registry