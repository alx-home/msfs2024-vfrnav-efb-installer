#pragma once

#include "Settings.h"

#include <windows/Registry/Registry.h>

namespace registry {

template <Store store, class Parent>
class AlxHome : public Key<store, "Software\\Alx Home", Parent, true> {
public:
   using key_t = Key<store, "Software\\Alx Home", Parent, true>;

   AlxHome() = default;

   KeyPtr<Settings<store, AlxHome>> settings_;

   void deleteKey() {
      this->keys_apply([](auto&& key) {
         key.deleteKey();
      });

      if (auto [keys, values] = key_t::info(); !keys && !values) {
         key_t::deleteKey();
      }
   }

   static constexpr Values values_{
   };

   static constexpr KeysPtr keys_{
      &AlxHome::settings_
   };
};

}  // namespace registry