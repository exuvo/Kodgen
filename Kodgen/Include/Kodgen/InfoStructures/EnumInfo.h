/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

#pragma once

#include <vector>

#include <clang-c/Index.h>

#include "Kodgen/InfoStructures/TypeInfo.h"
#include "Kodgen/InfoStructures/EntityInfo.h"
#include "Kodgen/InfoStructures/EnumValueInfo.h"

namespace kodgen
{
	class EnumInfo : public EntityInfo
	{
		public:
			static constexpr EEntityType	nestedEntityTypes = EEntityType::EnumValue;

			/** Detailed information on the enum. */
			TypeInfo						type;

			/** Detailed information on the underlying enum type. */
			TypeInfo						underlyingType;

			/** List of all values contained in the enum. */
			std::vector<EnumValueInfo>		enumValues;

			EnumInfo()										= default;
			EnumInfo(CXCursor const&			cursor,
					 std::vector<Property>&&	properties)	noexcept;

			/**
			*	@brief Refresh the outerEntity field of all nested entities. Internal use only.
			*/
			void	refreshOuterEntity()	noexcept;
	};
}