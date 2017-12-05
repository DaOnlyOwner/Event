


#pragma once
#define FROM_FUNC(func_type, func_ref) (doo::Delegate< func_type >::Create< func_ref >())
#define FROM_METHOD(func_type, method_ref, obj_ref) (doo::Delegate< func_type >::Create< std::remove_pointer<decltype(&obj_ref)>::type, & std::remove_pointer<decltype(&obj_ref)>::type :: method_ref > (obj_ref))
#define FROM_FUNCTOR(func_type, functor) (doo::Delegate< func_type >::Create( functor ))

namespace doo
{

	template <typename TForward>
	class Delegate;

	/** This class is designed after the "impossible fast delegate" (https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates) class. It is generally faster than the standard std::function version because 
	it heavily relies on templates and tries to avoid heap allocations where possible*/



	template <typename TReturn, typename... TArgs>
	class Delegate<TReturn(TArgs ...)>
	{
	public:

		Delegate() = default;

		Delegate(const Delegate& p_other) : m_call_func(p_other.m_call_func), m_alloc_func(p_other.m_alloc_func),
		                                  m_delete_func(p_other.m_delete_func)
		{
			if (m_alloc_func != nullptr) m_callee = (*m_alloc_func)(p_other.m_callee);
			else m_callee = p_other.m_callee;
		}

		Delegate(Delegate&& p_other) noexcept : m_call_func(p_other.m_call_func), m_alloc_func(p_other.m_alloc_func),
		                                      m_delete_func(p_other.m_delete_func), m_callee(p_other.m_callee)
		{
			p_other.m_call_func = nullptr;
			p_other.m_delete_func = nullptr;
			p_other.m_alloc_func = nullptr;
			p_other.m_call_func = nullptr;
		}


		Delegate& operator=(const Delegate& p_other)
		{
			if (&p_other == this)
				return *this;
			m_call_func = p_other.m_call_func;
			m_alloc_func = p_other.m_alloc_func;
			m_delete_func = p_other.m_delete_func;
			if (m_alloc_func != nullptr) m_callee = (*m_alloc_func)(p_other.m_callee);
			else m_callee = p_other.m_callee;
			return *this;
		}

		Delegate& operator=(Delegate&& p_other) noexcept
		{
			if (&p_other == this)
				return *this;
			m_call_func = p_other.m_call_func;
			m_alloc_func = p_other.m_alloc_func;
			m_delete_func = p_other.m_delete_func;
			m_callee = p_other.m_callee;
			p_other.m_call_func = nullptr;
			p_other.m_alloc_func = nullptr;
			p_other.m_delete_func = nullptr;
			p_other.m_callee = nullptr;
			return *this;
		}

		template <typename TFunctor>
		Delegate(const TFunctor& p_functor)
		{
			*this = Create(p_functor);
		}

		template <typename TFunctor>
		Delegate& operator=(const TFunctor& p_functor)
		{
			*this = Create(p_functor);
			return *this;
		}

		template <TReturn (&VFunction)(TArgs ...)>
		static Delegate Create()
		{
			Delegate out;
			out.m_call_func = &functionDispatch<VFunction>;
			return out;
		}

		template <typename TCallee, TReturn (TCallee::*VMethod)(TArgs ...)>
		static Delegate Create(TCallee& p_callee)
		{
			Delegate out;
			out.m_call_func = &methodDispatch<TCallee, VMethod>;
			out.m_callee = &p_callee;
			return out;
		};

		template <typename TFunctor>
		static Delegate Create(const TFunctor& p_functor)
		{
			Delegate out;
			out.m_callee = new TFunctor(p_functor);
			out.m_call_func = &functorDispatch<TFunctor>;
			out.m_alloc_func = &functorAlloc<TFunctor>;
			out.m_delete_func = &functorDelete<TFunctor>;
			return out;
		}

		TReturn operator()(TArgs ... p_args) const
		{
			return (*m_call_func)(m_callee, p_args...);
		}

		~Delegate()
		{
			if (m_delete_func != nullptr) (*m_delete_func)(m_callee);
		}

	private:
		typedef TReturn (*dispatch_func_ptr)(void*, TArgs ...);

		typedef void*(*allocate_func_ptr)(const void*);

		typedef void (*delete_func_ptr)(void*);


		template <typename TCallee, TReturn (TCallee::*VMethod)(TArgs ...)>
		static TReturn methodDispatch(void* p_callee, TArgs ... p_args)
		{
			return (static_cast<TCallee *>(p_callee) ->* VMethod)(p_args...);
		}

		template <TReturn (&VFunction)(TArgs ...)>
		static TReturn functionDispatch(void*, TArgs ... p_args)
		{
			return VFunction(p_args...);
		}

		template <typename TFunctor>
		static TReturn functorDispatch(void* p_functor, TArgs ... p_args)
		{
			return (*static_cast<TFunctor *>(p_functor))(p_args...);
		}

		template <typename TFunctor>
		static void* functorAlloc(const void* p_other)
		{
			return new TFunctor(*static_cast<const TFunctor *>(p_other));
		}

		template <typename TFunctor>
		static void functorDelete(void* p_toDelete)
		{
			delete static_cast<TFunctor *>(p_toDelete);
		}

	private:
		dispatch_func_ptr m_call_func;
		allocate_func_ptr m_alloc_func = nullptr;
		delete_func_ptr m_delete_func = nullptr;
		void* m_callee = nullptr;
	};
}
