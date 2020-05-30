#pragma once

#include <queue>
#include <vector>

namespace apfd::common {

template<class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>,
    class ErasureCompare = std::equal_to<typename Container::value_type>>
class priority_queue_ex : public std::priority_queue<T, Container, Compare>
{
  public:

    void erase(const T& value) {
      while (true) {
        auto it = std::find_if(this->c.begin(), this->c.end(), [value](T& cur) {
          return (ErasureCompare()(value,cur));
        });
        if (it != this->c.end()) {
          this->c.erase(it);
        } else {
          break;
        }
      }
      std::make_heap(this->c.begin(), this->c.end(), this->comp);
    }
};

}