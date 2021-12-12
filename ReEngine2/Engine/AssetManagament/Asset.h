#pragma once
#include "../Common/Guid.h"
#include "../Reflection/Reflection.h"

namespace editor
{
	class EditorContext;
	class ContentBrowserAssetDrawingSettings;
}

// Any file that could appear in the content browser?
class Asset
{
	RTTI_DECLARE_ASSET(Asset);
public:

	virtual void ShowInInspector(editor::EditorContext& editorContext) 
	{
		rtti::DrawByType(GetType(), this, GetName().c_str());
	}
	
	// /returns was context menu opened?
	virtual bool DrawContentBrowserElement(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath, const editor::ContentBrowserAssetDrawingSettings& assetDrawingContext);
	virtual void ContextMenuContentBrowser(std::weak_ptr<Asset> _this, const std::filesystem::path& currentPath);

	virtual void CreateDragPayload() const {} // = 0;
	
	Guid GetGuid() const
	{
		return _guid;
	}
	void SetGuid(Guid guid)
	{
		_guid = guid;
	}

	const std::string& GetName() const { return _name; }
	void SetName(const std::string& name) { _name = name; }

protected:
	void CreateDefaultDragPayload(const char* assetPayloadCode) const;

private:
	Guid _guid;
	std::string _name;
};


class NativeAsset : public Asset
{
	RTTI_DECLARE_ASSET(NativeAsset);
public:

	virtual void DeserializeNative(const std::filesystem::path& path)
	{

	}

	void SetNativeRelativePath(const std::filesystem::path& path)
	{
		_nativeRelativePath = path.string();
	}
	std::filesystem::path GetNativeRelativePath() const
	{
		return std::filesystem::path(_nativeRelativePath);
	}

private:
	std::string _nativeRelativePath; //< relative(from asset file) path to native asset 
};

#include "AssetReference.h"