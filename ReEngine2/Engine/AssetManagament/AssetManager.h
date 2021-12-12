#pragma once
#include <Engine/Common/Settings.h>
#include <Engine/Common/Guid.h>
#include <Engine/Reflection/Reflection.h>

#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <iostream>

class Asset;
class NativeAsset;

namespace engine
{
	class AssetManagerBase
	{
		static std::vector<AssetManagerBase*> _allAssetManagers;
	public:
		static std::vector<AssetManagerBase*>::iterator GetAllAssetManagersBegin()	{ return _allAssetManagers.begin(); }
		static std::vector<AssetManagerBase*>::iterator GetAllAssetManagersEnd()	{ return _allAssetManagers.end(); }

	public:
		AssetManagerBase(rtti::AssetType* assetType)
			: _assetDirectory("Assets")
			, _assetType(assetType)
		{
			_allAssetManagers.push_back(this);
		}
		
		rtti::AssetType* GetAssetType() const { return _assetType; }

		virtual std::weak_ptr<Asset> FindAssetBaseByGuid(Guid id) = 0;

		virtual void AddAssetFromFile(const std::filesystem::path& path) = 0;
		virtual void CreateAssetFileFromNative(const std::filesystem::path& nativeAssetPath) = 0;

		// TODO make it read multiple parameters
		Guid ReadGuidFromMetaFile(const std::filesystem::path& path) const
		{

			auto a = YAML::LoadAllFromFile(path.string());
			if (a.size() > 0)
			{
				if (a[0]["_guid"].IsDefined())
				{
					auto g = a[0]["_guid"].as<uint64>();

					return Guid(g);
				}
			}
			RE_ASSERT(false);
			// TODO error checking
			return Guid(0);// YAML::LoadAllFromFile(path.string())[0]["guid"].as<uint64>());
		}


		void ScanForTextures()
		{
			for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(_assetDirectory))
			{
				const auto& path = directoryEntry.path();

				if (GetAssetType()->IsSupportedNativeFileExtension(path.extension().c_str()))
				{
					bool isAssetAlreadyCreated = std::filesystem::exists(std::filesystem::path(path.string() + GetAssetType()->GetFileExtension()));;
					if (!isAssetAlreadyCreated)
					{
						// create asset an file bassed on the extension
						CreateAssetFileFromNative(path);
					}
				}
				else if (GetAssetType()->IsSupportedFileExtension(path.extension().c_str()))
				{
					auto guid = ReadGuidFromMetaFile(path);

					auto asset = FindAssetBaseByGuid(guid);

					if (asset.expired() == true)
					{
						// load the file into asset manager
						AddAssetFromFile(path);
					}
				}
			}
		}

	protected:
		std::unordered_map<std::string, Guid> _guidMap;
		std::filesystem::path _assetDirectory;
		rtti::AssetType* _assetType;

		bool IsMetaExtension(const std::string& extension) const
		{
			return extension == ".meta";
		}

		void CreateMetaFile(const std::filesystem::path& path, Guid guid)
		{
			std::ofstream ofs(path.string() + ".meta");


			YAML::Emitter emitter;
			emitter << YAML::BeginMap;
			emitter << YAML::Key << "guid" << YAML::Value << (uint64)guid;
			emitter << YAML::EndMap;

			ofs << emitter.c_str();
			ofs.close();
		}
	};

	template<typename AssetT>
	class TAssetManager;

	template<typename AssetT>
	TAssetManager<AssetT>& GetAssetManager();

	template<typename AssetT>
	class TAssetManager : public AssetManagerBase
	{
	public:

		std::shared_ptr<AssetT> FindAssetByGuid(Guid id)
		{
			return _assets[id];
		}
		virtual std::weak_ptr<Asset> FindAssetBaseByGuid(Guid id) override
		{
			return std::weak_ptr<Asset>(_assets[id]);
		}

		virtual void AddAssetFromFile(const std::filesystem::path& path) override
		{
			std::shared_ptr<AssetT> asset = std::make_shared<AssetT>();

			rtti::DeserializeFromFile(path, *asset);
			if constexpr (std::is_base_of<NativeAsset, AssetT>::value)
			{
				asset->DeserializeNative(path);
			}

			_assets[asset->GetGuid()] = asset;
		}

		virtual void CreateAssetFileFromNative(const std::filesystem::path& nativeAssetPath) override
		{
			if constexpr (std::is_base_of<class NativeAsset, AssetT>::value)
			{
				std::shared_ptr<AssetT> asset = std::make_shared<AssetT>();

				asset->SetNativeRelativePath(nativeAssetPath.filename().string());
				asset->SetName(nativeAssetPath.filename().string());

				asset->DeserializeNative(nativeAssetPath);

				std::filesystem::path path(nativeAssetPath.string() + rtti::GetAssetDescriptor<AssetT>()->GetFileExtension());
				rtti::SerializeToFile(path, *asset);
				_assets[asset->GetGuid()] = asset;
			}
			else
			{ 
				RE_ASSERT(false);
			}
		}

		template<class ...Args>
		std::shared_ptr<AssetT> CreateAsset(Args... args)
		{
			std::shared_ptr<AssetT> shared = std::make_shared<AssetT>(args...);
			_assets[shared->GetGuid()] = shared;
			return shared;
		}

		std::shared_ptr<AssetT> InsertAsset(AssetT* asset)
		{
			std::shared_ptr<AssetT> shared = std::shared_ptr<AssetT>(asset);
			_assets[shared->GetGuid()] = shared;
			return shared;
		}

		void RemoveAsset(Guid guid)
		{
			_assets.erase(guid);
		}


		auto begin() { return _assets.begin(); }
		auto end() { return _assets.end(); }
		auto cbegin() const { return _assets.begin(); }
		auto cend() const { return _assets.end(); }
		auto begin() const { return _assets.begin(); }
		auto end() const { return _assets.end(); }

	private:
		TAssetManager()
			: AssetManagerBase(rtti::GetAssetDescriptor<AssetT>())
		{
			static_assert(std::is_abstract<AssetT>::value == false);

			// for tests
			ScanForTextures();
		}

		std::unordered_map<Guid, std::shared_ptr<AssetT>> _assets;

		friend TAssetManager<AssetT>& GetAssetManager<AssetT>();
	};

	template<>
	class TAssetManager<Asset> : public AssetManagerBase
	{
		virtual std::weak_ptr<Asset> FindAssetBaseByGuid(Guid id)
		{
			return std::weak_ptr<Asset>();
		}

		virtual void AddAssetFromFile(const std::filesystem::path& path)
		{
			RE_ASSERT(false);
		}
		virtual void CreateAssetFileFromNative(const std::filesystem::path& nativeAssetPath)
		{
			RE_ASSERT(false);
		}

	private:
		TAssetManager()
			: AssetManagerBase(nullptr)
		{

		}
		friend TAssetManager<Asset>& GetAssetManager<Asset>();
	};

	template<>
	class TAssetManager<NativeAsset> : public AssetManagerBase
	{
		virtual std::weak_ptr<Asset> FindAssetBaseByGuid(Guid id)
		{
			return std::weak_ptr<Asset>();
		}

		virtual void AddAssetFromFile(const std::filesystem::path& path)
		{
			RE_ASSERT(false);
		}
		virtual void CreateAssetFileFromNative(const std::filesystem::path& nativeAssetPath)
		{
			RE_ASSERT(false);
		}

	private:
		TAssetManager()
			: AssetManagerBase(nullptr)
		{

		}

		friend TAssetManager<NativeAsset>& GetAssetManager<NativeAsset>();
	};

	template<typename AssetT>
	TAssetManager<AssetT>& GetAssetManager()
	{
		static TAssetManager<AssetT> instance;
		return instance;
	}
}