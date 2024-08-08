#include "Logger.h"
#include "SC4VersionDetection.h"
#include "version.h"
#include "cIGZCOM.h"
#include "cRZCOMDllDirector.h"
#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"


#ifdef __clang__
#define NAKED_FUN __attribute__((naked))
#else
#define NAKED_FUN __declspec(naked)
#endif


// #define TRANSPARENT_BASE_ID 0xf83443fa

static constexpr uint32_t kTransparentBaseDllDirectorID = 0x0DE56C42;

static constexpr std::string_view PluginLogFileName = "memo.transparent-texture-fix.log";

static constexpr uint32_t IsCellCovered_InjectPoint = 0x5df34e;
static constexpr uint32_t IsCellCovered_ContinueJump = 0x5df354;
static constexpr uint32_t IsCellCovered_ReturnJump_IsCovered = 0x5df362;
static constexpr uint32_t IsCellCovered_ReturnJump_NotCovered = 0x5df365;

namespace
{
	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}

	void InstallHook(uint32_t address, void (*pfnFunc)(void))
	{
		DWORD oldProtect;
		THROW_IF_WIN32_BOOL_FALSE(VirtualProtect((void *)address, 5, PAGE_EXECUTE_READWRITE, &oldProtect));

		*((uint8_t*)address) = 0xE9;
		*((uint32_t*)(address+1)) = ((uint32_t)pfnFunc) - address - 5;
	}

	void NAKED_FUN Hook_IsCellCovered(void)
	{
		// if iid == 0, then isCovered (so that actually a hole is displayed; used for the Lightrail-Subway-transition)
		// else if alpha == 0xff, then notCovered  // there is a (base or) overlay texture, so mark it as not covered, so that terrain is rendered first
		// else continue (a semitransparent texture like a zoning decal?) (eventually returns notCovered, so that terrain is displayed as base)
		__asm {
			cmp dword ptr [ecx - 0x5], 0;  // iid == 0
			je isCovered;  // we claim it isCovered, so that the terrain is not rendered and the cell actually stays uncovered

			// cmp dword ptr [ecx - 0x5], TRANSPARENT_BASE_ID;
			// je notCovered;  // we claim it is not covered, so that the terrain is rendered

			cmp byte ptr [ecx + 0x6], 0xff;  // checks if alpha is 0xff
			je notCovered;  // originally: je isCovered

			push IsCellCovered_ContinueJump;  // continues loop to check remaining textures, eventually returns notCovered
			ret;

isCovered:
			push IsCellCovered_ReturnJump_IsCovered;
			ret;

notCovered:
			push IsCellCovered_ReturnJump_NotCovered;
			ret;
		}
	}

	void InstallPatches()
	{
		Logger& logger = Logger::GetInstance();
		try
		{
			InstallHook(IsCellCovered_InjectPoint, Hook_IsCellCovered);
			logger.WriteLine(LogLevel::Info, "Installed Transparent Texture Fix patch.");
		}
		catch (const wil::ResultException& e)
		{
			logger.WriteLineFormatted(LogLevel::Error, "Failed to install Transparent Texture Fix patch.\n%s", e.what());
		}
	}
}


class TransparentBaseDllDirector final : public cRZCOMDllDirector
{
public:

	TransparentBaseDllDirector()
	{
		std::filesystem::path dllFolderPath = GetDllFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error);
		logger.WriteLogFileHeader("Transparent Texture Fix DLL " PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const
	{
		return kTransparentBaseDllDirectorID;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		const uint16_t gameVersion = versionDetection.GetGameVersion();
		if (gameVersion == 641)
		{
			InstallPatches();
		}
		else
		{
			Logger& logger = Logger::GetInstance();
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Requires game version 641, found game version %d.",
				gameVersion);
		}
		return true;
	}

private:

	const SC4VersionDetection versionDetection;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static TransparentBaseDllDirector sDirector;
	return &sDirector;
}