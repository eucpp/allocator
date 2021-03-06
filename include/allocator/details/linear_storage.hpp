#ifndef LINEAR_STORAGE_HPP
#define LINEAR_STORAGE_HPP

#include <type_traits>

namespace alloc_utility
{

namespace details
{

template <typename pointer, typename size_type>
class linear_storage
{
public:

    static_assert(std::is_same<typename std::pointer_traits<pointer>::element_type, std::uint8_t>::value,
                  "Type of pointed value should be uint8_t");

    linear_storage() noexcept:
        m_storage(nullptr)
      , m_storage_size(0)
      , m_offset(0)
    {}

    size_type storage_size() const noexcept
    {
        return m_storage_size;
    }

    bool is_memory_available(size_type mem_size) const noexcept
    {
        return m_storage && (mem_size <= m_storage_size) && (m_offset <= m_storage_size - mem_size);
    }

    bool is_owned(const pointer& ptr) const noexcept
    {
        return m_storage && (m_storage <= ptr) && (ptr < m_storage + m_storage_size);
    }

    pointer get_storage() const noexcept
    {
        return m_storage;
    }

    void set_storage(const pointer& storage, size_type size) noexcept
    {
        m_storage = storage;
        m_storage_size = size;
        m_offset = 0;
    }

    pointer allocate(size_type size) noexcept
    {
        pointer ptr = m_storage + m_offset;
        m_offset += size;
        return ptr;
    }

private:
    pointer m_storage;
    size_type m_storage_size;
    size_type m_offset;
};

} // namespace details

} // namespace alloc_utility

#endif // LINEAR_STORAGE_HPP
