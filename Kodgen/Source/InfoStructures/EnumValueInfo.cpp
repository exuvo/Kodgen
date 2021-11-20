#include "Kodgen/InfoStructures/EnumValueInfo.h"

using namespace kodgen;

EnumValueInfo::EnumValueInfo(CXCursor const& cursor, std::vector<Property>&& properties) noexcept:
	EntityInfo(cursor, std::forward<std::vector<Property>>(properties), EEntityType::EnumValue),
	value{clang_getEnumConstantDeclValue(cursor)}
{
}