#pragma once
#include "../Common/Settings.h"

#include <vector>
#include <functional>

namespace rtti
{

	/// /brief context for automatic drawing propertities in inspector or other places
	///
	/// in most cases you should use @rtti::Draw function instead manually filling in context parameters
	class TypeDrawingContext
	{
	public:
		TypeDrawingContext(void* objectStart, const char* imGuiId)
			: objectStart(objectStart)
			, property(nullptr)
			, imGuiId(imGuiId)
			, fieldName("")
		{
			RE_ASSERT((objectStart, "TypeDrawingContext.objectStart can\'t be null, please assign a valid object address"));
		}

		/// adress of currently drawn object
		void* objectStart;
		
		/// property the current object is comming from, or nullptr if not applicable
		const class Property* property;
		
		/// imguiid prefix to make sure every call to imgui window will be unique
		const char* imGuiId;

		/// name of a field to be used if there is no property assigned (like in case of array entries)
		const char* fieldName;

		/// indentation before property is drawn
		int offset = 0;

		/// /brief returns property name should be used to draw property
		const char* GetDisplayName() const;
	};

	/// Type drawers are used to define how given type will be drawn
	/// Properties are one kind of a drawer automatically added to drawer list from the type
	/// Not only properties are drawn, you can also draw messages, spaces, buttons, make that some properties will be drawn in one line and more
	class TypeDrawerBase
	{
	public:
		TypeDrawerBase()
			: _category("")
			, _editorVissible(true)
		{
		}

		// returns if parameter was changed
		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall = []() {}) const = 0;

		void SetCategory(const char* category) { _category = category; }
		const char* GetCategory() const { return _category; }

		virtual bool IsEditorVissible() const { return _editorVissible; }

	protected:
		const char*			_category; 
		uint8				_editorVissible : 1;	// Does property appear in property browser?
	};

	/// /brief compound drawer is used to represent drawers that can store other drawers
	///
	/// Supports category filters
	/// TODO inheriting categories
	class CompoundTypeDrawer : public TypeDrawerBase
	{
	public:
		virtual bool DrawType(TypeDrawingContext& context, std::function<void()> afterLabelCall = [](){}) const override;

		void AddDrawer(TypeDrawerBase* drawer);

		void PrepareCategories();
		

	protected:
		std::vector<TypeDrawerBase*> _typeDrawingEntries;
		std::vector<const char*> _categories;
	};


/// Varriable modifiers
//#define RTTI_READ_ONLY					// TODO - default behaviour for const types, makes varriable read only
//#define RTTI_CATEGORY(CategoryName)		// TODO - groups params together. All futher elements are inserted to the category until changed
#define RTTI_FOLDOUT					// TODO - "folder" like behaviour, groups elements. All futher elements are inserted to the category until changed
#define RTTI_SHOW_IF(condition)			// TODO	- in what way we will write the conditions
#define RTTI_HIDE_IF(condition)			// TODO - in what way we will write the conditions
#define RTTI_ENUM_FLAGS					// TODO - only with enums, enum type will be treated as flags
#define RTTI_REQUIRED					// TODO - Reference types, shows warrning if not set up


// drawing modifier
//#define RTTI_SPACE						// TODO - Adds a drawer that leaves a space
#define RTTI_INFO_BOX					// TODO - shows box with message
#define RTTI_TEXT_BOX					// TODO - shows string as expandable text box	// do we need that?
#define RTTI_REORDERABLE_LIST			// TODO - vector shown as a list will be reorderable 

}