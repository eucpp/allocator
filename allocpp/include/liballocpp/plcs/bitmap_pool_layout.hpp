#ifndef ALLOCPP_BITMAP_POOL_LAYOUT_HPP
#define ALLOCPP_BITMAP_POOL_LAYOUT_HPP

#include <limits>
#include <climits>
#include <bitset>
#include <cassert>
#include <exception>
#include <string>
#include <sstream>
#include <array>

#include <liballocpp/alloc_messaging.hpp>
#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/concepts/tags.hpp>
#include <liballocpp/ptrs/block_ptr.hpp>
#include <liballocpp/utils/pointer.hpp>
#include <liballocpp/utils/math.hpp>


namespace allocpp { namespace plcs {

template <typename Ptr>
class bitmap_pool_layout
{
    typedef unsigned long long ull_t;
public:
    ALLOCPP_CONCEPT_ASSERT((concepts::RandomAccessPtr<Ptr>));

    typedef Ptr pointer_type;
    typedef typename utils::pointer_traits<pointer_type>::size_type size_type;
    typedef Ptr internal_pointer_type;
    typedef concepts::memory_layout_policy_tag concept_tag;

    class incorrect_request : public std::exception
    {
    public:
        incorrect_request(const ptrs::block_ptr<internal_pointer_type>& blk,
                          const alloc_request<pointer_type>& req)
        {
            std::stringstream ss;
            ss << "Cannot fit allocation request with given block" << std::endl;
            ss << "block size: " << blk.size() << "; block align: " << blk.alignment()
               << "; request size: " << req.size() << "; request align: " << req.alignment() << std::endl;
            m_msg = ss.str();
        }

        const char* what() const noexcept override
        {
            return m_msg.c_str();
        }
    private:
        std::string m_msg;
    };

    static constexpr size_type max_size()
    {
        return std::numeric_limits<size_type>::max();
    }

    static constexpr size_type max_alignment()
    {
        // bitmap layout potentially can satisfy to any alignment
        // (depending on upstream allocator)
        return std::numeric_limits<size_type>::max();
    }

    static size_type block_size(const alloc_request<pointer_type>& req)
    {
        return max_cnt() * req.size();
    }

    bitmap_pool_layout(const ptrs::block_ptr<internal_pointer_type>& blk,
                       const alloc_request<pointer_type>& req)
        : m_blk(blk)
        , m_cnt(blk.size() / req.size())
        , m_bitmap(get_bitmap(m_cnt))
    {
        assert(blk.size() % req.size() == 0);

        if (m_cnt > max_cnt() || (req.alignment() && (blk.alignment() % req.alignment() != 0))) {
            throw incorrect_request(blk, req);
        }
    }

    bool is_feasible(const alloc_request<pointer_type>& req)
    {
        std::string s = m_bitmap.to_string();
        return m_bitmap.any();
    }

    alloc_response<pointer_type> allocate(const alloc_request<pointer_type>& req)
    {
        assert(is_feasible(req));

        size_t ind = utils::msb(m_bitmap.to_ullong()) - 1;
        m_bitmap[ind] = false;
        auto blk = ptrs::block_ptr<pointer_type>(m_blk.ptr() + ind * req.size(), req.size(), req.alignment());
        return typename alloc_response<pointer_type>::builder()
                .set_memory_block(blk)
                .build();
    }

    bool is_feasible(const dealloc_request<pointer_type>& req)
    {
        return m_blk.owns(req.ptr());
    }

    dealloc_response<pointer_type> deallocate(const dealloc_request<pointer_type>& req)
    {
        assert(is_feasible(req));

        size_t ind = (req.ptr() - m_blk.ptr()) + 1;
        m_bitmap[ind] = true;
        return typename dealloc_response<pointer_type>::builder().build();
    }

    ownership owns(pointer_type p) const
    {
        return m_blk.owns(p) ? ownership::OWNS : ownership::NOT_OWNS;
    }

    bool empty() const
    {
        return m_bitmap.count() == m_cnt;
    }

    ptrs::block_ptr<internal_pointer_type> block() const
    {
        return m_blk;
    }
private:
    static constexpr size_type max_cnt()
    {
        return CHAR_BIT * sizeof(unsigned long long);
    };

    // returns bitmap suitable for storing bits for cnt objects
    static constexpr ull_t get_bitmap(size_type cnt)
    {
        return (~ (ull_t)0) >> (max_cnt() - cnt);
    }

    typedef std::bitset<max_cnt()> bitmap_type;

    ptrs::block_ptr<internal_pointer_type> m_blk;
    size_type m_cnt;
    bitmap_type m_bitmap;
};

}}

#endif //ALLOCPP_BITMAP_POOL_LAYOUT_HPP