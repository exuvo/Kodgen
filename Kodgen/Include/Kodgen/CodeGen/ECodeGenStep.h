/**
*	Copyright (c) 2021 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

namespace kodgen
{
	enum class ECodeGenStep
	{
		/**
		*	Step during which CodeGenUnit & CodeGenModule are initialized.
		*	CodeGenUnit::preGenerateCode & CodeGenModule::initialize are called during this step.
		*/
		PreGeneration = 0,

		/**
		*	Step during which a **single** call to generateCode is performed on each CodeGenModule and PropertyCodeGen
		*	**before** iterating over the parsing result entities.
		*/
		InitialGeneration,

		/**
		*	Step during which all entities are traversed and forwarded to the generateCode method
		*	on each registered CodeGenModule and PropertyCodeGen.
		*/
		PerEntityGeneration,

		/**
		*	Step during which a **single** call to generateCode is performed on each CodeGenModule
		*	**after** the iteration over the parsing result entities have finished.
		*/
		FinalGeneration,

		/**
		*	Step during which CodeGenUnit::postGenerateCode is called.
		*/
		PostGeneration
	};
}