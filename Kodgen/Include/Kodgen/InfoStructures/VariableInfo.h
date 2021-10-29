/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the LICENSE.md file for full license details.
*/

#pragma once

#include <clang-c/Index.h>

#include "Kodgen/InfoStructures/EntityInfo.h"
#include "Kodgen/InfoStructures/TypeInfo.h"

namespace kodgen
{
	class VariableInfo : public EntityInfo
	{
		protected:
			VariableInfo(CXCursor const&			cursor,
						 std::vector<Property>&&	properties,
						 EEntityType				entityType)		noexcept;

		public:
			/** Is this variable static or not. */
			bool isStatic : 1;

			/** Type of this variable. */
			TypeInfo			type;

			VariableInfo(CXCursor const&			cursor,
						 std::vector<Property>&&	properties)	noexcept;
	};
}