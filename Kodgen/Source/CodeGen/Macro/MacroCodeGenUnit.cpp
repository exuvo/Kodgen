#include "Kodgen/CodeGen/Macro/MacroCodeGenUnit.h"

#include "Kodgen/Config.h"
#include "Kodgen/CodeGen/GeneratedFile.h"
#include "Kodgen/CodeGen/CodeGenHelpers.h"
#include "Kodgen/CodeGen/Macro/MacroCodeGenUnitSettings.h"
#include "Kodgen/CodeGen/Macro/MacroCodeGenModule.h"

using namespace kodgen;

std::array<std::string, static_cast<size_t>(ECodeGenLocation::Count)> const MacroCodeGenUnit::_separators =
{
	"\n",	//HeaderFileHeader is not wrapped inside a macro, so can use \n without breaking the code
	"\\\n",	//ClassFooter is wrapped in a macro so must use \ to keep multiline generated code valid
	"\\\n",	//HeaderFileFooter is wrapped in a macro so must use \ to keep multiline generated code valid
	"\n"	//SourceFileHeader is not wrapped in a macro, so can use \n without breaking the code
};

void MacroCodeGenUnit::generateCodeForEntity(EntityInfo const& entity, CodeGenEnv& env, std::function<void(EntityInfo const&, CodeGenEnv&, std::string&)> generate)	noexcept
{
	MacroCodeGenEnv& macroEnv = static_cast<MacroCodeGenEnv&>(env);

	//Generate code for each code location
	for (int i = 0u; i < static_cast<int>(ECodeGenLocation::Count); i++)
	{
		macroEnv._codeGenLocation	= static_cast<ECodeGenLocation>(i);
		macroEnv._separator			= _separators[i];

		/**
		*	Forward ECodeGenLocation::ClassFooter generation only if the entity is a
		*	struct, class, method or field
		*/
		if (macroEnv._codeGenLocation == ECodeGenLocation::ClassFooter)
		{
			if (!(entity.entityType == EEntityType::Struct || entity.entityType == EEntityType::Class ||
				entity.entityType == EEntityType::Method || entity.entityType == EEntityType::Field))
			{
				continue;
			}
			else
			{
				generateEntityClassFooterCode(entity, macroEnv, generate);
			}
		}
		else
		{
			generate(entity, macroEnv, _generatedCodePerLocation[i]);
		}
	}
}

MacroCodeGenEnv* MacroCodeGenUnit::createCodeGenEnv() const noexcept
{
	return new MacroCodeGenEnv();
}

bool MacroCodeGenUnit::preGenerateCode(FileParsingResult const& parsingResult, CodeGenEnv& env) noexcept
{
	if (CodeGenUnit::preGenerateCode(parsingResult, env))
	{
		//Reset variables before the generation step begins
		_classFooterGeneratedCode.clear();

		for (std::string& generatedCode : _generatedCodePerLocation)
		{
			generatedCode.clear();
		}

		return true;
	}

	return false;
}

bool MacroCodeGenUnit::postGenerateCode(CodeGenEnv& env) noexcept
{
	//Create generated header & generated source files
	generateHeaderFile(static_cast<MacroCodeGenEnv&>(env));
	generateSourceFile(static_cast<MacroCodeGenEnv&>(env));

	return true;
}

void MacroCodeGenUnit::generateHeaderFile(MacroCodeGenEnv& env) noexcept
{
	GeneratedFile generatedHeader(getGeneratedHeaderFilePath(env.getFileParsingResult()->parsedFile), env.getFileParsingResult()->parsedFile);

	MacroCodeGenUnitSettings const* castSettings = static_cast<MacroCodeGenUnitSettings const*>(settings);

	generatedHeader.writeLine("#pragma once\n");

	//Include the entity file
	generatedHeader.writeLine("#include \"" + CodeGenUnitSettings::entityMacrosFilename.string() + "\"");

	//Write header file header code
	generatedHeader.writeLine(std::move(_generatedCodePerLocation[static_cast<int>(ECodeGenLocation::HeaderFileHeader)]));

	//Write all struct/class footer macros
	//We must iterate over all structs/class from scratch since registered generators are not guaranteed to traverse all struct/class
	env.getFileParsingResult()->foreachEntityOfType(EEntityType::Class | EEntityType::Struct,
													[this, &generatedHeader, castSettings](EntityInfo const& entity)
													{
														//Cast is safe since we only iterate on structs & classes
														StructClassInfo const* struct_ = reinterpret_cast<StructClassInfo const*>(&entity);

														auto it = _classFooterGeneratedCode.find(struct_);

														generatedHeader.writeMacro(castSettings->getClassFooterMacro(*struct_), (it != _classFooterGeneratedCode.end()) ? std::move(it->second) : std::string());
													});

	//Write header file footer code
	generatedHeader.writeMacro(castSettings->getHeaderFileFooterMacro(env.getFileParsingResult()->parsedFile),
							   std::move(_generatedCodePerLocation[static_cast<int>(ECodeGenLocation::HeaderFileFooter)]));
}

void MacroCodeGenUnit::generateSourceFile(MacroCodeGenEnv& env) noexcept
{
	GeneratedFile generatedSource(getGeneratedSourceFilePath(env.getFileParsingResult()->parsedFile), env.getFileParsingResult()->parsedFile);

	generatedSource.writeLine("#pragma once\n");
	
	//Include the header file
	generatedSource.writeLine("#include \"" + env.getFileParsingResult()->parsedFile.string() + "\"\n");

	generatedSource.writeLine(std::move(_generatedCodePerLocation[static_cast<int>(ECodeGenLocation::SourceFileHeader)]));
}

bool MacroCodeGenUnit::isUpToDate(fs::path const& sourceFile) const noexcept
{
	fs::path generatedHeaderPath = getGeneratedHeaderFilePath(sourceFile);

	//If the generated header doesn't exist, create it and return false
	if (!fs::exists(generatedHeaderPath))
	{
		GeneratedFile generatedHeader(fs::path(generatedHeaderPath), sourceFile);
	}
	else if (isFileNewerThan(generatedHeaderPath, sourceFile))
	{
		fs::path generatedSource = getGeneratedSourceFilePath(sourceFile);

		return fs::exists(generatedSource) && isFileNewerThan(generatedSource, sourceFile);
	}

	return false;
}

void MacroCodeGenUnit::generateEntityClassFooterCode(EntityInfo const& entity, CodeGenEnv& env, std::function<void(EntityInfo const&, CodeGenEnv&, std::string&)> generate) noexcept
{
	if (entity.entityType == EEntityType::Struct || entity.entityType == EEntityType::Class)
	{
		//If the entity is a struct/class, append to the footer of the struct/class
		generate(entity, env, _classFooterGeneratedCode[&static_cast<StructClassInfo const&>(entity)]);
	}
	else
	{
		assert(entity.outerEntity != nullptr);
		assert(entity.outerEntity->entityType == EEntityType::Struct || entity.outerEntity->entityType == EEntityType::Class);

		//If the entity is NOT a struct/class, append to the footer of the outer struct/class
		generate(entity, env, _classFooterGeneratedCode[static_cast<StructClassInfo const*>(entity.outerEntity)]);
	}
}

fs::path MacroCodeGenUnit::getGeneratedHeaderFilePath(fs::path const& sourceFile) const noexcept
{
	return settings->getOutputDirectory() / static_cast<MacroCodeGenUnitSettings const*>(settings)->getGeneratedHeaderFileName(sourceFile);
}

fs::path MacroCodeGenUnit::getGeneratedSourceFilePath(fs::path const& sourceFile) const noexcept
{
	return settings->getOutputDirectory() / static_cast<MacroCodeGenUnitSettings const*>(settings)->getGeneratedSourceFileName(sourceFile);
}

void MacroCodeGenUnit::addModule(MacroCodeGenModule& generationModule) noexcept
{
	CodeGenUnit::addModule(generationModule);
}

void MacroCodeGenUnit::setSettings(MacroCodeGenUnitSettings const& cguSettings) noexcept
{
	settings = &cguSettings;
}