#pragma once
#include "rttiClassType.h"

class Asset;

namespace engine
{
	class AssetManagerBase;

	template<typename AssetT>
	class TAssetManager;

	template<typename AssetT>
	TAssetManager<AssetT>& GetAssetManager();

	class TextureAsset;
}

namespace rtti
{
	/// /brief rtti::Type for assets used in engine
	///
	/// Asset is an entity referenced by guid, often with possibility to be stored in a file
	class AssetType : public ClassType
	{
	public:
		AssetType(const char* name, uint16 typeSize)
			: ClassType(name, typeSize)
		{
		}

		/// /brief adds a file extension the asset type can be associated with
		void SetFileExtension(const wchar_t* extension)
		{
			_extension = extension;
		}

		/// /brief returns a file extension the asset type can be associated with
		const wchar_t* GetFileExtension() const
		{
			return _extension;
		}


		/// /brief adds a native extension the file asset type can be associated with
		/// 
		/// Native assets are the way 
		void AddNativeFileExtension(const wchar_t* extension)
		{
			_nativeExtensions.push_back(extension);
		}

		/// /brief checks if given extension is declared as a possible extension for the asset
		bool IsSupportedFileExtension(const wchar_t* extension)
		{
			return std::wcscmp(_extension, extension) == 0;;
		}

		/// /brief checks if given extension is declared as a possible extension for the asset
		bool IsSupportedNativeFileExtension(const wchar_t* extension)
		{
			for (auto it : _nativeExtensions)
			{
				bool sameExtension = std::wcscmp(it, extension) == 0;
				if (sameExtension)
				{
					return true;
				}
			}
			return false;
		}

		/// /brief returns asset manager of the asset
		///
		/// @note the function has to be virtual, we can not call GetAssetManager function at constructor, 
		/// we have to delay it into the future, after all reflection types have been constructed
		virtual engine::AssetManagerBase& GetAssetManager() const = 0;

	private:
		const wchar_t* _extension;
		std::vector<const wchar_t*> _nativeExtensions;
	};

	template<typename Type>
	class TAssetType : public AssetType
	{
	public:
		TAssetType(const char* name)
			: AssetType(name, sizeof(Type))
		{
			static_assert(std::is_class<Type>::value);
			SetAsAbstract(std::is_abstract<Type>::value);
		}

		virtual engine::AssetManagerBase& GetAssetManager() const override
		{
			return engine::GetAssetManager<Type>();
		}


	protected:
		virtual void* CreateNew() const override
		{
			if constexpr (std::is_abstract<Type>::value == false)
				return new Type;
			else
				return nullptr;
		}
		virtual void* DuplicateNew(const void* original) const override
		{
			if constexpr (std::is_abstract<Type>::value == false)
				return new Type(*((const Type*)original));// Call copy constructor
			else
				return nullptr;
		}
	};

	/// /brief accessor of asset types
	template<typename Type>
	inline AssetType* GetAssetDescriptor()
	{
		static_assert(std::is_base_of<Asset, Type>::value);
		RE_ASSERT((dynamic_cast<AssetType*>(GetTypeDescriptor<Type>()()) != nullptr));
		return (AssetType*)(TypeBase*)GetTypeDescriptor<Type>();
	}
}