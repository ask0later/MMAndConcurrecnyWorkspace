#ifndef MEMORY_MANAGEMENT_REFERECNCE_COUNTING_GC_INCLUDE_OBJECT_MODEL_H
#define MEMORY_MANAGEMENT_REFERECNCE_COUNTING_GC_INCLUDE_OBJECT_MODEL_H

#include <cstddef>
#include <utility>
#include <iostream>
//<string_view>

template <class T>
class Object;

template <class T, class... Args>
static Object<T> MakeObject([[maybe_unused]] Args... args)
{
    T *temp = new T{args...};
    Object<T> obj{temp};
    return obj;
}

template <class T>
class Object {
public:
    Object() : val_(nullptr) {}
    explicit Object(std::nullptr_t) : val_(nullptr) {}
    
    explicit Object([[maybe_unused]] T *ptr) : val_(ptr), refCount_(new size_t)
    {
        *refCount_ = 1;
    }

    ~Object()
    {
        if (refCount_ == nullptr)
        {
            return;
        }
        
        if (*refCount_ <= 1)
        {
            delete refCount_;
            delete val_;
        }
        else
        {
            (*refCount_)--;
            
            refCount_ = nullptr;
            val_ = nullptr;
        }
    }

    // copy semantic
    Object([[maybe_unused]] const Object<T> &other) : val_(other.val_), refCount_(other.refCount_) 
    {
        (*refCount_)++;
    }
    
    Object<T> &operator=([[maybe_unused]] const Object<T> &other)
    {
        if (this != &other)
        {
            if (UseCount() != 0)
            {
                (*refCount_)--;
            }
            
            val_ = other.val_;
            refCount_ = other.refCount_;
            (*refCount_)++;
        }

        return *this;
    }

    // move semantic
    Object([[maybe_unused]] Object<T> &&other)
    {
        std::swap(val_, other.val_);
        std::swap(refCount_, other.refCount_);
    }

    Object<T> &operator=([[maybe_unused]] Object<T> &&other)
    {
        if (this != &other)
        {
            std::swap(val_, other.val_);
            std::swap(refCount_, other.refCount_);
            
            other.~Object();                
        }

        return *this;
    }

    // member access operators
    T &operator*() const noexcept 
    {
        return *val_;
    }

    T *operator->() const noexcept 
    {
        return val_;
    }

    // internal access
    void Reset([[maybe_unused]] T *ptr)
    {
        if (val_ == ptr)
        {    
            return;
        }

        if (UseCount() == 0 || UseCount() == 1)
        {
            delete refCount_;
            delete val_;
        }
        else
        {
            (*refCount_)--;
        }

        refCount_ = new size_t;
        (*refCount_) = 1;
        val_ = ptr;
    }

    T *Get() const
    {
        return val_;
    }

    size_t UseCount() const
    {
        if (refCount_ == nullptr)
        {
            return 0;
        }
        return *refCount_;
    }    

private:

    void Dump() const
    {
        if (refCount_ == nullptr)
        {   
            std::cout << "reference counting is not exist" << std::endl;
        }
        else
        {
            std::cout << "reference counting " << UseCount() << std::endl;
        }
        
        if (val_ == nullptr)
        {   
            std::cout << "value of object is not exist" << std::endl;
        }
        else
        {
            std::cout << "value of object " << Get() << std::endl;
        }
    }

    size_t* refCount_ = nullptr;
    T* val_ = nullptr;
};

#endif  // MEMORY_MANAGEMENT_REFERECNCE_COUNTING_GC_INCLUDE_OBJECT_MODEL_H