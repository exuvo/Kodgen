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
			*	@brief	Call the visitor function one or multiple times on this code generator with the provided entity and environment.
			*			The visitor later calls ICodeGenerator::generateCode, and it is possible to forward custom data to it.
			* 
			*	@param entity	The entity provided to the visitor.
			*	@param env		The environment provided to the visitor.
			*	@param visitor	The visitor to run.
			* 
			*	@return A ETraversalBehaviour defining how the CodeGenUnit should pick the next entity.
			*/
			virtual ETraversalBehaviour	callVisitorOnEntity(EntityInfo const&									entity,
															CodeGenEnv&											env,
															std::function<ETraversalBehaviour(ICodeGenerator&,
																							  EntityInfo const&,
																							  CodeGenEnv&,
																							  void const*)>		visitor)	noexcept = 0;

			/**
			*	@brief	Generate code for the provided entity/environment pair.
			* 
			*	@param entity		The entity this generator should generate code for.
			*	@param env			The generation environment structure.
			*	@param inout_result	String the generated code should be appended to.
			*	@param data			Additional data forwarded from ICodeGenerator::callVisitorOnEntity.
			* 
			*	@return A ETraversalBehaviour defining how the CodeGenUnit should pick the next entity.
			*/
			virtual ETraversalBehaviour generateCode(EntityInfo const&	entity,
													 CodeGenEnv&		env,
													 std::string&		inout_result,
													 void const*		data)												noexcept = 0;

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