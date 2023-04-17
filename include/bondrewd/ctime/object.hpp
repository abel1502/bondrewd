#pragma once

#include <bondrewd/internal/common.hpp>
#include <bondrewd/ctime/object_ptr.hpp>
#include <bondrewd/ctime/scope.hpp>

#include <concepts>
#include <type_traits>


namespace bondrewd::ctime {


#pragma region Forward Declarations
class Object;
#pragma endregion Forward Declarations


#pragma region object_ptr
class object_ptr {
public:
    #pragma region Constructors
    object_ptr() = default;
    object_ptr(std::nullptr_t) {}
    #pragma endregion Constructors

    #pragma region Service constructors
    object_ptr(const object_ptr &other);
    object_ptr(object_ptr &&other);
    object_ptr &operator=(const object_ptr &other);
    object_ptr &operator=(object_ptr &&other);
    #pragma endregion Service constructors

    #pragma region Destructor
    ~object_ptr();
    #pragma endregion Destructor

    #pragma region API
    void reset();

    void swap(object_ptr &other) noexcept {
        std::swap(ptr, other.ptr);
    }

    Object *get() const noexcept {
        return ptr;
    }

    operator bool() const noexcept {
        return ptr != nullptr;
    }

    Object &operator*() const noexcept;

    Object *operator->() const noexcept {
        return ptr;
    }

    auto operator<=>(const object_ptr &other) const noexcept {
        return ptr <=> other.ptr;
    }

    bool operator==(const object_ptr &other) const noexcept = default;
    bool operator!=(const object_ptr &other) const noexcept = default;
    bool operator<=(const object_ptr &other) const noexcept = default;
    bool operator>=(const object_ptr &other) const noexcept = default;
    bool operator< (const object_ptr &other) const noexcept = default;
    bool operator> (const object_ptr &other) const noexcept = default;

    bool is_unique() const;
    #pragma endregion API

    #pragma region Create
    template <typename T, typename ... Args>
    requires std::derived_from<T, Object>
    static object_ptr create(Args &&... args) {
        return object_ptr(
            new T(
                std::forward<Args>(args)...
            )
        );
    }
    #pragma endregion Create

protected:
    #pragma region Fields
    Object *ptr = nullptr;
    #pragma endregion Fields

    #pragma region Private constructor
    explicit object_ptr(Object *ptr);
    #pragma endregion Private constructor

};
#pragma endregion object_ptr


#pragma region Object
class Object {
public:
    #pragma region Service constructors
    Object(const Object &) = delete;
    Object(Object &&) = delete;
    Object &operator=(const Object &) = delete;
    Object &operator=(Object &&) = delete;
    #pragma endregion Service constructors

    #pragma region Destructor
    virtual ~Object() = default;
    #pragma endregion Destructor

    #pragma region API
    size_t get_ref_cnt() const {
        return ref_cnt;
    }

    object_ptr get_type() const {
        return type;
    }
    #pragma endregion API

    #pragma region Virtual API
    // TODO
    #pragma endregion Virtual API

protected:
    #pragma region Fields
    size_t ref_cnt = 1;
    object_ptr type;
    #pragma endregion Fields

    #pragma region Constructor
    Object(object_ptr type) :
        type(type) {}
    #pragma endregion Constructor

    #pragma region object_ptr API
    friend class object_ptr;

    static void inc_ref(Object *obj) {
        if (!obj)
            return;
        
        ++obj->ref_cnt;
    }
    
    static void dec_ref(Object *obj) {
        if (!obj)
            return;
        
        if (--obj->ref_cnt == 0) {
            delete obj;
        }
    }

    static bool is_unique(Object *obj) {
        return !obj || obj->ref_cnt == 1;
    }
    #pragma endregion object_ptr API

};
#pragma endregion Object


#pragma region object_ptr late definitions
#pragma region Service constructors
inline object_ptr::object_ptr(const object_ptr &other) :
    ptr(other.ptr) {
    
    Object::inc_ref(ptr);
}

inline object_ptr::object_ptr(object_ptr &&other) :
    ptr(other.ptr) {
    
    other.ptr = nullptr;
}

inline object_ptr &object_ptr::operator=(const object_ptr &other) {
    if (ptr != other.ptr) {
        Object::dec_ref(ptr);
        ptr = other.ptr;
        Object::inc_ref(ptr);
    }

    return *this;
}

inline object_ptr &object_ptr::operator=(object_ptr &&other) {
    if (ptr != other.ptr) {
        Object::dec_ref(ptr);
        ptr = other.ptr;
        other.ptr = nullptr;
    }

    return *this;
}
#pragma endregion Service constructors

#pragma region Destructor
inline object_ptr::~object_ptr() {
    Object::dec_ref(ptr);
    ptr = nullptr;
}
#pragma endregion Destructor

#pragma region API
inline void object_ptr::reset() {
    Object::dec_ref(ptr);
    ptr = nullptr;
}

inline Object &object_ptr::operator*() const noexcept {
    return *ptr;
}

inline bool object_ptr::is_unique() const {
    return Object::is_unique(ptr);
}
#pragma endregion API

#pragma region Private constructor
explicit inline object_ptr::object_ptr(Object *ptr) :
    ptr(ptr) {
    
    assert(ptr);
    assert(ptr->ref_cnt == 1);
}
#pragma endregion Private constructor
#pragma endregion object_ptr late definitions


}  // namespace bondrewd::ctime