#include "Kodgen/InfoStructures/StructClassInfo.h"

#include "Kodgen/InfoStructures/NestedStructClassInfo.h"

using namespace kodgen;

StructClassInfo::StructClassInfo() noexcept:
	EntityInfo(),
	qualifiers{false},
	isForwardDeclaration{false}
{
}

StructClassInfo::StructClassInfo(CXCursor const& cursor, std::vector<Property>&& properties, EEntityType&& entityType, bool isForwardDeclaration) noexcept:
	EntityInfo(cursor, std::forward<std::vector<Property>>(properties), std::forward<EEntityType>(entityType)),
	qualifiers{false},
	isForwardDeclaration{isForwardDeclaration},
	type{clang_getCursorType(cursor)}
{
}

void StructClassInfo::refreshOuterEntity() noexcept
{
	for (std::shared_ptr<NestedStructClassInfo>& nestedClass : nestedClasses)
	{
		nestedClass->refreshOuterEntity();
		nestedClass->outerEntity = this;
	}

	for (std::shared_ptr<NestedStructClassInfo>& nestedStruct : nestedStructs)
	{
		nestedStruct->refreshOuterEntity();
		nestedStruct->outerEntity = this;
	}

	for (NestedEnumInfo& nestedEnum : nestedEnums)
	{
		nestedEnum.refreshOuterEntity();
		nestedEnum.outerEntity = this;
	}

	for (FieldInfo& field : fields)
	{
		field.outerEntity = this;
	}

	for (MethodInfo& method : methods)
	{
		method.outerEntity = this;
	}
}