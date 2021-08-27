/**
*	Copyright (c) 2021 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

#pragma once

#include <string>
#include <functional>	//std::function

#include "Kodgen/CodeGen/ETraversalBehaviour.h"
#include "Kodgen/Misc/FundamentalTypes.h"

namespace kodgen
{
	//Forward declaration
	class EntityInfo;
	class CodeGenEnv;

	class ICodeGenerator
	{
		public:
			ICodeGenerator()						= default;
			ICodeGenerator(ICodeGenerator const&)	= default;
			ICodeGenerator(ICodeGenerator&&)		= default;
			virtual ~ICodeGenerator()				= default;

			/**
			*	//TODO
			*/
			virtual ETraversalBehaviour	generateCode(EntityInfo const&										entity,
													 CodeGenEnv&											env,
													 std::function<ETraversalBehaviour(ICodeGenerator&,
																					   EntityInfo const&,
																					   CodeGenEnv&,
																					   void const*)>		visitor)		noexcept = 0;

			/**
			*	//TODO
			*/
			virtual ETraversalBehaviour generateCodeInterface(EntityInfo const&	entity,
															  CodeGenEnv&		env,
															  std::string&		inout_result,
															  void const*		data)										noexcept = 0;

			/**
			*	@brief	The generation order is a number defining in which order a code generator will generate code compared to other code generators.
			*			Code generators with a low generation order will execute first, and code generators with a high generation order will execute last.
			*			Code generators having the same generation order will execute in an undefined order.
			*			Default generation order is 0.
			* 
			*	@return The generation order.
			*/
			virtual int32				getGenerationOrder()														const	noexcept;

			ICodeGenerator& operator=(ICodeGenerator const&)	= default;
			ICodeGenerator& operator=(ICodeGenerator&&)			= default;
	};
}