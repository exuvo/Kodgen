/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

#pragma once

#include <clang-c/Index.h>

#include "Kodgen/InfoStructures/EnumInfo.h"
#include "Kodgen/Misc/EAccessSpecifier.h"

namespace kodgen
{
	class NestedEnumInfo : public EnumInfo
	{
		public:
			static constexpr EEntityType	nestedEntityTypes = EEntityType::EnumValue;

			/** Access specifier of this nested struct/class. */
			EAccessSpecifier				accessSpecifier;

			NestedEnumInfo(EnumInfo&&		base,
						   EAccessSpecifier accessSpecifier)	noexcept;
	};
}