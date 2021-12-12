#pragma once
#include "../../Common/Settings.h"
#include <functional>
#include <vector>
#include <stack>

namespace editor
{
namespace undoRedo
{
	class OperationBase
	{
	public:
		virtual void Undo() = 0;
		virtual void Redo() = 0;
	};

	class Manager
	{
	public:
		template<typename Operation, typename... Args>
		void Do(Args... args)
		{
			_operationStack.resize(_currentOperationId);
			++_currentOperationId;
			_operationStack.push_back(std::move(std::unique_ptr<OperationBase>(new Operation(args...))));
		}

		void Undo()
		{
			_operationStack[_currentOperationId - 1]->Undo();
			--_currentOperationId;
		}

		void Redo()
		{
			++_currentOperationId;
			_operationStack[_currentOperationId - 1]->Redo();
		}

	private:
		std::vector<std::unique_ptr<OperationBase>> _operationStack;
		size_t _currentOperationId = 0;
	};

	template<typename Ty>
	class VarriableOperation : public OperationBase
	{
	public:
		VarriableOperation(Ty* varriable)
			: _varriable(varriable)
			, _valueAtTheTime(*_varriable)
			, _oldValue(0) // unimportant value
		{
		}

		virtual void Undo() override
		{
			_oldValue = *_varriable;
			*_varriable = _valueAtTheTime;
		}

		virtual void Redo() override
		{
			*_varriable = _oldValue;
		}

	private:
		Ty* _varriable;
		Ty _valueAtTheTime;
		Ty _oldValue;
	};

	// varriables
	//		* in inspector
	//		* modifying transform
	//		* 
	// deletion of entity
	// duplicating entity
	// reordering entities
	// reordering array elements
	// removing asset(?)
}
}