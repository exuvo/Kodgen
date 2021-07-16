#include "Kodgen/CodeGen/CodeGenManagerSettings.h"

#include "Kodgen/Misc/TomlUtility.h"
#include "Kodgen/Misc/ILogger.h"

using namespace kodgen;

bool CodeGenManagerSettings::loadSettingsValues(toml::value const& tomlData, ILogger* logger) noexcept
{
	if (tomlData.contains(_tomlSectionName))
	{
		toml::value const& tomlGeneratorSettings = toml::find(tomlData, _tomlSectionName);
		
		loadSupportedExtensions(tomlGeneratorSettings, logger);
		loadToProcessFiles(tomlGeneratorSettings, logger);
		loadToProcessDirectories(tomlGeneratorSettings, logger);
		loadIgnoredFiles(tomlGeneratorSettings, logger);
		loadIgnoredDirectories(tomlGeneratorSettings, logger);

		return true;
	}
	else if (logger != nullptr)
	{
		logger->log("Could not find the [" + std::string(_tomlSectionName) + "] section in the TOML file.", ILogger::ELogSeverity::Warning);
	}

	return false;
}

bool CodeGenManagerSettings::addToProcessFile(fs::path const& path) noexcept
{
	fs::path sanitizedPath = FilesystemHelpers::sanitizePath(path);

	if (!sanitizedPath.empty() && !fs::is_directory(sanitizedPath))
	{
		return _toProcessFiles.emplace(std::move(sanitizedPath)).second;
	}

	return false;
}

bool CodeGenManagerSettings::addToProcessDirectory(fs::path const& path) noexcept
{
	fs::path sanitizedPath = FilesystemHelpers::sanitizePath(path);

	if (!sanitizedPath.empty() && fs::is_directory(sanitizedPath))
	{
		return _toProcessDirectories.emplace(std::move(sanitizedPath)).second;
	}

	return false;
}

bool CodeGenManagerSettings::addIgnoredFile(fs::path const& path) noexcept
{
	fs::path sanitizedPath = FilesystemHelpers::sanitizePath(path);

	if (!sanitizedPath.empty() && !fs::is_directory(sanitizedPath))
	{
		return _ignoredFiles.emplace(std::move(sanitizedPath)).second;
	}

	return false;
}

bool CodeGenManagerSettings::addIgnoredDirectory(fs::path const& path) noexcept
{
	fs::path sanitizedPath = FilesystemHelpers::sanitizePath(path);

	if (!sanitizedPath.empty() && fs::is_directory(sanitizedPath))
	{
		return _ignoredDirectories.emplace(std::move(sanitizedPath)).second;
	}

	return false;
}

bool CodeGenManagerSettings::addSupportedExtension(fs::path const& extension) noexcept
{
	std::string extensionAsString = extension.string();

	if (!extensionAsString.empty() && extensionAsString[0] == '.' && extension.has_stem())
	{
		_supportedExtensions.emplace(std::move(extensionAsString));

		return true;
	}

	return false;
}

void CodeGenManagerSettings::removeToProcessFile(fs::path const& path) noexcept
{
	_toProcessFiles.erase(FilesystemHelpers::sanitizePath(path));
}

void CodeGenManagerSettings::removeToProcessDirectory(fs::path const& path) noexcept
{
	_toProcessDirectories.erase(FilesystemHelpers::sanitizePath(path));
}

void CodeGenManagerSettings::removeIgnoredFile(fs::path const& path) noexcept
{
	_ignoredFiles.erase(FilesystemHelpers::sanitizePath(path));
}

void CodeGenManagerSettings::removeSupportedExtension(fs::path const& ext) noexcept
{
	_supportedExtensions.erase(ext.string());
}

void CodeGenManagerSettings::removeIgnoredDirectory(fs::path const& path) noexcept
{
	_ignoredDirectories.erase(FilesystemHelpers::sanitizePath(path));
}

void CodeGenManagerSettings::clearToProcessFiles() noexcept
{
	_toProcessFiles.clear();
}

void CodeGenManagerSettings::clearToProcessDirectories() noexcept
{
	_toProcessDirectories.clear();
}

void CodeGenManagerSettings::clearIgnoredFiles() noexcept
{
	_ignoredFiles.clear();
}

void CodeGenManagerSettings::clearIgnoredDirectories() noexcept
{
	_ignoredDirectories.clear();
}

void CodeGenManagerSettings::clearSupportedExtensions() noexcept
{
	_supportedExtensions.clear();
}

void CodeGenManagerSettings::loadSupportedExtensions(toml::value const& generationSettings, ILogger* logger) noexcept
{
	//Clear supported extensions before loading
	_supportedExtensions.clear();

	std::unordered_set<std::string> loadedExtensions;
	if (TomlUtility::updateSetting(generationSettings, "supportedExtensions", loadedExtensions, logger) && logger != nullptr)
	{
		for (std::string const& extension : loadedExtensions)
		{
			if (addSupportedExtension(extension))
			{
				if (logger != nullptr)
				{
					logger->log("[TOML] Load new supported extension: " + extension);
				}
			}
			else if (logger != nullptr)
			{
				if (logger != nullptr)
				{
					logger->log("[TOML] Failed to add supported extension: " + extension);
				}
			}
		}
	}
}

void CodeGenManagerSettings::loadToProcessFiles(toml::value const& generationSettings, ILogger* logger) noexcept
{
	std::unordered_set<fs::path, PathHash> toParseFiles;

	clearToProcessFiles();

	if (TomlUtility::updateSetting(generationSettings, "toProcessFiles", toParseFiles, logger))
	{
		bool success;

		for (fs::path const& path : toParseFiles)
		{
			success = addToProcessFile(path);

			if (logger != nullptr)
			{
				if (success)
				{
					logger->log("[TOML] Load new file to parse: " + FilesystemHelpers::sanitizePath(path).string());
				}
				else
				{
					logger->log("[TOML] Failed to add toParseFile as it doesn't exist, is not a file or is already part of the list of files to parse: " + path.string(), ILogger::ELogSeverity::Warning);
				}
			}
		}
	}
}

void CodeGenManagerSettings::loadToProcessDirectories(toml::value const& generationSettings, ILogger* logger) noexcept
{
	std::unordered_set<fs::path, PathHash> toParseDirectories;

	clearToProcessDirectories();

	if (TomlUtility::updateSetting(generationSettings, "toProcessDirectories", toParseDirectories, logger))
	{
		bool success;

		for (fs::path const& path : toParseDirectories)
		{
			success = addToProcessDirectory(path);

			if (logger != nullptr)
			{
				if (success)
				{
					logger->log("[TOML] Load new directory to parse: " + FilesystemHelpers::sanitizePath(path).string());
				}
				else
				{
					logger->log("[TOML] Failed to add toParseDirectory as it doesn't exist, is not a directory or is already part of the list of directories to parse: " + path.string(), ILogger::ELogSeverity::Warning);
				}
			}
		}
	}
}

void CodeGenManagerSettings::loadIgnoredFiles(toml::value const& generationSettings, ILogger* logger) noexcept
{
	std::unordered_set<fs::path, PathHash> ignoredFiles;

	clearIgnoredFiles();

	if (TomlUtility::updateSetting(generationSettings, "ignoredFiles", ignoredFiles, logger))
	{
		bool success;

		for (fs::path const& path : ignoredFiles)
		{
			success = addIgnoredFile(path);

			if (logger != nullptr)
			{
				if (success)
				{
					logger->log("[TOML] Load new ignored file: " + FilesystemHelpers::sanitizePath(path).string());
				}
				else
				{
					logger->log("[TOML] Failed to add ignoredFile as it doesn't exist, is not a file or is already part of the list of ignored files: " + path.string(), ILogger::ELogSeverity::Warning);
				}
			}
		}
	}
}

void CodeGenManagerSettings::loadIgnoredDirectories(toml::value const& generationSettings, ILogger* logger) noexcept
{
	std::unordered_set<fs::path, PathHash> ignoredDirectories;

	clearIgnoredDirectories();

	if (TomlUtility::updateSetting(generationSettings, "ignoredDirectories", ignoredDirectories, logger))
	{
		bool success;

		for (fs::path const& path : ignoredDirectories)
		{
			success = addIgnoredDirectory(path);

			if (logger != nullptr)
			{
				if (success)
				{
					logger->log("[TOML] Load new ignored directory: " + FilesystemHelpers::sanitizePath(path).string());
				}
				else
				{
					logger->log("[TOML] Failed to add ignoredDirectory as it doesn't exist, is not a directory or is already part of the list of ignored directories: " + path.string(), ILogger::ELogSeverity::Warning);
				}
			}
		}
	}
}

std::unordered_set<fs::path, PathHash> const& CodeGenManagerSettings::getToProcessFiles() const noexcept
{
	return _toProcessFiles;
}

std::unordered_set<fs::path, PathHash> const& CodeGenManagerSettings::getToProcessDirectories() const noexcept
{
	return _toProcessDirectories;
}

std::unordered_set<fs::path, PathHash> const& CodeGenManagerSettings::getIgnoredFiles() const noexcept
{
	return _ignoredFiles;
}

std::unordered_set<fs::path, PathHash> const& CodeGenManagerSettings::getIgnoredDirectories() const noexcept
{
	return _ignoredDirectories;
}

std::unordered_set<std::string> const& CodeGenManagerSettings::getSupportedExtensions() const noexcept
{
	return _supportedExtensions;
}