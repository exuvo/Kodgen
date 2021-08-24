/**
*	Copyright (c) 2021 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

#pragma once

#include "Kodgen/Misc/FundamentalTypes.h"

namespace kodgen
{
	class ICodeGenOrderable
	{
		public:
			/**
			*	@brief	The generation order is a number defining in which order a code generator will generate code compared to other code generators.
			*			Code generators with a low generation order will execute first, and code generators with a high generation order will execute last.
			*			Code generators having the same generation order will execute in an undefined order.
			*			Default generation order is 0.
			* 
			*	@return The generation order.
			*/
			virtual int32 getGenerationOrder() const noexcept;
	};
}