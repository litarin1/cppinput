#include <cstddef>
#include <optional>
#include <type_traits>

// N must be a power of 2
template <class T, size_t N, bool external = false, bool drop_newest = true>
class RingBuffer {
    static_assert(N > 0 && (N & (N - 1)) == 0, "RingBuffer N must be a power of 2");

    size_t m_write = 0;
    size_t m_read = 0;

public:
    // using ArrayType = std::conditional_t<external, T*, T[N]>;
    using ArrayType = std::conditional_t<external, T*, T[N]>;

private:
    ArrayType m_array;

public:
    // @returns true on successful write; false if buffer is full and drop_newest == true
    constexpr bool push_back(T obj) {
        if constexpr (drop_newest)
            if (is_full()) return false;

        m_array[m_write % N] = obj;
        m_write++;

        if constexpr (!drop_newest)
            if (is_full()) m_read = m_write - N;
        return true;
    }
    template <class... Args>
    constexpr bool emplace_back(Args&&... args) {
        if constexpr (drop_newest)
            if (is_full()) return false;

        m_array[m_write % N] = T(std::forward(args...));
        m_write++;

        if constexpr (!drop_newest)
            if (is_full()) m_read = m_write - N;

        return true;
    }
    constexpr std::optional<T> pop_front() {
        if (m_read == m_write) return std::nullopt;
        return std::move(m_array[m_read++ % N]);
    }

    bool is_full() const { return m_write - m_read >= N; }

    RingBuffer()
        requires(!external)
    {}
    RingBuffer(T* array)
        requires(external)
        : m_array(array) {}
};
