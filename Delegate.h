//
// Created by Jan_M720 on 15.10.2017.
//

#pragma once
#define FROM_FUNC(type, func) (doo::Delegate< type >::Create< func >())
#define FROM_METHOD(type,class_type, method, obj) (doo::Delegate< type >::Create< class_type, & class_type :: method > (obj))
#define FROM_FUNCTOR(type, functor) (doo::Delegate< type >::Create( functor ))

#include <iostream>

namespace doo
{

    typedef unsigned int DelegateID;

    template<typename TForward>
    class Delegate;

    template<typename TReturn, typename... TArgs>
    class Delegate<TReturn(TArgs...)>
    {
    public:

        Delegate() = default;

        Delegate(const Delegate &other) : m_call_func(other.m_call_func), m_alloc_func(other.m_alloc_func),
                                          m_delete_func(other.m_delete_func)
        {
            if (m_alloc_func != nullptr) m_callee = (*m_alloc_func)(other.m_callee);
            else m_callee = other.m_callee;
        }

        Delegate(Delegate &&other) : m_callee(other.m_callee), m_alloc_func(other.m_alloc_func),
                                     m_delete_func(other.m_delete_func), m_call_func(other.m_call_func)
        {
            other.m_call_func = nullptr;
            other.m_delete_func = nullptr;
            other.m_alloc_func = nullptr;
            other.m_call_func = nullptr;
        }


        Delegate &operator=(const Delegate &other)
        {
            if (&other == this)
                return *this;
            m_call_func = other.m_call_func;
            m_alloc_func = other.m_alloc_func;
            m_delete_func = other.m_delete_func;
            if (m_alloc_func != nullptr) m_callee = (*m_alloc_func)(other.m_callee);
            else m_callee = other.m_callee;
            return *this;
        }

        Delegate &operator=(Delegate &&other)
        {
            if (&other == this)
                return *this;
            m_call_func = other.m_call_func;
            m_alloc_func = other.m_alloc_func;
            m_delete_func = other.m_delete_func;
            m_callee = other.m_callee;
            other.m_call_func = nullptr;
            other.m_alloc_func = nullptr;
            other.m_delete_func = nullptr;
            other.m_callee = nullptr;
            return *this;
        }

        template<typename TFunctor>
        explicit Delegate (const TFunctor& functor)
        {
            *this = Create(functor);
        }

        template<typename TFunctor>
        Delegate& operator=(const TFunctor& functor)
        {
            *this = Create(functor);
            return *this;
        }

        template<TReturn (&VFunction)(TArgs...)>
        static Delegate Create()
        {
            Delegate out;
            out.m_call_func = &function_dispatch<VFunction>;
            return out;
        }

        template<typename TCallee, TReturn (TCallee::*VMethod)(TArgs...)>
        static Delegate Create(TCallee &callee)
        {
            Delegate out;
            out.m_call_func = &method_dispatch<TCallee, VMethod>;
            out.m_callee = &callee;
            return out;
        };

        template<typename TFunctor>
        static Delegate Create(const TFunctor &functor)
        {
            Delegate out;
            out.m_callee = new TFunctor(functor);
            out.m_call_func = &functor_dispatch<TFunctor>;
            out.m_alloc_func = &functor_alloc<TFunctor>;
            out.m_delete_func = &functor_delete<TFunctor>;
            std::cout << "Allocation new functor obj" << std::endl;
            return out;
        }

        TReturn operator()(TArgs... args)
        {
            (*m_call_func)(m_callee, args...);
        }

        ~Delegate()
        {
            if (m_delete_func != nullptr) (*m_delete_func)(m_callee);
        }

    private:
        typedef TReturn (*dispatch_func_ptr)(void *, TArgs...);

        typedef void *(*allocate_func_ptr)(const void *);

        typedef void (*delete_func_ptr)(void *);


        template<typename TCallee, TReturn (TCallee::*VMethod)(TArgs...)>
        static TReturn method_dispatch(void *callee, TArgs... args)
        {
            (static_cast<TCallee *>(callee)->*VMethod)(args...);
        }

        template<TReturn (&VFunction)(TArgs...)>
        static TReturn function_dispatch(void *dummy, TArgs... args)
        {
            return VFunction(args...);
        }

        template<typename TFunctor>
        static TReturn functor_dispatch(void *functor, TArgs... args)
        {
            return (*static_cast<TFunctor *>(functor))(args...);
        }

        template<typename TFunctor>
        static void *functor_alloc(const void *other)
        {
            std::cout << "Allocation new functor obj" << std::endl;
            return new TFunctor(*static_cast<const TFunctor *>(other));
        }

        template<typename TFunctor>
        static void functor_delete(void *to_delete)
        {
            std::cout << "Deallocating functor obj" << std::endl;
            delete static_cast<TFunctor *>(to_delete);
        }

        dispatch_func_ptr m_call_func;
        allocate_func_ptr m_alloc_func = nullptr;
        delete_func_ptr m_delete_func = nullptr;
        void *m_callee = nullptr;

    };
}