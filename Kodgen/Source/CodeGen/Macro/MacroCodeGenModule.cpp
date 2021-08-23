#include "Kodgen/CodeGen/Macro/MacroCodeGenModule.h"

#include "Kodgen/Config.h"
#include "Kodgen/InfoStructures/EntityInfo.h"
#include "Kodgen/CodeGen/CodeGenHelpers.h"
#include "Kodgen/CodeGen/Macro/MacroCodeGenEnv.h"
#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

using namespace kodgen;

bool MacroCodeGenModule::initialize(CodeGenEnv& env) noexcept
{
	if (CodeGenModule::initialize(env))
	{
		//Check that the provided environment is castable to MacroCodeGenEnv
#ifdef RTTI_ENABLED
		if (dynamic_cast<MacroCodeGenEnv*>(&env) == nullptr)
		{
			env.getLogger()->log("MacroCodeGenModule can't be used with non MacroCodeGenEnv environments.", ILogger::ELogSeverity::Error);
			return false;
		}
		else
		{
			return true;
		}
#else
		//Can't perform the cast check
		return true;
#endif
	}

	return false;
}

ETraversalBehaviour MacroCodeGenModule::generateCode(EntityInfo const* entity, CodeGenEnv& env, std::string& inout_result) noexcept
{
	MacroCodeGenEnv& macroEnv = static_cast<MacroCodeGenEnv&>(env);

	ETraversalBehaviour result;

	//Dispatch code generation call to the right sub-method
	switch (macroEnv.getCodeGenLocation())
	{
		case ECodeGenLocation::HeaderFileHeader:
			if (preGenerateCode(entity, macroEnv))
			{
				//Run property code generators
				result = CodeGenModule::generateCode(entity, env, inout_result);

				return CodeGenHelpers::combineTraversalBehaviours(result, generateHeaderFileHeaderCode(entity, macroEnv, inout_result));
			}
			else
			{
				return ETraversalBehaviour::AbortWithFailure;
			}

		case ECodeGenLocation::ClassFooter:
			//Run property code generators
			result = CodeGenModule::generateCode(entity, env, inout_result);

			return CodeGenHelpers::combineTraversalBehaviours(result, generateClassFooterCode(entity, macroEnv, inout_result));

		case ECodeGenLocation::HeaderFileFooter:
			result = CodeGenModule::generateCode(entity, env, inout_result);

			//CodeGenModule::generateCode runs eligible property code generators if any
			return CodeGenHelpers::combineTraversalBehaviours(result, generateHeaderFileFooterCode(entity, macroEnv, inout_result));

		case ECodeGenLocation::SourceFileHeader:
			result = CodeGenModule::generateCode(entity, env, inout_result);
			
			//Property code gen are triggered BEFORE calling MacroCodeGenModule::postGenerateCode
			result = CodeGenHelpers::combineTraversalBehaviours(result, generateSourceFileHeaderCode(entity, macroEnv, inout_result));

			return (postGenerateCode(entity, macroEnv)) ? result : ETraversalBehaviour::AbortWithFailure;

		case ECodeGenLocation::Count:
			//Should never get here
			if (env.getLogger() != nullptr)
			{
				env.getLogger()->log("MacroPropertyCodeGen::generateCode called with ECodeGenLocation::Count location. Abort generation.", ILogger::ELogSeverity::Error);
			}
			return ETraversalBehaviour::AbortWithFailure;
	}
	
	//Should never reach this point as all cases should be handled by the previous switch statement
	assert(false);

	return ETraversalBehaviour::AbortWithFailure;
}

ETraversalBehaviour MacroCodeGenModule::generateHeaderFileHeaderCode(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return CodeGenHelpers::leastPrioritizedTraversalBehaviour;
}

ETraversalBehaviour MacroCodeGenModule::generateClassFooterCode(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return CodeGenHelpers::leastPrioritizedTraversalBehaviour;
}

ETraversalBehaviour MacroCodeGenModule::generateHeaderFileFooterCode(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return CodeGenHelpers::leastPrioritizedTraversalBehaviour;
}

ETraversalBehaviour MacroCodeGenModule::generateSourceFileHeaderCode(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return CodeGenHelpers::leastPrioritizedTraversalBehaviour;
}

bool MacroCodeGenModule::preGenerateCode(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */) noexcept
{
	//Default implementation does nothing
	return true;
}

bool MacroCodeGenModule::postGenerateCode(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */) noexcept
{
	//Default implementation does nothing
	return true;
}

void MacroCodeGenModule::addPropertyCodeGen(MacroPropertyCodeGen& propertyCodeGen) noexcept
{
	CodeGenModule::addPropertyCodeGen(propertyCodeGen);
}