#include "AssetManager.h"
#include <fstream>
#include <iostream>
#include "yaml-cpp/yaml.h"

namespace engine
{
	std::vector<AssetManagerBase*> AssetManagerBase::_allAssetManagers;
}