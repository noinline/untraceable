/* clang-format off */
 /***
 *  
 *     /$$                                        /$$         /$$/$$                     
 *    | $$                                       |__/        | $|__/                     
 *    | $$$$$$$ /$$   /$$       /$$$$$$$  /$$$$$$ /$$/$$$$$$$| $$/$$/$$$$$$$  /$$$$$$    
 *    | $$__  $| $$  | $$      | $$__  $$/$$__  $| $| $$__  $| $| $| $$__  $$/$$__  $$   
 *    | $$  \ $| $$  | $$      | $$  \ $| $$  \ $| $| $$  \ $| $| $| $$  \ $| $$$$$$$$   
 *    | $$  | $| $$  | $$      | $$  | $| $$  | $| $| $$  | $| $| $| $$  | $| $$_____/   
 *    | $$$$$$$|  $$$$$$$      | $$  | $|  $$$$$$| $| $$  | $| $| $| $$  | $|  $$$$$$$   
 *    |_______/ \____  $$      |__/  |__/\______/|__|__/  |__|__|__|__/  |__/\_______/   
 *              /$$  | $$                                                                
 *             |  $$$$$$/                                                                
 *              \______/                                                                 
 *  
 ***/
/* clang-format on */
/*
 * The program executes a series of system commands to terminate specific
 * processes related to Epic Games and Fortnite, delete registry keys associated
 * with these games, reset the firewall, and remove various directories and
 * files from the system drive. This includes directories related to the Epic
 * Games Launcher, Fortnite, NVIDIA Corporation, EasyAntiCheat, Unreal Engine,
 * and others. It also cleans up temporary files, logs, and other unneeded
 * data.
 */
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <windows.h>

#define RESET				 "\u001b[0m"
#define COLOR(color_code, s) "\u001b[" color_code "m" s RESET

#define RED(s)	  "\u001b[1;31m" s RESET
#define GREEN(s)  "\u001b[1;32m" s RESET
#define YELLOW(s) "\u001b[1;33m" s RESET
#define CYAN(s)	  "\u001b[1;36m" s RESET
#define WHITE(s)  "\u001b[37;1m" s RESET

#define RED_MULTISTRING(s1, s2)	   printf(COLOR("1;31", s1) "%s", s2)
#define GREEN_MULTISTRING(s1, s2)  printf(COLOR("1;32", s1) "%s", s2)
#define YELLOW_MULTISTRING(s1, s2) printf(COLOR("1;33", s1) "%s", s2)
#define CYAN_MULTISTRING(s1, s2)   printf(COLOR("1;36", s1) "%s", s2)
#define WHITE_MULTISTRING(s1, s2)  printf(COLOR("37;1", s1) "%s", s2)

struct Assert
{
	DWORD error;
	Assert() : error(::GetLastError())
	{
	}
	auto check(void) -> bool
	{
		if (error != NULL)
		{
			::printf(RED("Error") " code: " WHITE("%lu") "\n ", error);
			return false;
		}
		return true;
	}
} assert;

struct Data
{
	HANDLE consoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screenBuffer = {};
	COORD topLeft = {0, 0};
	DWORD written = 0UL;
	int choice = 0;
	std::string number = "";
} data;

struct Processes
{
	int killedCount = NULL;
	FILE *pipe = nullptr;
	char buffer[128];
	std::string command = "";
	std::string result = "";
	std::string successMessage = "";
	std::vector<std::string> processes = {
		"epicgameslauncher.exe",
		"EpicWebHelper.exe",
		"FortniteClient-Win64-Shipping_EAC.exe",
		"FortniteClient-Win64-Shipping_BE.exe",
		"FortniteLauncher.exe",
		"FortniteClient-Win64-Shipping.exe",
		"EpicGamesLauncher.exe",
		"EasyAntiCheat.exe",
		"BEService.exe",
		"BEServices.exe",
		"BattleEye.exe"};
} proc;

auto __stdcall ClearConsole(void) -> decltype(void())
{
	::GetConsoleScreenBufferInfo(data.consoleHandle, &data.screenBuffer);
	::FillConsoleOutputCharacterA(data.consoleHandle, ' ',
								  data.screenBuffer.dwSize.X *
									  data.screenBuffer.dwSize.Y,
								  data.topLeft, &data.written);
	::FillConsoleOutputAttribute(
		data.consoleHandle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		data.screenBuffer.dwSize.X * data.screenBuffer.dwSize.Y, data.topLeft,
		&data.written);
	::SetConsoleCursorPosition(data.consoleHandle, data.topLeft);
	return;
}

auto __stdcall ShowOptions(void) -> decltype(void())
{
	::printf("[");
	GREEN_MULTISTRING("1", "]"
						   " "
						   "Simple clean\n");
	::printf("[");
	YELLOW_MULTISTRING("2", "]"
							" "
							"Deep clean\n");
	::printf("[");
	RED_MULTISTRING("3", "]"
						 " "
						 "Exit application\n");
	return;
}

auto __stdcall PrintLogo(void) -> decltype(void())
{
	/* clang-format off */
	::printf("                       /$$                                                      /$$       /$$                 /$$$$$$ \n"
	         "                      | $$                                                     | $$      | $$                 /$$__  $$\n");
	::printf(" /$$   /$$ /$$$$$$$  /$$$$$$    /$$$$$$  /$$$$$$   /$$$$$$$  /$$$$$$   /$$$$$$ | $$$$$$$ | $$  /$$$$$$    /$$|__/  \\ $$\n"
		     "| $$  | $$| $$__  $$|_  $$_/   /$$__  $$|____  $$ /$$_____/ /$$__  $$ |____  $$| $$__  $$| $$ /$$__  $$  |__/   /$$$$$/\n");
	::printf("| $$  | $$| $$  \\ $$  | $$    | $$  \\__/ /$$$$$$$| $$      | $$$$$$$$  /$$$$$$$| $$  \\ $$| $$| $$$$$$$$        |___  $$\n"
	         "| $$  | $$| $$  | $$  | $$ /$$| $$      /$$__  $$| $$      | $$_____/ /$$__  $$| $$  | $$| $$| $$_____/   /$$ /$$  \\ $$\n");
	::printf("|  $$$$$$/| $$  | $$  |  $$$$/| $$     |  $$$$$$$|  $$$$$$$|  $$$$$$$|  $$$$$$$| $$$$$$$/| $$|  $$$$$$$  |__/|  $$$$$$/\n"
		" \\______/ |__/  |__/   \\___/  |__/      \\_______/ \\_______/ \\_______/ \\_______/|_______/ |__/ \\_______/       \\______/ \n\n");
	/* clang-format on */
	return;
}

auto __stdcall TaskKill(void) -> decltype(void())
{
	proc.killedCount = NULL;

	for (const auto &process : proc.processes)
	{
		proc.command = "taskkill /f /im " + process + " 2>&1";
		proc.pipe = ::_popen(proc.command.c_str(), "r");
		if (!assert.check() && !proc.pipe)
			continue;

		while (!::feof(proc.pipe))
		{
			if (::fgets(proc.buffer, 128, proc.pipe) != NULL)
			{
				proc.result += proc.buffer;
			}
		}
		::_pclose(proc.pipe);

		proc.successMessage = "SUCCESS: The process \"" + process + "\"";

		if (proc.result.find(proc.successMessage) != std::string::npos)
		{
			proc.killedCount++;
		}
	}
	if (proc.killedCount > 0)
	{
		::printf(GREEN("Successfully") " "
									   "killed %i tasks!\n",
				 proc.killedCount);
	}
	else
	{
		::printf(GREEN("%i") " "
							 "tasks found! Proceeding...\n",
				 proc.killedCount);
	}

	return;
}

auto __stdcall PerformRegEditClean(void) -> decltype(void())
{
	/* clang-format off */
	::printf(YELLOW("Performing") " "
								  "registry keys cleaning!\n");
	::system("reg delete \"HKLM\\SYSTEM\\ControlSet001\\Services\\EpicOnlineServices\" /f > nul 2>&1");
    ::system("reg delete \"HKCU\\SOFTWARE\\Epic Games\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SOFTWARE\\Classes\\com.epicgames.launcher\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SYSTEM\\ControlSet001\\Services\\BEService\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SYSTEM\\ControlSet001\\Services\\BEDaisy\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SYSTEM\\ControlSet001\\Services\\EasyAntiCheat\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SYSTEM\\CurrentControlSet\\Services\\BEService\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SYSTEM\\CurrentControlSet\\Services\\BEDaisy\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SYSTEM\\CurrentControlSet\\Services\\EasyAntiCheat\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SOFTWARE\\WOW6432Node\\EasyAntiCheat\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SOFTWARE\\WOW6432Node\\Epic Games\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore\\microphone\\NonPackaged\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SOFTWARE\\Microsoft\\RADAR\\HeapLeakDetection\\DiagnosedApplications\" /f > nul 2>&1");
    ::system("reg delete \"HKCU\\Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\com.epicgames.launcher\" /f > nul 2>&1");
    ::system("reg delete \"HKCR\\com.epicgames.eos\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SOFTWARE\\Microsoft\\RADAR\\HeapLeakDetection\\DiagnosedApplications\" /f > nul 2>&1");
    ::system("reg delete \"HKLM\\SOFTWARE\\EpicGames\" /f > nul 2>&1");
    ::system("reg delete \"HKEY_USERS\\S-1-5-18\\Software\\Epic Games\" /f > nul 2>&1");
	::printf(GREEN("Successfully") " "
								   "cleaned registry keys!\n");
	/* clang-format on */
	return;
}

auto __stdcall PerformSimpleDirectoriesClean(void) -> decltype(void())
{
	/* clang-format off */
	::printf(YELLOW("Performing") " "
								  "directories cleaning!\n");
    ::system("RMDIR /S /Q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\EpicGamesLauncher\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\FortniteGame\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\Epic Games\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\NVIDIA Corporation\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Users\\%username%\\AppData\\Roaming\\EasyAntiCheat\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Users\\All Users\\Epic\\EpicGamesLauncher\\Data\\EMS\\current\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\UnrealEngine\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\UnrealEngineLauncher\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\ProgramData\\Epic\\EpicOnlineServices\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\ProgramData\\Epic\\EpicGamesLauncher\\Data\\EMS\\current\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Program Files (x86)\\Epic Games\\Epic Online Services\\service\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Program Files\\Epic Games\\Fortnite\\FortniteGame\\Binaries\\Win64\\Shared Files\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Program Files (x86)\\Common Files\\BattlEye\" > nul 2>&1");
    ::system("RMDIR /S /Q \"%systemdrive%\\Program Files (x86)\\EasyAntiCheat\" > nul 2>&1");
    ::system("RMDIR /s /Q \"%systemdrive%\\$Recycle.bin\" > nul 2>&1");
	::system("el /q \"%systemdrive%\\ProgramData\\Microsoft\\Windows\\WER\\Temp\\\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\ProgramData\\Microsoft\\Windows\\WER\\Temp\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\Temp\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Users\\%username%\\AppData\\Local\\Temp\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Windows\\Temp\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Windows\\Temp\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Windows\\TEMP\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Windows\\TEMP\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Windows\\temp\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Windows\\temp\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Program Files (x86)\\Temp\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Program Files (x86)\\Temp\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Windows\\Logs\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Windows\\Logs\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\D3DSCache\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Users\\%username%\\AppData\\Local\\D3DSCache\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\CrashReportClient\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Users\\%username%\\AppData\\Local\\CrashReportClient\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Windows\\Prefetch\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Windows\\Prefetch\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Users\\%username%\\Recent\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Users\\%username%\\Recent\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\AMD\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Users\\%username%\\AppData\\Local\\AMD\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\AMD_Common\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Users\\%username%\\AppData\\Local\\AMD_Common\\*\") do @rd /s /q \"%x\" > nul 2>&1");
    ::system("del /q \"%systemdrive%\\Users\\%username%\\AppData\\Local\\Microsoft\\Feeds\\*\" > nul 2>&1");
    ::system("for /d %x in (\"%systemdrive%\\Users\\%username%\\AppData\\Local\\Microsoft\\Feeds\\*\") do @rd /s /q \"%x\" > nul 2>&1");
	::printf(GREEN("Successfully") " "
								   "cleaned directories!\n");
	/* clang-format on */
	return;
}

auto __stdcall PerformSimpleClean(void) -> decltype(void())
{
	/* clang-format off */
	/* kill everything that prevents us from cleaning registry keys and other files */

    ::TaskKill();
	if (!assert.check()) return;

	/* delete registry keys */

	::PerformRegEditClean();
	if (!assert.check()) return;

	/* reset our firewall */

    ::system("netsh advfirewall reset > nul 2>&1");
	if (!assert.check()) return;

	/* clean the stuff */

	::PerformSimpleDirectoriesClean();
	if (!assert.check()) return;

	/* clang-format on */
	return;
}

auto __stdcall PerformDeepDirectoriesClean(void) -> decltype(void())
{
	/* clang-format off */
	::printf(YELLOW("Performing") " "
								  "directories deep cleaning!\n");
    ::system("del /f /s /q %systemdrive%\\Users\\%username%\\AppData\\Local\\Microsoft\\Windows\\INetCookies\\ > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\Users\\%username%\\AppData\\Local\\Microsoft\\Windows\\History\\ > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\Users\\%username%\\AppData\\Local\\Microsoft\\Windows\\INetCache\\ > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\Users\\%username%\\AppData\\Local\\Temp\\ > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\Windows\\Temp\\ > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\Windows\\Prefetch\\ > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\Temp\\ > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.etl > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.log > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.tmp > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.old > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.bak > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.bac > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.bup > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.chk > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.dmp > nul 2>&1");
    ::system("del /f /s /q %systemdrive%\\*.temp > nul 2>&1");
	::printf(GREEN("Successfully") " "
								   "deep cleaned directories!\n\n");
	/* clang-format on */
	return;
}

auto __stdcall PerformDeepClean(void) -> decltype(void())
{
	/* clang-format off */
	/* kill everything that prevents us from cleaning registry keys and other files */

    ::TaskKill();
	if (!assert.check()) return;

	/* delete registry keys */

    ::PerformRegEditClean();
	if (!assert.check()) return;

	/* reset our firewall */

    ::system("netsh advfirewall reset > nul 2>&1");
	if (!assert.check()) return;

	/* clean the stuff */

	::PerformDeepDirectoriesClean();
	if (!assert.check()) return;

	/* clang-format on */
	return;
}

[[maybe_unused]] [[__maybevalid]] __declspec(nothrow) __declspec(
	noinline) auto __stdcall main(void) -> decltype(2)
{
	::SetConsoleTitleA("untraceable");
	do
	{
		::ClearConsole();
		::PrintLogo();
		::ShowOptions();
		std::getline(std::cin, data.number);
		data.choice = std::stoi(data.number);
		switch (data.choice)
		{
		case 1:
			::ClearConsole();
			::PrintLogo();
			::printf("Cleaning your filthy PC :3c\n\n");
			::PerformSimpleClean();
			::Sleep(2000);
			break;
		case 2:
			::ClearConsole();
			::PrintLogo();
			::printf("Deep cleaning your filthy PC :3c\n\n");
			::PerformDeepClean();
			::Sleep(2000);
			break;
		case 3:
			::ClearConsole();
			::PrintLogo();
			::printf("But i wanted to clean your filthy PC :(\n\n");
			break;
		default:
			::ClearConsole();
			::PrintLogo();
			RED_MULTISTRING("Grrrr",
							"!!! Press a " GREEN("valid") " number!\n\n");
			::Sleep(2000);
		}
	} while (data.choice != 3);

	::Sleep(1500);
	::system("CLS");
	return EXIT_SUCCESS;
}