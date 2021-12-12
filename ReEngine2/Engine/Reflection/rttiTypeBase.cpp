#include "rttiTypeBase.h"
#include "rttiSerialization.h"
#include <fstream>

namespace rtti
{
	TypeBase::TypeBase(const char* name, uint16 size)
		: _displayName(name)
		, _nativeName(name)
		, _typeDrawer(nullptr)
		, _typeSerializer(nullptr)
		, _size(size)
		, _tooltip("")
		, _defaultSetter(nullptr)
		, _defaultGetter(nullptr)
		, _isTypeSerializerAReference(false)
		, _isTypeDrawerAReference(false)
	{
	}
	TypeBase::~TypeBase()
	{
		if (_typeDrawer && !_isTypeDrawerAReference)
			delete _typeDrawer;

		if (_typeSerializer && !_isTypeSerializerAReference)
			delete _typeSerializer;
	}
	void TypeBase::FinalizeInitialization()
	{
	}
	bool TypeBase::SerializeToFile(const std::filesystem::path& filePath, const SerializationContext& context)
	{
		std::ofstream file;
		file.open(filePath);

		if (file)
		{
			YAML::Emitter emitter;
			_typeSerializer->Serialize(emitter, context);
			//Serialize(file, context);

			file << emitter.c_str();
			file.close();
			return true;
		}


		// log error
		std::cerr << "Falied to open the file: \"" << filePath << "\" to save\n";
		std::cerr << "badbit " << file.badbit << ", failbit" << file.failbit << ", eof" << file.eofbit;

		return false;
	}

	bool TypeBase::DeserializeFromFile(const std::filesystem::path& filePath, const SerializationContext& context) const
	{
		YAML::Node node = YAML::LoadFile(std::move(filePath.string()));

		if (node)
		{
			_typeSerializer->Deserialize(node, context);
			
			return true;
		}

		return false;
	}

	bool TypeBase::SerializeToString(std::string& s, const SerializationContext& context)
	{
		YAML::Emitter emitter;
		_typeSerializer->Serialize(emitter, context);
		
		s =  emitter.c_str();
		return true;
	}

	bool TypeBase::DeserializeFromString(const std::string_view& filePath, const SerializationContext& context) const
	{
		YAML::Node node = YAML::Load(std::string(filePath).c_str());

		if (node)
		{
			_typeSerializer->Deserialize(node, context);

			return true;
		}

		return false;
	}

}