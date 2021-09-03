#include "Kodgen/Parsing/ClassParser.h"

#include <cassert>

#include "Kodgen/Parsing/ParsingSettings.h"
#include "Kodgen/Parsing/PropertyParser.h"
#include "Kodgen/InfoStructures/NamespaceInfo.h"
#include "Kodgen/InfoStructures/StructClassInfo.h"
#include "Kodgen/InfoStructures/NestedStructClassInfo.h"
#include "Kodgen/InfoStructures/FieldInfo.h"
#include "Kodgen/InfoStructures/MethodInfo.h"
#include "Kodgen/InfoStructures/EntityInfo.h"
#include "Kodgen/InfoStructures/NestedEnumInfo.h"
#include "Kodgen/InfoStructures/StructClassTree.h"
#include "Kodgen/Misc/Helpers.h"
#include "Kodgen/Misc/DisableWarningMacros.h"

using namespace kodgen;

CXChildVisitResult ClassParser::parse(CXCursor const& classCursor, ParsingContext const& parentContext, ClassParsingResult& out_result) noexcept
{
	//Make sure the cursor is compatible for the class parser
	assert(classCursor.kind == CXCursorKind::CXCursor_ClassDecl || classCursor.kind == CXCursorKind::CXCursor_StructDecl);

	//Init context
	ParsingContext& context = pushContext(classCursor, parentContext, out_result);

	if (!clang_visitChildren(classCursor, &ClassParser::parseNestedEntity, this) && context.shouldCheckProperties)
	{
		//If we reach this point, the cursor had no child (no annotation)
		//Check if the parent has the shouldParseAllNested flag set
		if (shouldParseCurrentEntity())
		{
			getParsingResult()->parsedClass.emplace(classCursor, std::vector<Property>(), (classCursor.kind == CXCursorKind::CXCursor_ClassDecl) ? EEntityType::Class : EEntityType::Struct, isForwardDeclaration(classCursor));
		}
	}

	popContext();

	DISABLE_WARNING_PUSH
	DISABLE_WARNING_UNSCOPED_ENUM

	return (parentContext.parsingSettings->shouldAbortParsingOnFirstError && !out_result.errors.empty()) ? CXChildVisitResult::CXChildVisit_Break : CXChildVisitResult::CXChildVisit_Continue;

	DISABLE_WARNING_POP
}

CXChildVisitResult ClassParser::parseNestedEntity(CXCursor cursor, CXCursor /* parentCursor */, CXClientData clientData) noexcept
{
	ClassParser*	parser	= reinterpret_cast<ClassParser*>(clientData);
	ParsingContext&	context = parser->getContext();

	if (context.shouldCheckProperties)
	{
		context.shouldCheckProperties = false;

		if (parser->shouldParseCurrentEntity() && cursor.kind != CXCursorKind::CXCursor_AnnotateAttr)
		{
			//Make it valid right away so init the result
			parser->getParsingResult()->parsedClass.emplace(context.rootCursor, std::vector<Property>(), (context.rootCursor.kind == CXCursorKind::CXCursor_ClassDecl) ? EEntityType::Class : EEntityType::Struct, isForwardDeclaration(context.rootCursor));
		}
		else
		{
			//Set parsed struct/class in result if it is valid
			return parser->setParsedEntity(cursor);
		}
	}

	CXChildVisitResult visitResult = CXChildVisitResult::CXChildVisit_Continue;

	switch (cursor.kind)
	{
		case CXCursorKind::CXCursor_CXXFinalAttr:
			if (parser->getParsingResult()->parsedClass.has_value())
			{
				parser->getParsingResult()->parsedClass->qualifiers.isFinal = true;
			}
			break;

		case CXCursorKind::CXCursor_CXXAccessSpecifier:
			parser->updateAccessSpecifier(cursor);
			break;

		case CXCursorKind::CXCursor_CXXBaseSpecifier:
			parser->updateStructClassTree(cursor);
			parser->addBaseClass(cursor);
			break;

		case CXCursorKind::CXCursor_Constructor:
			//TODO: Might handle this someday
			break;

		case CXCursorKind::CXCursor_StructDecl:
			[[fallthrough]];
		case CXCursorKind::CXCursor_ClassDecl:
			parser->addClassResult(parser->parseClass(cursor, visitResult));
			break;

		case CXCursorKind::CXCursor_EnumDecl:
			parser->addEnumResult(parser->parseEnum(cursor, visitResult));
			break;

		case CXCursorKind::CXCursor_VarDecl:	//For static fields
			[[fallthrough]];
		case CXCursorKind::CXCursor_FieldDecl:
			parser->addFieldResult(parser->parseField(cursor, visitResult));
			break;

		case CXCursorKind::CXCursor_CXXMethod:
			parser->addMethodResult(parser->parseMethod(cursor, visitResult));
			break;

		default:
			break;
	}

	return visitResult;
}

ClassParsingResult ClassParser::parseClass(CXCursor const& classCursor, CXChildVisitResult& out_visitResult) noexcept
{
	ClassParsingResult classResult;

	out_visitResult	= parse(classCursor, getContext(), classResult);

	return classResult;
}

EnumParsingResult ClassParser::parseEnum(CXCursor const& enumCursor, CXChildVisitResult& out_visitResult) noexcept
{
	EnumParsingResult enumResult;

	out_visitResult = _enumParser.parse(enumCursor, getContext(), enumResult);

	return enumResult;
}

FieldParsingResult ClassParser::parseField(CXCursor const& fieldCursor, CXChildVisitResult& out_visitResult) noexcept
{
	FieldParsingResult fieldResult;

	out_visitResult = _fieldParser.parse(fieldCursor, getContext(), fieldResult);

	return fieldResult;
}

MethodParsingResult	ClassParser::parseMethod(CXCursor const& methodCursor, CXChildVisitResult& out_visitResult) noexcept
{
	MethodParsingResult methodResult;

	out_visitResult = _methodParser.parse(methodCursor, getContext(), methodResult);

	return methodResult;
}

ParsingContext& ClassParser::pushContext(CXCursor const& classCursor, ParsingContext const& parentContext, ClassParsingResult& out_result) noexcept
{
	ParsingContext newContext;

	newContext.parentContext			= &parentContext;
	newContext.rootCursor				= classCursor;
	newContext.shouldCheckProperties	= true;
	newContext.propertyParser			= parentContext.propertyParser;
	newContext.parsingSettings			= parentContext.parsingSettings;
	newContext.structClassTree			= parentContext.structClassTree;
	newContext.parsingResult			= &out_result;
	newContext.currentAccessSpecifier	= (classCursor.kind == CXCursorKind::CXCursor_ClassDecl) ? EAccessSpecifier::Private : EAccessSpecifier::Public;

	contextsStack.push(std::move(newContext));

	return contextsStack.top();
}

CXChildVisitResult ClassParser::setParsedEntity(CXCursor const& annotationCursor) noexcept
{
	ParsingContext& context = getContext();

	if (opt::optional<std::vector<Property>> properties = getProperties(annotationCursor))
	{
		//Set the parsing entity in the result and update the shouldParseAllNested flag in the context
		updateShouldParseAllNested(getParsingResult()->parsedClass.emplace(context.rootCursor, std::move(*properties), (context.rootCursor.kind == CXCursorKind::CXCursor_ClassDecl) ? EEntityType::Class : EEntityType::Struct, isForwardDeclaration(context.rootCursor)));

		return CXChildVisitResult::CXChildVisit_Recurse;
	}
	else
	{
		if (!context.propertyParser->getParsingErrorDescription().empty())
		{
			context.parsingResult->errors.emplace_back(ParsingError(context.propertyParser->getParsingErrorDescription(), clang_getCursorLocation(annotationCursor)));
		}

		return CXChildVisitResult::CXChildVisit_Break;
	}
}

opt::optional<std::vector<Property>> ClassParser::getProperties(CXCursor const& cursor) noexcept
{
	ParsingContext& context = getContext();

	context.propertyParser->clean();

	if (clang_getCursorKind(cursor) == CXCursorKind::CXCursor_AnnotateAttr)
	{
		switch (context.rootCursor.kind)
		{
			case CXCursorKind::CXCursor_ClassDecl:
				return context.propertyParser->getClassProperties(Helpers::getString(clang_getCursorSpelling(cursor)));
				break;

			case CXCursorKind::CXCursor_StructDecl:
				return context.propertyParser->getStructProperties(Helpers::getString(clang_getCursorSpelling(cursor)));
				break;

			default:
				break;
		}
	}

	return opt::nullopt;
}

void ClassParser::updateAccessSpecifier(CXCursor const& cursor) noexcept
{
	assert(cursor.kind == CXCursorKind::CXCursor_CXXAccessSpecifier);

	getContext().currentAccessSpecifier = static_cast<EAccessSpecifier>(clang_getCXXAccessSpecifier(cursor));
}

void ClassParser::updateStructClassTree(CXCursor cursor) noexcept
{
	ParsingContext& context = getContext();

	updateStructClassTreeRecursion(clang_getCanonicalType(clang_getCursorType(context.rootCursor)), cursor, *context.structClassTree);
}

void ClassParser::updateStructClassTreeRecursion(CXType childType, CXCursor baseOfCursor, StructClassTree& out_structClassTree) noexcept
{
	//Make sure the baseOf cursor is indeed a CXCursor_CXXBaseSpecifier
	assert(clang_getCursorKind(baseOfCursor) == CXCursorKind::CXCursor_CXXBaseSpecifier);

	if (out_structClassTree.addInheritanceLink(Helpers::getString(clang_getTypeSpelling(childType)),
											   Helpers::getString(clang_getTypeSpelling(clang_getCanonicalType(clang_getCursorType(baseOfCursor)))),
											   static_cast<EAccessSpecifier>(clang_getCXXAccessSpecifier(baseOfCursor))))
	{
		//Recursively fill inheritance tree
		clang_visitChildren(clang_getTypeDeclaration(clang_getCursorType(baseOfCursor)), [](CXCursor cursor, CXCursor parentCursor, CXClientData clientData)
		{
			StructClassTree& structClassTree = *reinterpret_cast<StructClassTree*>(clientData);

			if (cursor.kind == CXCursorKind::CXCursor_CXXBaseSpecifier)
			{
				//Recurse on parentType's parents
				updateStructClassTreeRecursion(clang_getCanonicalType(clang_getCursorType(parentCursor)), cursor, structClassTree);

				return CXChildVisitResult::CXChildVisit_Continue;
			}
			else if (cursor.kind == CXCursorKind::CXCursor_CXXFinalAttr || cursor.kind == CXCursorKind::CXCursor_AnnotateAttr)
			{
				return CXChildVisitResult::CXChildVisit_Continue;
			}
			else
			{
				return CXChildVisitResult::CXChildVisit_Break;
			}
		}, &out_structClassTree);
	}
}

void ClassParser::addBaseClass(CXCursor cursor) noexcept
{
	assert(clang_getCursorKind(cursor) == CXCursorKind::CXCursor_CXXBaseSpecifier);

	if (getParsingResult()->parsedClass.has_value())
	{
		StructClassInfo&	parsedClass = getParsingResult()->parsedClass.value();
		CXType				parentType	= clang_getCursorType(cursor);

		parsedClass.parents.emplace_back(StructClassInfo::ParentInfo{ static_cast<EAccessSpecifier>(clang_getCXXAccessSpecifier(cursor)), TypeInfo(parentType) });
	}
}

void ClassParser::addFieldResult(FieldParsingResult&& result) noexcept
{
	ParsingContext& context = getContext();

	if (result.parsedField.has_value() && getParsingResult()->parsedClass.has_value())
	{
		//Update field access specifier
		result.parsedField->accessSpecifier = context.currentAccessSpecifier;

		getParsingResult()->parsedClass->fields.emplace_back(std::move(result.parsedField).value());
	}

	getParsingResult()->appendResultErrors(result);
}

void ClassParser::addMethodResult(MethodParsingResult&& result) noexcept
{
	ParsingContext& context = getContext();

	if (result.parsedMethod.has_value() && getParsingResult()->parsedClass.has_value())
	{
		//Update method access specifier
		result.parsedMethod->accessSpecifier = context.currentAccessSpecifier;

		getParsingResult()->parsedClass->methods.emplace_back(std::move(result.parsedMethod).value());
	}

	getParsingResult()->appendResultErrors(result);
}

void ClassParser::addClassResult(ClassParsingResult&& result) noexcept
{
	ParsingContext& context = getContext();

	if (result.parsedClass.has_value() && getParsingResult()->parsedClass.has_value())
	{
		switch (result.parsedClass->entityType)
		{
			case EEntityType::Struct:
				getParsingResult()->parsedClass->nestedStructs.emplace_back(std::make_shared<NestedStructClassInfo>(std::move(result.parsedClass).value(), context.currentAccessSpecifier));
				break;

			case EEntityType::Class:
				getParsingResult()->parsedClass->nestedClasses.emplace_back(std::make_shared<NestedStructClassInfo>(std::move(result.parsedClass).value(), context.currentAccessSpecifier));
				break;

			default:
				assert(false);	//Should never reach this point
				break;
		}
	}

	getParsingResult()->appendResultErrors(result);
}

void ClassParser::addEnumResult(EnumParsingResult&& result) noexcept
{
	ParsingContext& context = getContext();

	if (result.parsedEnum.has_value() && getParsingResult()->parsedClass.has_value())
	{
		getParsingResult()->parsedClass->nestedEnums.emplace_back(NestedEnumInfo(std::move(result.parsedEnum).value(), context.currentAccessSpecifier));
	}

	getParsingResult()->appendResultErrors(result);
}

bool ClassParser::isForwardDeclaration(CXCursor cursor) noexcept
{
	assert(cursor.kind == CXCursorKind::CXCursor_ClassDecl || cursor.kind == CXCursorKind::CXCursor_StructDecl);

	CXCursor definition = clang_getCursorDefinition(cursor);

	return	clang_equalCursors(definition, clang_getNullCursor()) ||	//Couldn't find the definition in the TU
			!clang_equalCursors(cursor, definition);					//Found a definition but it's not the same as the checked cursor
}