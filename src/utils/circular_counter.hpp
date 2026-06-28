// #include <limits>

// template <class T>
// struct CircularCounter {
//     T value;

//     constexpr static T max = std::numeric_limits<T>().max();
//     constexpr static T min = std::numeric_limits<T>().min();

//     constexpr CircularCounter(const T& value = 0) : value(value) {}

//     constexpr CircularCounter& operator++() {
//         if (value == max)
//             value = min;
//         else
//             ++value;
//         return *this;
//     }
//     constexpr CircularCounter& operator--() {
//         if (value == min)
//             value = max;
//         else
//             --value;
//         return *this;
//     }
//     constexpr CircularCounter operator++(int) {
//         if (value == max) {
//             value = min;
//             return CircularCounter<T>{max};
//         } else
//             return *this;
//     }
//     constexpr CircularCounter operator--(int) {
//         if (value == min) {
//             value--;
//             return CircularCounter<T>(max);
//         } else
//             return *this;
//     }
//     constexpr operator T() const { return value; }
//     constexpr auto operator<=>(const T& other) const { return value <=> other; }
// };
