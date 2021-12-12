#pragma once
#include "rttiTypeBase.h"
#include "rttiTypeDrawer.h"
#include "rttiSerialization.h"
#include <algorithm>

namespace rtti
{
	/// /brief Type for array like types
	class ArrayTypeBase : public TypeBase, public TypeDrawerBase, public TypeSerializerBase
	{
	public:
		ArrayTypeBase(const char* name, uint16 size, TypeBase* arrayElementType)
			: TypeBase(name, size)
			, _elementType(arrayElementType)
		{
			SetTypeDrawer(this);
			SetTypeSerializer(this);
		}
		
		/// /brief returns type storet in the array
		const TypeBase* GetArrayElementType() const { return _elementType; }

	protected:
		virtual void* GetArrayElementAdress(size_t id, void* arrayStart) const = 0;
		//virtual void* AddArrayElement(void* arrayStart) const = 0;
		virtual void ClearArray(void* arrayStart) const = 0;
		virtual void Resize(size_t size, void* arrayStart) const = 0;
		virtual void Swap(size_t id1, size_t id2, void* arrayStart) const = 0;
		virtual void RemoveElement(size_t id1, void* arrayStart) const = 0;
		virtual void DuplicateElement(size_t id1, void* arrayStart) const = 0;

		virtual size_t GetArraySize(void* arrayStart) const = 0;

	public: // Drawing
		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall) const override;

	public: // Serializing
		virtual void Serialize(YAML::Emitter& emitter, SerializationContext serializationContext) const override;
		virtual void Deserialize(const YAML::Node& node, SerializationContext serializationContext) const override;


	private:
		TypeBase* _elementType;
	};


	template<typename ArrayElementType>
	class TVectorArray : public ArrayTypeBase
	{
	public:
		TVectorArray(const char* name, TypeBase* elementType)
			: ArrayTypeBase(name, sizeof(std::vector<ArrayElementType>), elementType)
		{
		}

		virtual void* GetArrayElementAdress(size_t id, void* arrayStart) const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return nullptr;
			else
			{
				std::vector<ArrayElementType>* array = (std::vector<ArrayElementType>*)arrayStart;
				
				if constexpr (std::is_same<bool, ArrayElementType>::value)
				{
					return nullptr;
				}
				else
				{
					return array->data() + id;
				}
			}
		}

		/**virtual void* AddArrayElement(void* arrayStart) const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return nullptr;
			else
			{
				std::vector<ArrayElementType>* array = (std::vector<ArrayElementType>*)arrayStart;
				array->push_back(ArrayElementType{});
				auto back = array->back();
				return &back;
			}
		}*/

		virtual void Swap(size_t id1, size_t id2, void* arrayStart) const
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return;
			else
			{
				std::vector<ArrayElementType>* array = (std::vector<ArrayElementType>*)arrayStart;
				
				auto it1 = array->begin() + id1;
				auto it2 = array->begin() + id2;

				auto temp = *it1;
				*it1 = *it2;
				*it2 = temp;
			}
		}

		virtual size_t GetArraySize(void* arrayStart) const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return 0;
			else
			{
				std::vector<ArrayElementType>* array = (std::vector<ArrayElementType>*)arrayStart;
				return array->size();
			}
		}

		virtual void ClearArray(void* arrayStart) const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return;
			else
			{
				std::vector<ArrayElementType>* array = (std::vector<ArrayElementType>*)arrayStart;
				array->clear();
			}
		}

		virtual void Resize(size_t size, void* arrayStart) const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return;
			else
			{
				std::vector<ArrayElementType>* array = (std::vector<ArrayElementType>*)arrayStart;
				array->resize(size, {});
			}
		}


		virtual void RemoveElement(size_t id, void* arrayStart) const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return;
			else
			{
				std::vector<ArrayElementType>* array = (std::vector<ArrayElementType>*)arrayStart;
				array->erase(array->begin() + id);
			}
		}

		virtual void DuplicateElement(size_t id, void* arrayStart) const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return;
			else
			{
				std::vector<ArrayElementType>* array = (std::vector<ArrayElementType>*)arrayStart;
				// TODO
			}
		}


	protected:
		virtual void* CreateNew() const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return nullptr;
			else
			{
				return new std::vector<ArrayElementType>();
			}
		}

		virtual void* DuplicateNew(const void* original) const override
		{
			if constexpr (std::is_abstract<ArrayElementType>::value)
				return nullptr;
			else
			{
				return new std::vector<ArrayElementType>(*((const std::vector<ArrayElementType>*)original));
			}
		}
	};

}