#pragma once

#include <bondrewd/internal/common.hpp>

#include <concepts>
#include <type_traits>
#include <variant>


namespace bondrewd::ctime {


#pragma region Forward Declarations
class Object;

#pragma region objectlike
template <typename T>
concept objectlike = std::derived_from<std::remove_cv_t<T>, Object>;
#pragma endregion objectlike

template <objectlike To>
class object_ptr_to;
#pragma endregion Forward Declarations


#pragma region object_ptr
class object_ptr {
public:
    #pragma region Constants and typedefs
    using element_type = Object;
    #pragma endregion Constants and typedefs

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

    template <objectlike To>
    object_ptr_to<To> as() const {
        return object_ptr_to<To>(ptr);
    }
    #pragma endregion API

protected:
    #pragma region Fields
    Object *ptr = nullptr;
    #pragma endregion Fields

    #pragma region Private constructor
    explicit object_ptr(Object *ptr);
    #pragma endregion Private constructor

};
#pragma endregion object_ptr


#pragma region object_ptr_to
template <objectlike To>
class object_ptr_to : public object_ptr {
public:
    #pragma region Constants and typedefs
    using element_type = To;
    #pragma endregion Constants and typedefs

    #pragma region Constructors
    object_ptr_to() : object_ptr() {}
    object_ptr_to(std::nullptr_t) : object_ptr() {}
    #pragma endregion Constructors

    #pragma region Service constructors
    object_ptr_to(const object_ptr_to &other) = default;
    object_ptr_to(object_ptr_to &&other) = default;
    object_ptr_to &operator=(const object_ptr_to &other) = default;
    object_ptr_to &operator=(object_ptr_to &&other) = default;
    #pragma endregion Service constructors

    #pragma region Destructor
    ~object_ptr_to() = default;
    #pragma endregion Destructor

    #pragma region Casts
    object_ptr_to(object_ptr ptr) :
        object_ptr(ptr) {
        
        if (ptr && !dynamic_cast<element_type *>(ptr.get())) {
            throw std::bad_cast();
        }
    }

    operator object_ptr() const {
        return object_ptr(ptr);
    }

    operator object_ptr_to<const element_type>() const {
        return object_ptr_to<const element_type>(ptr);
    }
    #pragma endregion Casts

    #pragma region API
    element_type *get() const noexcept {
        return dynamic_cast<element_type *>(ptr);
    }

    element_type &operator*() const noexcept {
        return *get();
    }

    element_type *operator->() const noexcept {
        return get();
    }
    #pragma endregion API

    #pragma region Create
    template <typename ... Args>
    static object_ptr_to create(Args &&... args) {
        return object_ptr_to(
            new element_type(
                std::forward<Args>(args)...
            )
        );
    }
    #pragma endregion Create

};
#pragma endregion object_ptr_to


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
    // The virtual methods are api that might be overriden by builtin types
    // either for performance reasons, or to provide some fundamental implementations

    size_t get_ref_cnt() const {
        return ref_cnt;
    }

    object_ptr get_type() const {
        return type;
    }

    /// Returns the trait implementation for the given trait.
    /// The returned implementation is effective for this object.
    object_ptr get_effective_trait_impl(object_ptr trait) const;

    // TODO: Decide on and add some more methods?
    #pragma endregion API

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


#pragma region std specializations
// Note: object_ptr and object_ptr_to are hashed by-identity.
//       I probably want to use a provided Hash (Key?) implementation instead

template <>
struct std::hash<bondrewd::ctime::object_ptr> {
    size_t operator()(const bondrewd::ctime::object_ptr &ptr) const noexcept {
        return std::hash<bondrewd::ctime::Object *>()(ptr.get());
    }
};

template <bondrewd::ctime::objectlike To>
struct std::hash<bondrewd::ctime::object_ptr_to<To>> {
    size_t operator()(const object_ptr_to<To> &ptr) const noexcept {
        return std::hash<To *>()(ptr.get());
    }
};
#pragma endregion std specializations