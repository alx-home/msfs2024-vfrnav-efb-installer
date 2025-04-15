#pragma once

#include "windows/Registry/impl/Registry.h"
#include <windows/Registry/Registry.h>

#include <string>

namespace registry {

template <Store store, class Parent>
class Run : public Key<store, "Run", Parent, true> {
public:
   using key_t = Key<store, "Run", Parent, true>;

   Run() = default;

   Value<std::string, Run, "MSFS VFRNav Server"> value_{};

   static constexpr Values values_{
      &Run::value_,
   };

   void deleteKey() {
      value_.deleteValue();
   }

   static constexpr KeysPtr<> keys_{};
};

}  // namespace registry