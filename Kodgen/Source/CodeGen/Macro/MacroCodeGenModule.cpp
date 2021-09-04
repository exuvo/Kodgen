#include "Kodgen/CodeGen/Macro/MacroCodeGenModule.h"

#include "Kodgen/Config.h"
#include "Kodgen/InfoStructures/EntityInfo.h"
#include "Kodgen/CodeGen/CodeGenHelpers.h"
#include "Kodgen/CodeGen/Macro/MacroCodeGenEnv.h"
#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

using namespace kodgen;

ETraversalBehaviour MacroCodeGenModule::generateCodeForEntity(EntityInfo const* entity, CodeGenEnv& env, std::string& inout_result) noexcept
{
	MacroCodeGenEnv& macroEnv = static_cast<MacroCodeGenEnv&>(env);

	ETraversalBehaviour result;

	//Dispatch code generation call to the right sub-method
	switch (macroEnv.getCodeGenLocation())
	{
		case ECodeGenLocation::HeaderFileHeader:
			if (preGenerateCodeForEntity(entity, macroEnv))
			{
				return generateHeaderFileHeaderCodeForEntity(entity, macroEnv, inout_result);
			}
			else
			{
				return ETraversalBehaviour::AbortWithFailure;
			}

		case ECodeGenLocation::ClassFooter:
			return generateClassFooterCodeForEntity(entity, macroEnv, inout_result);

		case ECodeGenLocation::HeaderFileFooter:
			return generateHeaderFileFooterCodeForEntity(entity, macroEnv, inout_result);

		case ECodeGenLocation::SourceFileHeader:
			result = generateSourceFileHeaderCodeForEntity(entity, macroEnv, inout_result);

			return (postGenerateCodeForEntity(entity, macroEnv)) ? result : ETraversalBehaviour::AbortWithFailure;

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

bool MacroCodeGenModule::initialGenerateCode(CodeGenEnv& env, std::string& inout_result) noexcept
{
	//Check that the provided environment is castable to MacroCodeGenEnv
#ifdef RTTI_ENABLED
	if (dynamic_cast<MacroCodeGenEnv*>(&env) == nullptr)
	{
		env.getLogger()->log("MacroCodeGenModule can't be used with non MacroCodeGenEnv environments.", ILogger::ELogSeverity::Error);
		return false;
	}
#else
	//Can't perform the cast check
#endif

	MacroCodeGenEnv& macroEnv = static_cast<MacroCodeGenEnv&>(env);

	//Dispatch code generation call to the right sub-method
	switch (macroEnv.getCodeGenLocation())
	{
		case ECodeGenLocation::HeaderFileHeader:
			return initialGenerateHeaderFileHeaderCode(macroEnv, inout_result);

		case ECodeGenLocation::HeaderFileFooter:
			return initialGenerateHeaderFileFooterCode(macroEnv, inout_result);

		case ECodeGenLocation::SourceFileHeader:
			return initialGenerateSourceFileHeaderCode(macroEnv, inout_result);

		case ECodeGenLocation::ClassFooter:
			[[fallthrough]];
		case ECodeGenLocation::Count:
			[[fallthrough]];
		default:
			//Should never get here
			if (env.getLogger() != nullptr)
			{
				env.getLogger()->log("MacroPropertyCodeGen::generateCode called with ECodeGenLocation::Count location. Abort generation.", ILogger::ELogSeverity::Error);
			}
			return false;
	}
}

bool MacroCodeGenModule::finalGenerateCode(CodeGenEnv& env, std::string& inout_result) noexcept
{
	MacroCodeGenEnv& macroEnv = static_cast<MacroCodeGenEnv&>(env);

	//Dispatch code generation call to the right sub-method
	switch (macroEnv.getCodeGenLocation())
	{
		case ECodeGenLocation::HeaderFileHeader:
			return finalGenerateHeaderFileHeaderCode(macroEnv, inout_result);

		case ECodeGenLocation::HeaderFileFooter:
			return finalGenerateHeaderFileFooterCode(macroEnv, inout_result);

		case ECodeGenLocation::SourceFileHeader:
			return finalGenerateSourceFileHeaderCode(macroEnv, inout_result);

		case ECodeGenLocation::ClassFooter:
			[[fallthrough]];
		case ECodeGenLocation::Count:
			[[fallthrough]];
		default:
			//Should never get here
			if (env.getLogger() != nullptr)
			{
				env.getLogger()->log("MacroPropertyCodeGen::generateCode called with ECodeGenLocation::Count location. Abort generation.", ILogger::ELogSeverity::Error);
			}
			return false;
	}
}

bool MacroCodeGenModule::initialGenerateHeaderFileHeaderCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroCodeGenModule::initialGenerateHeaderFileFooterCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroCodeGenModule::initialGenerateSourceFileHeaderCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroCodeGenModule::finalGenerateHeaderFileHeaderCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroCodeGenModule::finalGenerateHeaderFileFooterCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroCodeGenModule::finalGenerateSourceFileHeaderCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

ETraversalBehaviour MacroCodeGenModule::generateHeaderFileHeaderCodeForEntity(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return CodeGenHelpers::leastPrioritizedTraversalBehaviour;
}

ETraversalBehaviour MacroCodeGenModule::generateClassFooterCodeForEntity(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return CodeGenHelpers::leastPrioritizedTraversalBehaviour;
}

ETraversalBehaviour MacroCodeGenModule::generateHeaderFileFooterCodeForEntity(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return CodeGenHelpers::leastPrioritizedTraversalBehaviour;
}

ETraversalBehaviour MacroCodeGenModule::generateSourceFileHeaderCodeForEntity(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return CodeGenHelpers::leastPrioritizedTraversalBehaviour;
}

bool MacroCodeGenModule::preGenerateCodeForEntity(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */) noexcept
{
	//Default implementation does nothing
	return true;
}

bool MacroCodeGenModule::postGenerateCodeForEntity(EntityInfo const* /* entity */, MacroCodeGenEnv& /* env */) noexcept
{
	//Default implementation does nothing
	return true;
}

void MacroCodeGenModule::addPropertyCodeGen(MacroPropertyCodeGen& propertyCodeGen) noexcept
{
	CodeGenModule::addPropertyCodeGen(propertyCodeGen);
}