#pragma once
#include "../../Imgui/imgui.h"
#include "../../Imgui/imgui-SFML.h"
#include "rttiTypeDrawer.h"
#include "rttiProperty.h"

// TODO move to editor

namespace editor
{
	inline const char* GetIdWithLabel(const char* label, const char* imGuiId)
	{
		static std::string _buffor;
		_buffor = label;
		_buffor += "##";
		_buffor += imGuiId;
		return _buffor.c_str();
	}

	// TODO change name
	constexpr float OFFSET_VALUE = 30.0f;

	template<typename Type>
	inline bool DrawFloat(const char* label, const char* imGuiId, Type* varriable, float offset, float dragSpeed, std::function<void()> afterLabelCall)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::Text(label);
		afterLabelCall();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::SameLine();

		float v = *varriable;
		if (ImGui::DragFloat(GetIdWithLabel("", imGuiId), &v, dragSpeed))
		{
			*varriable = v;
			return true;
		}
		return false;
	}

	template<typename Type>
	inline bool DrawVector2Float(const char* label, const char* imGuiId, Type* varriable, float offset, float dragSpeed, std::function<void()> afterLabelCall)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::Text(label); 
		afterLabelCall();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::SameLine();
		float buffer[2]{ varriable->x, varriable->y };
		if (ImGui::DragFloat2(GetIdWithLabel("", imGuiId), buffer, dragSpeed))
		{
			varriable->x = buffer[0];
			varriable->y = buffer[1];
			return true;
		}
		return false;
	}
	template<typename Type>
	inline bool DrawVector2Int(const char* label, const char* imGuiId, Type* varriable, float offset, float dragSpeed, std::function<void()> afterLabelCall)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::Text(label);
		afterLabelCall();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::SameLine();
		int buffer[2]{ varriable->x, varriable->y };
		if (ImGui::DragInt2(GetIdWithLabel("", imGuiId), buffer, dragSpeed))
		{
			varriable->x = buffer[0];
			varriable->y = buffer[1];
			return true;
		}
		return false;
	}

	template<typename Type>
	inline bool DrawInt(const char* label, const char* imGuiId, Type* varriable, float offset, float dragSpeed, std::function<void()> afterLabelCall)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::Text(label);
		afterLabelCall();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::SameLine();

		int v = *varriable;
		if (ImGui::DragInt(GetIdWithLabel("", imGuiId), &v, dragSpeed))
		{
			*varriable = v;
			return true;
		}
		return false;
	}

	inline bool DrawBool(const char* label, const char* imGuiId, bool* varriable, float offset, std::function<void()> afterLabelCall)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::Text(label);
		afterLabelCall();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::SameLine();
		return ImGui::Checkbox(GetIdWithLabel("", imGuiId), varriable);
	}

	inline bool DrawString(const char* label, const char* imGuiId, std::string* varriable, float offset, std::function<void()> afterLabelCall)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::Text(label);
		afterLabelCall();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::SameLine();

		char buffor[50];
		memcpy(buffor, varriable->c_str(), sizeof(varriable->c_str()));
		bool r = ImGui::InputText(GetIdWithLabel("", imGuiId), buffor, sizeof(buffor));
		*varriable = buffor;
		return r;
	}

	inline bool DrawColor(const char* label, const char* imGuiId, sf::Color* varriable, float offset, std::function<void()> afterLabelCall)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::Text(label);
		afterLabelCall();

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset * OFFSET_VALUE);
		ImGui::SameLine();

		float color[4]
		{
			(float)varriable->r / 255.0f,
			(float)varriable->g / 255.0f,
			(float)varriable->b / 255.0f,
			(float)varriable->a / 255.0f,
		};
		bool modified = ImGui::ColorEdit4(GetIdWithLabel("", imGuiId), color);
		if (modified)
		{
			varriable->r = (sf::Uint8)(color[0] * 255.0f);
			varriable->g = (sf::Uint8)(color[1] * 255.0f);
			varriable->b = (sf::Uint8)(color[2] * 255.0f);
			varriable->a = (sf::Uint8)(color[3] * 255.0f);
		}

		return modified;
	}

	class VoidDrawer : public rtti::TypeDrawerBase
	{
	public:
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			return false;
		}
	};

	template<typename Type>
	class FloatDrawer : public rtti::TypeDrawerBase
	{
	public:
		FloatDrawer(float dragSpeed = 0.1f)
			:dragSpeed(dragSpeed)
		{

		}
		float dragSpeed;
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			Type varriable = *((Type*)context.objectStart);
			if (context.property != nullptr && context.property->GetOverridePropertyGetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.result = &varriable;
				context.property->GetOverridePropertyGetter()->Invoke(functionContext);
			}

			bool changed = DrawFloat<Type>(context.GetDisplayName(), context.imGuiId, &varriable, (float)context.offset, dragSpeed, afterLabelCall);
			if (changed)
			{
				//editor::editor->redoUndoManager.Do<editor::undoRedo::VarriableOperation<Type>>(varriable);
			}


			if (context.property != nullptr && context.property->GetOverridePropertySetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.argumentPointers.push_back(&varriable);
				context.property->GetOverridePropertySetter()->Invoke(functionContext);
			}
			else
			{
				*((Type*)context.objectStart) = varriable;
			}

			return changed;
		}
	};

	template<typename Type>
	class IntDrawer : public rtti::TypeDrawerBase
	{
	public:
		IntDrawer(float dragSpeed = 1.0f)
			:dragSpeed(dragSpeed)
		{

		}
		float dragSpeed;
		
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			Type varriable = *((Type*)context.objectStart);
			if (context.property != nullptr && context.property->GetOverridePropertyGetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.result = &varriable;
				context.property->GetOverridePropertyGetter()->Invoke(functionContext);
			}
			bool changed = DrawInt<Type>(context.GetDisplayName(), context.imGuiId, &varriable, (float)context.offset, dragSpeed, afterLabelCall);
			if (changed)
			{
				//editor::editor->redoUndoManager.Do<editor::undoRedo::VarriableOperation<Type>>(varriable);
			}

			if (context.property != nullptr && context.property->GetOverridePropertySetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.argumentPointers.push_back(&varriable);
				context.property->GetOverridePropertySetter()->Invoke(functionContext);
			}
			else
			{
				*((Type*)context.objectStart) = varriable;
			}
			return changed;
		}
	};

	class BoolDrawer : public rtti::TypeDrawerBase
	{
	public:
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			bool varriable = *((bool*)context.objectStart);
			if (context.property != nullptr && context.property->GetOverridePropertyGetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.result = &varriable;
				context.property->GetOverridePropertyGetter()->Invoke(functionContext);
			}
			bool changed = DrawBool(context.GetDisplayName(), context.imGuiId, &varriable, (float)context.offset, afterLabelCall);

			if (changed)
			{
				
				//editor::editor->redoUndoManager.Do<editor::undoRedo::VarriableOperation<bool>>(varriable);

				if (context.property != nullptr && context.property->GetOverridePropertySetter())
				{
					rtti::FunctionCallingContext functionContext;
					functionContext.object = context.property->GetOwnerAddres(context.objectStart);
					functionContext.argumentPointers.push_back(&varriable);
					context.property->GetOverridePropertySetter()->Invoke(functionContext);
				}
				else
				{
					*((bool*)context.objectStart) = varriable;
				}
			}

			return changed;
		}
	};

	// TODO do something about that allocation
	template<typename StringType>
	class StringDrawer : public rtti::TypeDrawerBase
	{
	public:
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			std::string varriable = *((StringType*)context.objectStart);
			if (context.property != nullptr && context.property->GetOverridePropertyGetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.result = &varriable;
				context.property->GetOverridePropertyGetter()->Invoke(functionContext);
			}

			bool changed = DrawString(context.GetDisplayName(), context.imGuiId, &varriable, (float)context.offset, afterLabelCall);
			
			if (changed)
			{
				if (context.property != nullptr && context.property->GetOverridePropertySetter())
				{
					rtti::FunctionCallingContext functionContext;
					functionContext.object = context.property->GetOwnerAddres(context.objectStart);
					functionContext.argumentPointers.push_back(&varriable);
					context.property->GetOverridePropertySetter()->Invoke(functionContext);
				}
				else
				{
					*((StringType*)context.objectStart) = varriable;
				}
			}

			return changed;
		}
	};

	template<typename Type>
	class Vector2FDrawer : public rtti::TypeDrawerBase
	{
	public:
		Vector2FDrawer(float dragSpeed = 0.1f)
			:dragSpeed(dragSpeed)
		{

		}
		float dragSpeed;
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			Type varriable = *((Type*)context.objectStart);
			if (context.property != nullptr && context.property->GetOverridePropertyGetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.result = &varriable;
				context.property->GetOverridePropertyGetter()->Invoke(functionContext);
			}

			bool changed = DrawVector2Float(context.GetDisplayName(), context.imGuiId, &varriable, (float)context.offset, dragSpeed, afterLabelCall);

			if (changed)
			{
				if (context.property != nullptr && context.property->GetOverridePropertySetter())
				{
					rtti::FunctionCallingContext functionContext;
					functionContext.object = context.property->GetOwnerAddres(context.objectStart);
					functionContext.argumentPointers.push_back(&varriable);
					context.property->GetOverridePropertySetter()->Invoke(functionContext);
				}
				else
				{
					*((Type*)context.objectStart) = varriable;
				}
			}
			return changed;
		}
	};

	template<typename Type>
	class Vector2IDrawer : public rtti::TypeDrawerBase
	{
	public:
		Vector2IDrawer(float dragSpeed = 0.1f)
			:dragSpeed(dragSpeed)
		{

		}
		float dragSpeed;
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			Type varriable = *((Type*)context.objectStart);
			if (context.property != nullptr && context.property->GetOverridePropertyGetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.result = &varriable;
				context.property->GetOverridePropertyGetter()->Invoke(functionContext);
			}

			bool changed = DrawVector2Int(context.GetDisplayName(), context.imGuiId, &varriable, (float)context.offset, dragSpeed, afterLabelCall);

			if (changed)
			{
				if (context.property != nullptr && context.property->GetOverridePropertySetter())
				{
					rtti::FunctionCallingContext functionContext;
					functionContext.object = context.property->GetOwnerAddres(context.objectStart);
					functionContext.argumentPointers.push_back(&varriable);
					context.property->GetOverridePropertySetter()->Invoke(functionContext);
				}
				else
				{
					*((Type*)context.objectStart) = varriable;
				}
			}
			return changed;
		}
	};

	class ColorDrawer : public rtti::TypeDrawerBase
	{
	public:
		virtual bool DrawType(rtti::TypeDrawingContext& context, std::function<void()> afterLabelCall) const override
		{
			sf::Color varriable = *((sf::Color*)context.objectStart);
			if (context.property != nullptr && context.property->GetOverridePropertyGetter())
			{
				rtti::FunctionCallingContext functionContext;
				functionContext.object = context.property->GetOwnerAddres(context.objectStart);
				functionContext.result = &varriable;
				context.property->GetOverridePropertyGetter()->Invoke(functionContext);
			}

			bool changed = DrawColor(context.GetDisplayName(), context.imGuiId, &varriable, (float)context.offset, afterLabelCall);
			
			if (changed)
			{
				if (context.property != nullptr && context.property->GetOverridePropertySetter())
				{
					rtti::FunctionCallingContext functionContext;
					functionContext.object = context.property->GetOwnerAddres(context.objectStart);
					functionContext.argumentPointers.push_back(&varriable);
					context.property->GetOverridePropertySetter()->Invoke(functionContext);
				}
				else
				{
					*((sf::Color*)context.objectStart) = varriable;
				}
			}

			return changed;
		}
	};

}

