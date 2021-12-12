#pragma once
#include "rttiTypeBase.h"
#include <initializer_list>

/// TODO move to some utility template file
template<typename Target, typename ListHead, typename... ListTails>
inline constexpr size_t GetTypeIndexInTemplateList()
{
	if constexpr (std::is_same<Target, ListHead>::value)
		return 0;
	else
		return 1 + GetTypeIndexInTemplateList<Target, ListTails...>();
}

namespace rtti
{
	/// /brief Context for rtti functions calls
	struct FunctionCallingContext
	{
		/// object adress for member functions
		/// if this is not a member function you can left it nullptr
		void* object = nullptr;

		/// object adress of the result expression value
		/// if function does not return anything you can left it nullptr
		void* result = nullptr;

		/// array of argument pointers
		std::vector<void*> argumentPointers;
	};

	/// /brief base type for rtti functions
	class FunctionTypeBase
	{
	public:
		FunctionTypeBase(const char* functionName)
			: _nativeName(functionName)
			, _displayName(functionName)
			, _tooltip("")
		{

		}

		/// /brief invokes function,
		///
		/// provided context should contain data corresponding to stored function otherwise will assert
		virtual void Invoke(FunctionCallingContext& context) const = 0;

		/// /brief returns type the function returns. 
		///
		/// should never return nullptr
		virtual const TypeBase* GetResultType() const = 0;

		/// /brief returns idth argument type 
		///
		/// should never return nullptr
		virtual const TypeBase* GetArgumentType(size_t id) const = 0;

		/// /brief returns how many arguments function requires
		virtual size_t GetArgumentCount() const = 0;

		/// /brief returns a real name of the function
		///
		/// note can contain namespace too
		const char* GetNativeName() const { return _nativeName; }

		/// /brief returns name the function is displayed in editor with
		///
		/// by default display name is equal to native name
		const char* GetDisplayName() const { return _displayName; }

		/// /brief sets name the function is displayed in editor with
		///
		/// by default display name is equal to native name
		FunctionTypeBase& SetDisplayName(const char* displayName) { _displayName = displayName; return *this; }

		/// /brief returns tooltip for the function
		const char* GetTooltip() const { return _tooltip; }

		/// /brief sets tooltip for the function
		FunctionTypeBase& SetTooltip(const char* tooltip) { _tooltip = tooltip; return *this; }

	private:
		const char* _nativeName;
		const char* _displayName;

		const char* _tooltip;
	};

	template<typename Result, typename... Arguments >
	class TFunctionType : public FunctionTypeBase
	{
	public:
		TFunctionType(const char* functionName, Result(*ptr)(Arguments...))
			: FunctionTypeBase(functionName)
			, _ptr(ptr)
			, _resultType(GetTypeDescriptor<Result>())
			, _argumentTypes({ (GetTypeDescriptor<Arguments>())... }) // iteration through initializer list
		{
		}

		virtual void Invoke(FunctionCallingContext& context) const override
		{
			if constexpr (std::is_same<void, Result>::value)
			{
				std::invoke(_ptr, (*((Arguments*)context.argumentPointers[GetTypeIndexInTemplateList < Arguments, Arguments...>()]))...);
			}
			else
			{
				if (context.result)
				{
					*((Result*)context.result) = std::invoke(_ptr, (*((Arguments*)context.argumentPointers[GetTypeIndexInTemplateList < Arguments, Arguments...>()]))...);
				}
				else
				{
					std::invoke(_ptr, (*((Arguments*)context.argumentPointers[GetTypeIndexInTemplateList < Arguments, Arguments...>()]))...);
				}
			}
		}

		Result Invoke(Arguments... arguments)
		{
			return _ptr(arguments...);
		}

		virtual const TypeBase* GetResultType() const override { return _resultType; }
		virtual const TypeBase* GetArgumentType(size_t id) const override { return _argumentTypes[id]; }
		virtual size_t GetArgumentCount() const override { return _argumentTypes.size(); }


	private:
		Result(*_ptr)(Arguments...);
		TypeBase* _resultType;
		std::array<TypeBase*, sizeof...(Arguments)> _argumentTypes;
	};

	template<typename ClassType, typename Result, typename... Arguments >
	class TMemberFunctionType : public FunctionTypeBase
	{
	public:
		TMemberFunctionType(const char* functionName, Result(ClassType::* ptr)(Arguments...))
			: FunctionTypeBase(functionName)
			, _ptr((Result(ClassType::*)(Arguments...) const)ptr)
			, _resultType(GetTypeDescriptor<Result>())
			, _argumentTypes({ (GetTypeDescriptor<Arguments>())... }) // iteration through initializer list
		{
		}

		TMemberFunctionType(const char* functionName, Result(ClassType::* ptr)(Arguments...) const)
			: FunctionTypeBase(functionName)
			, _ptr(ptr)
			, _resultType(GetTypeDescriptor<Result>())
			, _argumentTypes({ (GetTypeDescriptor<Arguments>())... }) // iteration through initializer list
		{
		}

		virtual void Invoke(FunctionCallingContext& context) const override
		{
			CallableInvoke callable;
			callable.object = (ClassType*)context.object;
			callable.ptr = _ptr;

			if constexpr (std::is_same<void, Result>::value)
			{
				std::invoke(callable, (*((Arguments*)context.argumentPointers[GetTypeIndexInTemplateList < Arguments, Arguments...>()]))...);
			}
			else
			{
				if (context.result)
				{
					*((Result*)context.result) = std::invoke(callable, (*((Arguments*)context.argumentPointers[GetTypeIndexInTemplateList < Arguments, Arguments...>()]))...);
				}
				else
				{
					std::invoke(callable, (*((Arguments*)context.argumentPointers[GetTypeIndexInTemplateList < Arguments, Arguments...>()]))...);
				}
			}
		}

		Result Invoke(ClassType& object, Arguments... arguments)
		{
			return (object.*_ptr)(arguments...);
		}

		virtual const TypeBase* GetResultType() const override { return _resultType; }
		virtual const TypeBase* GetArgumentType(size_t id) const override { return _argumentTypes[id]; }
		virtual size_t GetArgumentCount() const override { return _argumentTypes.size(); }

	private:
		struct CallableInvoke
		{
			ClassType* object;
			Result(ClassType::* ptr)(Arguments...) const;
			Result operator() (Arguments... args) const noexcept
			{
				return (object->*ptr)(args...);
			}
		};


	private:
		Result(ClassType::* _ptr)(Arguments...) const;
		TypeBase* _resultType;
		std::array<TypeBase*, sizeof...(Arguments)> _argumentTypes;
	};

}