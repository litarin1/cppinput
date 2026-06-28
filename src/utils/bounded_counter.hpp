#include <limits>

template <class T>
struct BoundedCounter {
    T value;

    constexpr static T max = std::numeric_limits<T>().max();
    constexpr static T min = std::numeric_limits<T>().min();

    constexpr BoundedCounter(const T& value = 0) : value(value) {}

    constexpr BoundedCounter& operator++() {
        if (value != max) value++;
        return *this;
    }
    constexpr BoundedCounter& operator--() {
        if (value != min) value--;
        return *this;
    }
    constexpr BoundedCounter operator++(int) {
        if (value != max) {
            BoundedCounter old{*this};
            value++;
            return old;
        } else
            return *this;
    }
    constexpr BoundedCounter operator--(int) {
        if (value != min) {
            BoundedCounter old{*this};
            value--;
            return old;
        } else
            return *this;
    }
    constexpr bool is_max() const { return value == max; }
    constexpr bool is_min() const { return value == min; }
    constexpr operator T() const { return value; }
    constexpr auto operator<=>(const T& other) const { return value <=> other; }
};
