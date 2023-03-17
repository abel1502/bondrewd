#pragma once

#include <bondrewd/internal/common.hpp>

#include <memory>
#include <concepts>
#include <type_traits>
#include <new>
#include <vector>
#include <stdexcept>
#include <ranges>


namespace bondrewd::util {


#pragma region Forward declarations
class Arena;

template <typename T, bondrewd::util::Arena *>
class arena_ptr;
#pragma endregion Forward declarations


#pragma region Arena
/**
 * An arena allocator with reference counting.
 *
 * Strictly speaking, it isn't exactly an arena, since the actual objects are allocated
 * in the common heap. What it does is allocate the tracking structures in an arena,
 * allowing for less memory fragmentation than many independent shared_ptrs would
 * have caused.
 *
 * Note that while it has a global instance, you may create your own arenas. The only requirement
 * is that their address must be a constant expression (i.e. they must be a global variable).
 */
class Arena {
public:
    #pragma region Instance
    static Arena instance;
    #pragma endregion Instance

    #pragma region Constructors
    Arena() {}
    #pragma endregion Constructors

    #pragma region Service constructors
    // Arena can't be moved because arena_ptr's would be invalidated.

    Arena(const Arena &) = delete;
    Arena(Arena &&) = delete;
    Arena &operator=(const Arena &) = delete;
    Arena &operator=(Arena &&) = delete;
    #pragma endregion Service constructors

    #pragma region Factory
    template <typename T, bondrewd::util::Arena *arena /*= &Arena::instance*/, typename Allocator = std::allocator<T>, typename ... Args>
    static arena_ptr<T, arena> make_arena_ptr(Args &&... args) {
        T *ptr = Allocator{}.allocate(1);

        new (ptr) T{std::forward<Args>(args)...};

        return arena_ptr<T, arena>{arena->allocations.emplace_back(ptr)};
    }
    #pragma endregion Factory

protected:
    #pragma region Helper types
    class Allocation {
    public:
        #pragma region Constructors
        template <typename T>
        Allocation(T *ptr) :
            ptr{ptr},
            deleter{[](void *ptr_) { delete static_cast<T *>(ptr_); }} {}
        #pragma endregion Constructors

        #pragma region Service constructors
        Allocation(const Allocation &) = delete;
        Allocation(Allocation &&other) : ptr{nullptr}, deleter{nullptr}, refs{0} {
            *this = std::move(other);
        }
        Allocation &operator=(const Allocation &) = delete;
        Allocation &operator=(Allocation &&other) {
            std::swap(ptr, other.ptr);
            std::swap(deleter, other.deleter);
            std::swap(refs, other.refs);
            return *this;
        }
        #pragma endregion Service constructors

        #pragma region Destructor
        ~Allocation() {
            if (ptr != nullptr) {
                deleter(ptr);
            }

            if (refs != 0) {
                ERR("WARNING: Arena destroyed with a dangling reference! (%p, refs=%u)\n", ptr, refs);
            }
        }
        #pragma endregion Destructor

        #pragma region API
        void *take() {
            ++refs;
            return ptr;
        }

        bool release() {
            if (--refs == 0) {
                deleter(ptr);
                ptr = nullptr;
                return true;
            }

            return false;
        }

        constexpr void *get_raw() const {
            return ptr;
        }

        constexpr unsigned get_refs() const {
            return refs;
        }

        constexpr operator bool() const {
            return ptr != nullptr;
        }
        #pragma endregion API

    protected:
        #pragma region Fields
        void *ptr;
        void (*deleter)(void *);
        unsigned refs = 0;
        #pragma endregion Fields
    };

    template <typename, Arena *>
    friend class arena_ptr;
    #pragma endregion Helper types

    #pragma region Fields
    std::vector<Allocation> allocations{};
    #pragma endregion Fields

    #pragma region arena_ptr API
    void arena_ptr_incref(void *ptr) {
        if (!ptr) return;

        auto &allocation = find_allocation(ptr);

        allocation.take();
    }

    void arena_ptr_decref(void *ptr) {
        static int recursion_depth = 0;

        if (!ptr) return;

        ++recursion_depth;

        auto &allocation = find_allocation(ptr);

        if (allocation.release() && recursion_depth == 1) {
            // Remove all null allocations
            allocations.erase(
                std::remove_if(allocations.begin(), allocations.end(), [](auto &allocation_) {
                    return !allocation_;
                }),
                allocations.end()
            );

        }

        --recursion_depth;
    }

    bool arena_ptr_is_unique(void *ptr) {
        if (!ptr) return true;

        auto &allocation = find_allocation(ptr);

        return allocation.get_refs() == 1;
    }

    Allocation &find_allocation(void *ptr) {
        // TODO: Speed up with a map?
        // For now, my optimization is to search backwards, since the most recent allocations
        // are the most likely to be referenced. This is, strictly speaking, not true for
        // destructor calls at the end of the program, but performance isn't really a concern
        // at that point.
        for (auto &allocation : std::ranges::views::reverse(allocations)) {
            if (allocation.get_raw() == ptr) {
                return allocation;
            }
        }

        ERR("WARNING: Arena reference to a dangling pointer! (%p)\n", ptr);
        abort();  // TODO: Maybe less harsh?
    }
    #pragma endregion arena_ptr API

};
#pragma endregion Arena


#pragma region arena_ptr
template <typename T, bondrewd::util::Arena *arena = &Arena::instance>
class arena_ptr {
public:
    #pragma region Constants and typedefs
    using element_type = T;
    #pragma endregion Constants and typedefs

    #pragma region Constructors
    arena_ptr() : ptr{nullptr} {};

    arena_ptr(nullptr_t) : arena_ptr() {};
    #pragma endregion Constructors

    #pragma region Service constructors
    arena_ptr(const arena_ptr &other) : ptr{other.ptr} {
        arena->arena_ptr_incref(ptr);
    }

    arena_ptr(arena_ptr &&other) : ptr{other.ptr} {
        other.ptr = nullptr;
    }

    arena_ptr &operator=(const arena_ptr &other) {
        if (ptr != other.ptr) {
            arena->arena_ptr_decref(ptr);
            ptr = other.ptr;
            arena->arena_ptr_incref(ptr);
        }

        return *this;
    }

    arena_ptr &operator=(arena_ptr &&other) {
        if (this != &other) {
            arena->arena_ptr_decref(ptr);
            ptr = other.ptr;
            other.ptr = nullptr;
        }

        return *this;
    }
    #pragma endregion Service constructors

    #pragma region Destructor
    ~arena_ptr() {
        arena->arena_ptr_decref(ptr);
        ptr = nullptr;
    }
    #pragma endregion Destructor

    #pragma region API
    void reset() {
        arena->arena_ptr_decref(ptr);
        ptr = nullptr;
    }

    void swap(arena_ptr &other) noexcept {
        std::swap(ptr, other.ptr);
    }

    T *get() const noexcept {
        return ptr;
    }

    operator bool() const noexcept {
        return ptr != nullptr;
    }

    T &operator*() const noexcept {
        return *ptr;
    }

    T *operator->() const noexcept {
        return ptr;
    }

    auto operator<=>(const arena_ptr &other) const noexcept {
        return ptr <=> other.ptr;
    }

    bool operator==(const arena_ptr &other) const noexcept = default;
    bool operator!=(const arena_ptr &other) const noexcept = default;
    bool operator<=(const arena_ptr &other) const noexcept = default;
    bool operator>=(const arena_ptr &other) const noexcept = default;
    bool operator< (const arena_ptr &other) const noexcept = default;
    bool operator> (const arena_ptr &other) const noexcept = default;

    bool is_unique() const {
        return arena->is_unique(ptr);
    }
    #pragma endregion API

    // TODO: Casting to compatible types!

protected:
    #pragma region Fields
    T *ptr;
    #pragma endregion Fields

    #pragma region Private constructors
    arena_ptr(Arena::Allocation &allocation) :
        ptr{static_cast<T *>(allocation.take())} {}
    #pragma endregion Private constructors

    #pragma region Friends
    friend class Arena;
    #pragma endregion Friends

};
#pragma endregion arena_ptr


#pragma region make_arena_ptr
template <typename T, Arena *arena /*= &Arena::instance*/, typename Allocator = std::allocator<T>, typename ... Args>
arena_ptr<T, arena> make_arena_ptr(Args &&... args) {
    return Arena::make_arena_ptr<T, arena, Allocator>(std::forward<Args>(args)...);
}
#pragma endregion make_arena_ptr


#pragma region Concepts
template <typename T, Arena *arena>
concept arena_ptr_at = std::same_as<std::decay_t<T>, arena_ptr<typename T::element_type, arena>>;
#pragma endregion Concepts


}  // namespace bondrewd::util


#pragma region std::swap
namespace std {


template <typename T, bondrewd::util::Arena *arena>
void swap(bondrewd::util::arena_ptr<T, arena> &a, bondrewd::util::arena_ptr<T, arena> &b) noexcept {
    a.swap(b);
}


}  // namespace std
#pragma endregion std::swap
