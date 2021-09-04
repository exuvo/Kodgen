#include "Kodgen/CodeGen/Macro/MacroPropertyCodeGen.h"

#include <cassert>

using namespace kodgen;

bool MacroPropertyCodeGen::generateCodeForEntity(EntityInfo const* entity, Property const* property, uint8 propertyIndex, CodeGenEnv& env, std::string& inout_result) noexcept
{
	MacroCodeGenEnv& macroData = static_cast<MacroCodeGenEnv&>(env);

	//Dispatch code generation call to the right sub-method
	switch (macroData.getCodeGenLocation())
	{
		case ECodeGenLocation::HeaderFileHeader:
			return preGenerateCodeForEntity(entity, property, propertyIndex, macroData) && generateHeaderFileHeaderCodeForEntity(entity, property, propertyIndex, macroData, inout_result);

		case ECodeGenLocation::ClassFooter:
			return generateClassFooterCodeForEntity(entity, property, propertyIndex, macroData, inout_result);

		case ECodeGenLocation::HeaderFileFooter:
			return generateHeaderFileFooterCodeForEntity(entity, property, propertyIndex, macroData, inout_result);

		case ECodeGenLocation::SourceFileHeader:
			return generateSourceFileHeaderCodeForEntity(entity, property, propertyIndex, macroData, inout_result) && postGenerateCodeForEntity(entity, property, propertyIndex, macroData);

		case ECodeGenLocation::Count:
			//Should never get here
			if (env.getLogger() != nullptr)
			{
				env.getLogger()->log("MacroPropertyCodeGen::generateCode called with ECodeGenLocation::Count location. Abort generation.", ILogger::ELogSeverity::Error);
			}
			return false;
	}

	//Should never reach this point as all cases should be handled by the previous switch statement
	assert(false);
	return false;
}

bool MacroPropertyCodeGen::initialGenerateCode(CodeGenEnv& env, std::string& inout_result) noexcept
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

bool MacroPropertyCodeGen::finalGenerateCode(CodeGenEnv& env, std::string& inout_result) noexcept
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

bool MacroPropertyCodeGen::initialGenerateHeaderFileHeaderCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::initialGenerateHeaderFileFooterCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::initialGenerateSourceFileHeaderCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::finalGenerateHeaderFileHeaderCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::finalGenerateHeaderFileFooterCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::finalGenerateSourceFileHeaderCode(MacroCodeGenEnv& env, std::string& inout_result) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::generateHeaderFileHeaderCodeForEntity(EntityInfo const* /* entity */, Property const* /* property */, uint8 /* propertyIndex */,
														MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::generateClassFooterCodeForEntity(EntityInfo const* /* entity */, Property const* /* property */, uint8 /* propertyIndex */,
												   MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::generateHeaderFileFooterCodeForEntity(EntityInfo const* /* entity */, Property const* /* property */, uint8 /* propertyIndex */,
														MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::generateSourceFileHeaderCodeForEntity(EntityInfo const* /* entity */, Property const* /* property */, uint8 /* propertyIndex */,
														MacroCodeGenEnv& /* env */, std::string& /* inout_result */) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::preGenerateCodeForEntity(EntityInfo const* /* entity */, Property const* /* property */, uint8 /* propertyIndex */, MacroCodeGenEnv& /* env */) noexcept
{
	//Default implementation generates no code
	return true;
}

bool MacroPropertyCodeGen::postGenerateCodeForEntity(EntityInfo const* /* entity */, Property const* /* property */, uint8 /* propertyIndex */, MacroCodeGenEnv& /* env */) noexcept
{
	//Default implementation generates no code
	return true;
}