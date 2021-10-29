#include "Kodgen/InfoStructures/EnumValueInfo.h"

using namespace kodgen;

EnumValueInfo::EnumValueInfo(CXCursor const& cursor) noexcept:
	EntityInfo(cursor, std::vector<Property>(), EEntityType::EnumValue),
	value{clang_getEnumConstantDeclValue(cursor)}
{
}