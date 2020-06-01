#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>

namespace apfd::common {

class Singleton {
  public:  
    class Strong {
      public:
        template<class C, class... Args>
        static std::shared_ptr<C> create(Args&&... args) {
          auto index = std::type_index(typeid(C));
          auto cached = strongSingleton.find(index);
          if (cached!=strongSingleton.end()) {
            auto shared = cached->second;
            return std::static_pointer_cast<C>(shared);
          }
          auto obj = new C(std::forward<Args>(args)...);
          auto made = std::shared_ptr<C>(obj,D<C>());
          strongSingleton[index]=made;
          return made;
        }
      protected:
        template<class C>
        class D { public: void operator()(C* p) const { delete p; } };
    };
    class Weak {
      public:
        template<class C, class... Args>
        static std::shared_ptr<C> create(Args&&... args) {
          auto index = std::type_index(typeid(C));
          auto cached = weakSingleton.find(index);
          if (cached!=weakSingleton.end()) {
            auto shared = cached->second.lock();
            return std::static_pointer_cast<C>(shared);
          }
          auto obj = new C(std::forward<Args>(args)...);
          auto made = std::shared_ptr<C>(obj,D<C>());
          weakSingleton[index]=made;
          return made;
      }
      protected:
        template<class C>
        class D { public: void operator()(C* p) const { delete p; } };
    };
    template<class C>
    static std::shared_ptr<C> get() {
      auto index = std::type_index(typeid(C));
      auto weak = weakSingleton.find(index);
      if (weak!=weakSingleton.end()) {
        auto shared = weak->second.lock();
        return std::static_pointer_cast<C>(shared);
      }
      auto strong = strongSingleton.find(index);
      if (strong!=strongSingleton.end()) {
        auto shared = strong->second;
        return std::static_pointer_cast<C>(shared);
      }
      return std::shared_ptr<C>();
    }
  private:
    static std::unordered_map<std::type_index,std::weak_ptr<void>> weakSingleton;
    static std::unordered_map<std::type_index,std::shared_ptr<void>> strongSingleton;
}; 

}