#define NOMINMAX
#include <windows.h>
#include "resource.h"

#include <iostream>
#include <vector>
#include <processthreadsapi.h>
#include <Psapi.h>
#include <fcntl.h>
#include <io.h>
#include <tchar.h>
#include <thread>
#include <chrono>
#include <cwctype>
#include <string>
#include <atomic>
#include <algorithm>  
#include <tlhelp32.h>
#include <d3d11.h>
#include <shlwapi.h>
#include <random>

#include "imgui-files/imgui.h"
#include "imgui-files/imgui_impl_dx11.h"
#include "imgui-files/imgui_impl_win32.h"
#include "imgui-files/json.hpp"

#include "miniz.h"

#include <wininet.h>
#include <comdef.h>
#include <shlobj.h>
#include <condition_variable>
#include <fstream>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <format>
#include <unordered_map>
#include <math.h>
#include <memory.h>
#include <synchapi.h>
#include <dwmapi.h>
#include <variant>
#include <algorithm>
#include <shellscalingapi.h>

// Library for HTTP (To get version data from my github page)
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Shcore.lib")

using json = nlohmann::json;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct Theme {
	std::string name;
	ImVec4 bg_dark;
	ImVec4 bg_medium;
	ImVec4 bg_light;
	ImVec4 accent_primary;
	ImVec4 accent_secondary;
	ImVec4 text_primary;
	ImVec4 text_secondary;
	ImVec4 success_color;
	ImVec4 warning_color;
	ImVec4 error_color;
	ImVec4 border_color;
	float window_rounding;
	float frame_rounding;
	float button_rounding;
};

// Default themes
std::vector<Theme> defaultthemes = {
	{"Modern Dark", ImVec4(0.08f, 0.08f, 0.12f, 1.0f), ImVec4(0.12f, 0.12f, 0.16f, 1.0f),
	 ImVec4(0.16f, 0.16f, 0.20f, 1.0f), ImVec4(0.20f, 0.60f, 1.0f, 1.0f),
	 ImVec4(0.15f, 0.50f, 0.85f, 1.0f), ImVec4(0.95f, 0.95f, 0.95f, 1.0f),
	 ImVec4(0.70f, 0.70f, 0.70f, 1.0f), ImVec4(0.20f, 0.80f, 0.40f, 1.0f),
	 ImVec4(0.90f, 0.60f, 0.20f, 1.0f), ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
	 ImVec4(0.20f, 0.20f, 0.25f, 1.0f), 8.0f, 4.0f, 4.0f},
	{"Cyberpunk", ImVec4(0.05f, 0.05f, 0.10f, 1.0f), ImVec4(0.10f, 0.10f, 0.15f, 1.0f),
	 ImVec4(0.15f, 0.15f, 0.20f, 1.0f), ImVec4(0.00f, 0.80f, 1.0f, 1.0f),
	 ImVec4(0.00f, 0.60f, 0.80f, 1.0f), ImVec4(0.90f, 0.90f, 1.0f, 1.0f),
	 ImVec4(0.60f, 0.60f, 0.80f, 1.0f), ImVec4(0.00f, 1.0f, 0.50f, 1.0f),
	 ImVec4(1.0f, 0.50f, 0.00f, 1.0f), ImVec4(1.0f, 0.20f, 0.40f, 1.0f),
	 ImVec4(0.00f, 0.40f, 0.60f, 1.0f), 6.0f, 3.0f, 3.0f},
	{"Forest Green", ImVec4(0.05f, 0.12f, 0.08f, 1.0f), ImVec4(0.08f, 0.16f, 0.12f, 1.0f),
	 ImVec4(0.12f, 0.20f, 0.16f, 1.0f), ImVec4(0.20f, 0.80f, 0.40f, 1.0f),
	 ImVec4(0.15f, 0.60f, 0.30f, 1.0f), ImVec4(0.90f, 0.95f, 0.90f, 1.0f),
	 ImVec4(0.70f, 0.80f, 0.70f, 1.0f), ImVec4(0.30f, 0.90f, 0.50f, 1.0f),
	 ImVec4(0.90f, 0.70f, 0.20f, 1.0f), ImVec4(0.80f, 0.30f, 0.30f, 1.0f),
	 ImVec4(0.15f, 0.30f, 0.20f, 1.0f), 10.0f, 5.0f, 5.0f},
	{"Sunset Orange", ImVec4(0.12f, 0.08f, 0.05f, 1.0f), ImVec4(0.16f, 0.12f, 0.08f, 1.0f),
	 ImVec4(0.20f, 0.16f, 0.12f, 1.0f), ImVec4(1.0f, 0.50f, 0.20f, 1.0f),
	 ImVec4(0.80f, 0.40f, 0.15f, 1.0f), ImVec4(1.0f, 0.95f, 0.90f, 1.0f),
	 ImVec4(0.80f, 0.70f, 0.60f, 1.0f), ImVec4(0.40f, 0.80f, 0.30f, 1.0f),
	 ImVec4(1.0f, 0.70f, 0.20f, 1.0f), ImVec4(0.90f, 0.30f, 0.30f, 1.0f),
	 ImVec4(0.30f, 0.20f, 0.15f, 1.0f), 12.0f, 6.0f, 6.0f},
	{"Purple Haze", ImVec4(0.10f, 0.05f, 0.15f, 1.0f), ImVec4(0.15f, 0.10f, 0.20f, 1.0f),
	 ImVec4(0.20f, 0.15f, 0.25f, 1.0f), ImVec4(0.80f, 0.40f, 1.0f, 1.0f),
	 ImVec4(0.60f, 0.30f, 0.80f, 1.0f), ImVec4(0.95f, 0.90f, 1.0f, 1.0f),
	 ImVec4(0.70f, 0.60f, 0.80f, 1.0f), ImVec4(0.40f, 0.80f, 0.60f, 1.0f),
	 ImVec4(1.0f, 0.60f, 0.40f, 1.0f), ImVec4(0.90f, 0.30f, 0.50f, 1.0f),
	 ImVec4(0.25f, 0.15f, 0.30f, 1.0f), 8.0f, 4.0f, 4.0f}};

std::vector<Theme> themes = defaultthemes;

// Current theme index and custom theme
int current_theme_index = 0;
Theme custom_theme = themes[0]; // Initialize with first theme
bool show_theme_editor = false;
bool theme_modified = false;

// DirectX11 Variables
ID3D11Device *g_pd3dDevice = NULL;
ID3D11DeviceContext *g_pd3dDeviceContext = NULL;
IDXGISwapChain *g_pSwapChain = NULL;
ID3D11RenderTargetView *g_mainRenderTargetView = NULL;

// Forward Declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void RenderSectionSettings(int section);
static void RenderThemeEditor();
static void ApplyTheme(const Theme &theme);
// TO PUT IN A KEYBOARD KEY, GO TO https://www.millisecond.com/support/docs/current/html/language/scancodes.htm
// Convert the scancode into hexadecimal before putting it into the HoldKey or ReleaseKey functions
// Ex: E = 18 = 0x12 = HoldKey(0x12)

// If you want to create custom HOTKEYS for stuff that isn't an alphabet/function key, go to https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes and get your virtual key code or value

std::string text = "/e dance2"; // Custom chat text

std::atomic<bool> isdesyncloop(false); // Set the variable used for the threads
std::atomic<bool> isspeed(false);
std::atomic<bool> isHHJ(false);
std::atomic<bool> isspamloop(false);
std::atomic<bool> isitemloop(false);
std::atomic<bool> iswallwalkloop(false);
std::atomic<bool> isbhoploop(false);
std::atomic<bool> isafk(false);
std::atomic<bool> iswallhopthread(false);
std::atomic<bool> ispresskeythread(false);

std::atomic<unsigned int> RobloxFPS(120);

std::mutex renderMutex;
std::condition_variable renderCondVar;
bool renderFlag = false;
bool running = true;
HWND hwnd;

std::atomic<bool> g_isVk_BunnyhopHeldDown(false);
HHOOK g_keyboardHook = NULL;

const DWORD SCAN_CODE_FLAGS = KEYEVENTF_SCANCODE;
const DWORD RELEASE_FLAGS = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

INPUT inputkey = {};
INPUT inputhold = {};
INPUT inputrelease = {};

// Translate all VK's into variables so it's less annoying to debug it
unsigned int scancode_shift = 0x2A;
unsigned int vk_f5 = VK_F5;
unsigned int vk_f6 = VK_F6;
unsigned int vk_f8 = VK_F8;
unsigned int vk_mbutton = VK_MBUTTON;
unsigned int vk_xbutton1 = VK_XBUTTON1;
unsigned int vk_xbutton2 = VK_XBUTTON2;
unsigned int vk_spamkey = VK_LBUTTON;
unsigned int vk_clipkey = VK_F3;
unsigned int vk_wallkey = VK_F1;
unsigned int vk_laughkey = VK_F7;
unsigned int vk_zkey = VkKeyScanEx('Z', GetKeyboardLayout(0)) & 0xFF; // Use this for alphabet keys so it works across layouts
unsigned int vk_dkey = VkKeyScanEx('D', GetKeyboardLayout(0)) & 0xFF;
unsigned int vk_xkey = VkKeyScanEx('X', GetKeyboardLayout(0)) & 0xFF;
unsigned int vk_wkey = VkKeyScanEx('W', GetKeyboardLayout(0)) & 0xFF;
unsigned int vk_bouncekey = VkKeyScanEx('C', GetKeyboardLayout(0)) & 0xFF;
unsigned int vk_leftbracket = MapVirtualKey(0x1A, MAPVK_VSC_TO_VK);
unsigned int vk_bunnyhopkey = MapVirtualKey(0x39, MAPVK_VSC_TO_VK);

//////////////////////////
unsigned int vk_Lagkey = VK_F2;
////////////////////////// 


// ADD KEYBIND VARIABLES FOR EACH SECTION
static const std::unordered_map<unsigned int, unsigned int *> section_to_key = {
	{0, &vk_mbutton},    {1, &vk_f5},           {2, &vk_xbutton1}, {3, &vk_xkey},
	{4, &vk_f8},         {5, &vk_zkey},         {6, &vk_xbutton2}, {7, &vk_f6},
	{8, &vk_clipkey},    {9, &vk_laughkey},     {10, &vk_wallkey}, {11, &vk_leftbracket},
	{12, &vk_bouncekey}, {13, &vk_bunnyhopkey}, {14, &vk_Lagkey}};

const std::string G_SETTINGS_FILEPATH = "RMCSettings.json";
static std::string G_CURRENTLY_LOADED_PROFILE_NAME = "";

const std::string METADATA_KEY = "_metadata";
const std::string LAST_ACTIVE_PROFILE_KEY = "last_active_profile";

// Lookup Table - Backup to avoid displaying Hex if unneccessary
std::unordered_map<int, std::string> vkToString = {
    {VK_LBUTTON, "VK_LBUTTON"},
    {VK_RBUTTON, "VK_RBUTTON"},
    {VK_CANCEL, "VK_CANCEL"},
    {VK_MBUTTON, "VK_MBUTTON"},
    {VK_XBUTTON1, "VK_XBUTTON1"},
    {VK_XBUTTON2, "VK_XBUTTON2"},
    {VK_BACK, "VK_BACK"},
    {VK_TAB, "VK_TAB"},
    {VK_CLEAR, "VK_CLEAR"},
    {VK_RETURN, "VK_RETURN"},
    {VK_SHIFT, "VK_SHIFT"},
    {VK_CONTROL, "VK_CONTROL"},
    {VK_MENU, "VK_MENU"},
    {VK_PAUSE, "VK_PAUSE"},
    {VK_CAPITAL, "VK_CAPITAL"},
    {VK_ESCAPE, "VK_ESCAPE"},
    {VK_SPACE, "VK_SPACE"},
    {VK_PRIOR, "VK_PRIOR"},
    {VK_NEXT, "VK_NEXT"},
    {VK_END, "VK_END"},
    {VK_HOME, "VK_HOME"},
    {VK_LEFT, "VK_LEFT"},
    {VK_UP, "VK_UP"},
    {VK_RIGHT, "VK_RIGHT"},
    {VK_DOWN, "VK_DOWN"},
    {VK_SELECT, "VK_SELECT"},
    {VK_PRINT, "VK_PRINT"},
    {VK_EXECUTE, "VK_EXECUTE"},
    {VK_SNAPSHOT, "VK_SNAPSHOT"},
    {VK_INSERT, "VK_INSERT"},
    {VK_DELETE, "VK_DELETE"},
    {VK_HELP, "VK_HELP"},
    {VK_LWIN, "VK_LWIN"},
    {VK_RWIN, "VK_RWIN"},
    {VK_NUMPAD0, "VK_NUMPAD0"},
    {VK_NUMPAD1, "VK_NUMPAD1"},
    {VK_NUMPAD2, "VK_NUMPAD2"},
    {VK_NUMPAD3, "VK_NUMPAD3"},
    {VK_NUMPAD4, "VK_NUMPAD4"},
    {VK_NUMPAD5, "VK_NUMPAD5"},
    {VK_NUMPAD6, "VK_NUMPAD6"},
    {VK_NUMPAD7, "VK_NUMPAD7"},
    {VK_NUMPAD8, "VK_NUMPAD8"},
    {VK_NUMPAD9, "VK_NUMPAD9"},
    {VK_MULTIPLY, "VK_MULTIPLY"},
    {VK_ADD, "VK_ADD"},
    {VK_SEPARATOR, "VK_SEPARATOR"},
    {VK_SUBTRACT, "VK_SUBTRACT"},
    {VK_DECIMAL, "VK_DECIMAL"},
    {VK_DIVIDE, "VK_DIVIDE"},
    {VK_LSHIFT, "VK_LSHIFT"},
    {VK_RSHIFT, "VK_RSHIFT"},
    {VK_LCONTROL, "VK_LCONTROL"},
    {VK_RCONTROL, "VK_RCONTROL"},
    {VK_LMENU, "VK_LMENU"},
    {VK_RMENU, "VK_RMENU"},
    {VK_OEM_PLUS, "VK_OEM_PLUS"},
    {VK_OEM_COMMA, "VK_OEM_COMMA"},
    {VK_OEM_MINUS, "VK_OEM_MINUS"},
    {VK_OEM_PERIOD, "VK_OEM_PERIOD"},
    {VK_OEM_2, "VK_OEM_2"},
    {VK_OEM_3, "VK_OEM_3"},
    {VK_OEM_4, "VK_OEM_4"},
    {VK_OEM_5, "VK_OEM_5"},
    {VK_OEM_6, "VK_OEM_6"},
    {VK_OEM_7, "VK_OEM_7"},
    {VK_OEM_8, "VK_OEM_8"},
    {VK_OEM_102, "VK_OEM_102"},
	{0x0, "RESTART PC"}
};

// Window and UI settings
int screen_width = GetSystemMetrics(SM_CXSCREEN) / 1.5;
int screen_height = GetSystemMetrics(SM_CYSCREEN) / 1.5 + 10;
int selected_dropdown = 0;
std::string KeyButtonText = "Click to Bind Key";
std::string KeyButtonTextalt = "Click to Bind Key";
std::string chatkey = "/";

// Keybind and macro settings
char settingsBuffer[256] = "RobloxPlayerBeta.exe"; // Default value for the textbox
char KeyBuffer[256] = "None";
char KeyBufferalt[256] = "None";
char KeyBufferhuman[256] = "None";
char KeyBufferhumanalt[256] = "None";
char ItemDesyncSlot[256] = "5";
char ItemSpeedSlot[256] = "3";
char ItemClipSlot[256] = "7";
char ItemClipDelay[256] = "34";
char BunnyHopDelayChar[256] = "10";
char WallhopPixels[256] = "300";
char WallhopDelayChar[256] = "17";
char WallhopBonusDelayChar[256] = "0";
char WallhopDegrees[256] = "150";
char SpamDelay[256] = "20";
char RobloxSensValue[256] = "0.5";
char RobloxPixelValueChar[256] = "716";
char RobloxWallWalkValueChar[256] = "-94";
char RobloxWallWalkValueDelayChar[256] = "72720";
char ChatKeyChar[2] = "/";
char CustomTextChar[256] = "";
char RobloxFPSChar[256] = "60";
char AntiAFKTimeChar[256] = "15";
char PressKeyDelayChar[256] = "16";


// Toggles and switches
bool macrotoggled = true;
bool shiftswitch = false;
bool unequiptoggle = false;
bool camfixtoggle = false;
bool wallhopswitch = false;
bool wallwalktoggleside = false;
bool wallhopcamfix = false;
bool chatoverride = true;
bool toggle_jump = true;
bool toggle_flick = true;
bool fasthhj = false;
bool wallesslhjswitch = false;
bool autotoggle = false;
bool isspeedswitch = false;
bool isfreezeswitch = false;
bool freezeoutsideroblox = true;
bool iswallwalkswitch = false;
bool isspamswitch = false;
bool isitemclipswitch = false;
bool antiafktoggle = true;
bool bouncesidetoggle = false;
bool bouncerealignsideways = true;
bool bounceautohold = true;
bool laughmoveswitch = false;
bool takeallprocessids = false;
bool ontoptoggle = false;
bool bunnyhoptoggled = false;
bool bunnyhopsmart = true;
bool presskeyinroblox = false;
bool unequipinroblox = false;
bool shortdescriptions = false;

//////////////////////////
bool islagswitch = false;
bool autoflickdir = true;
bool lagoverlayswitch = false;
bool lastpressedAD = false;
////////////////////////// 

// Section toggles and order
constexpr int section_amounts = 15;
bool section_toggles[15] = {true, true,  true,  true,  true,  false, true, true, true, false, false, false, false, false, true};
int section_order[15] = {0, 14, 1, 2, 3, 4, 5, 6, 13, 7, 8, 9, 10, 11, 12};

// Numeric settings
int wallhop_dx = 300;
int wallhop_dy = -300;
int speed_strengthx = 959;
int wallwalk_strengthx = 94;
int speedoffsetx = 0;
int speed_strengthy = -959;
int wallwalk_strengthy = -94;
int speedoffsety = 0;
int speed_slot = 3;
int desync_slot = 5;
int clip_slot = 7;
int clip_delay = 30;
int BunnyHopDelay = 10;
int RobloxWallWalkValueDelay = 72720;
float spam_delay = 20.0f;
float maxfreezetime = 9.00f;
int maxfreezeoverride = 50;
int real_delay = 1000;
int RobloxPixelValue = 716;
int RobloxWallWalkValue = -94;
int WallhopDelay = 17;
int WallhopBonusDelay = 0;
int AntiAFKTime = 15;
int display_scale = 100;
int PressKeyDelay = 16;
int WindowPosX = 0;
int WindowPosY = 0;

//////////////////////////
float maxlagtime = 9.00f;
int maxlagoverride = 10;
int ovarlayverticaloffset = 50;
int ovarlayhorisontaloffset = 50;
int g_textSize = 24;
////////////////////////// 


// Dropdown options
const char* optionsforoffset[] = {"/e dance2", "/e laugh", "/e cheer"};

// Window and UI state
RECT screen_rect;
int dragged_section = -1; // -1 means no section is being dragged
static int selected_section = -1;

// Process and timing state
bool processFound = false; // Initialize as no process found
bool done = false;
bool bindingMode = false;
bool bindingModealt = false;
bool notbinding = true;
bool wallhopupdate = false;
bool UserOutdated = false;
static bool wasMButtonPressed = false;

//////////////////////////
static bool wasLagPressed = false;
static bool lastLag = false;
////////////////////////// 

// Timing and chrono
auto rebindtime = std::chrono::steady_clock::now();
auto suspendStartTime = std::chrono::steady_clock::time_point();

//////////////////////////
auto lagStartTime = std::chrono::steady_clock::time_point();
////////////////////////// 

// Previous values (used for comparisons)
static float PreviousSensValue = -1.0f;
static float PreviousWallWalkSide = 0;
static float PreviousWallWalkValue = 0.5f;
static float windowOpacityPercent = 100.0f;


typedef LONG(NTAPI *NtSuspendProcess)(HANDLE ProcessHandle);
typedef LONG(NTAPI *NtResumeProcess)(HANDLE ProcessHandle);

//////////////////////////

HWND g_overlay = nullptr;
std::wstring g_overlayText = L"LagSwitch OFF";
COLORREF g_textColor = RGB(255, 0, 0); // default green

// Forward declaration
void CreateOverlay(HWND parent);
void DestroyOverlay();
void UpdateOverlayPosition();

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rc;
		GetClientRect(hwnd, &rc);

		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(hdc, &rc, hBrush);
		DeleteObject(hBrush);

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, g_textColor);

		int fontHeight = -g_textSize;
		HFONT hFont = CreateFont(fontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
					 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

		HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

		// Calculate bounding box for text
		RECT textRect = {10, 10, rc.right - 10, rc.bottom - 10};
		DrawTextW(hdc, g_overlayText.c_str(), -1, &textRect,
			  DT_LEFT | DT_TOP | DT_CALCRECT);

		// Optional: Expand window or clip region based on textRect if needed

		// Actually draw text
		DrawTextW(hdc, g_overlayText.c_str(), -1, &textRect, DT_LEFT | DT_TOP);

		SelectObject(hdc, oldFont);
		DeleteObject(hFont);

		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND GetWindowFromProcess(const std::vector<HANDLE> &pids)
{
	HWND result = nullptr;

	struct EnumContext {
		const std::vector<HANDLE> *pids;
		HWND *result;
	} context = {&pids, &result};

	EnumWindows(
		[](HWND hwnd, LPARAM lParam) -> BOOL {
			auto *ctx = reinterpret_cast<EnumContext *>(lParam);
			DWORD winPid = 0;
			GetWindowThreadProcessId(hwnd, &winPid);

			for (const HANDLE &h : *ctx->pids) {
				DWORD pid = GetProcessId(h);
				if (pid == winPid && IsWindowVisible(hwnd)) {
					*ctx->result = hwnd;
					return FALSE; // stop enumeration
				}
			}
			return TRUE; // continue
		},
		reinterpret_cast<LPARAM>(&context));

	return result;
}

void CreateOverlay(HINSTANCE hInstance)
{
	if (g_overlay)
		return; // already created

	const wchar_t CLASS_NAME[] = L"OverlayClass";

	static bool classRegistered = false;
	if (!classRegistered) {
		WNDCLASS wc = {};
		wc.lpfnWndProc = OverlayWndProc; // your window proc function
		wc.hInstance = hInstance;
		wc.lpszClassName = CLASS_NAME;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = nullptr;

		if (!RegisterClass(&wc)) {
			DWORD err = GetLastError();
			if (err != ERROR_CLASS_ALREADY_EXISTS) // ignore if class already exists
			{
				wchar_t buffer[256];
				swprintf(buffer, 256, L"RegisterClass failed with error code %lu",
					 err);
				MessageBox(nullptr, buffer, L"Error", MB_OK | MB_ICONERROR);
				return;
			}
		}
		classRegistered = true;
	}

	// Calculate center of screen for initial position
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int width = screenWidth;
	int height = screenHeight;
	int x = -std::numeric_limits<int>::max();
	int y = -std::numeric_limits<int>::max();

	g_overlay = CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT, CLASS_NAME,
				   L"", WS_POPUP, x, y, width, height, nullptr, nullptr, hInstance,
				   nullptr);

	if (!g_overlay) {
		DWORD err = GetLastError();
		wchar_t buffer[256];
		swprintf(buffer, 256, L"CreateWindowEx failed with error code %lu", err);
		MessageBox(nullptr, buffer, L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	SetLayeredWindowAttributes(g_overlay, RGB(0, 0, 0), 255, LWA_COLORKEY);

	ShowWindow(g_overlay, SW_SHOW);
	UpdateWindow(g_overlay);
}

void DestroyOverlay()
{
	if (g_overlay) {
		DestroyWindow(g_overlay);
		g_overlay = nullptr;
	}
}

void UpdateOverlayToWindow(HWND overlayHwnd, HWND targetHwnd)
{
	if (!overlayHwnd || !targetHwnd)
		return;

	RECT rect;
	if (GetWindowRect(targetHwnd, &rect)) {
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		SetWindowPos(overlayHwnd,
			     HWND_TOPMOST, // insert after Roblox
			     rect.left + ovarlayhorisontaloffset, rect.top + ovarlayverticaloffset,
			     width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
	}
}

const std::wstring FIREWALL_RULE_NAME = L"Roblox_Block";

std::wstring GetExePathFromHandle(HANDLE hProcess)
{
	wchar_t exePath[MAX_PATH];
	DWORD size = MAX_PATH;

	if (QueryFullProcessImageNameW(hProcess, 0, exePath, &size)) {
		return std::wstring(exePath);
	} else {
		return L"";
	}
}

bool RunCommand(const std::wstring &cmd)
{
	STARTUPINFOW si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	// CreateProcess requires writable buffer, so copy cmd to wchar_t array
	std::vector<wchar_t> cmdBuffer(cmd.begin(), cmd.end());
	cmdBuffer.push_back(0);

	if (!CreateProcessW(nullptr, cmdBuffer.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
			    nullptr, nullptr, &si, &pi)) {
		return false;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}

bool BlockFirewallRule(const std::wstring &exePath)
{
	std::wstringstream cmd;
	cmd << L"netsh advfirewall firewall add rule " << L"name=\"" << FIREWALL_RULE_NAME << L"\" "
	    << L"dir=out action=block program=\"" << exePath << L"\" enable=yes";
	return RunCommand(cmd.str());
}

bool UnblockFirewallRule()
{
	std::wstringstream cmd;
	cmd << L"netsh advfirewall firewall delete rule name=\"" << FIREWALL_RULE_NAME << L"\"";
	return RunCommand(cmd.str());
}

static void LagOrUnLagProcess(const std::vector<HANDLE> &pids, bool suspend)
{
	for (HANDLE pid : pids) {

		std::wstring exePath = GetExePathFromHandle(pid);
		if (suspend) {
			BlockFirewallRule(exePath);
		} else {
			UnblockFirewallRule();
		}
	}
}

////////////////////////// 


// Helper function to suspend or resume a process
static void SuspendOrResumeProcess(NtSuspendProcess pfnSuspend, NtResumeProcess pfnResume, const std::vector<HANDLE>& pids, bool suspend)
{
    for (HANDLE pid : pids)
    {
        if (suspend)
        {
		pfnSuspend(pid);
        }
        else
        {
		pfnResume(pid);
        }
    }
}


static INPUT createInput()
{
	INPUT inputkey = {};
	inputkey.type = INPUT_KEYBOARD;
	return inputkey;
}

// Hold a key down, self explanatory
static void HoldKey(WORD scanCode)
{
	INPUT input = {};
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = scanCode;
	input.ki.dwFlags = SCAN_CODE_FLAGS;

	SendInput(1, &input, sizeof(INPUT));
}

static void ReleaseKey(WORD scanCode)
{
	INPUT input = {};
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = scanCode;
	input.ki.dwFlags = RELEASE_FLAGS;

	SendInput(1, &input, sizeof(INPUT));
}

// Use these only if the input comes from a bind

static void HoldKeyBinded(WORD Vk_key)
{
    INPUT input = {};

    // Check if Vk corresponds to M1, M2, or M3
    if (Vk_key == VK_LBUTTON || Vk_key == VK_RBUTTON || Vk_key == VK_MBUTTON)
    {
        input.type = INPUT_MOUSE;
        switch (Vk_key)
        {
        case VK_LBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            break;
        case VK_RBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
            break;
        case VK_MBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
            break;
        }
    }
    else
    {
		Vk_key = MapVirtualKeyEx(Vk_key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = Vk_key;
        input.ki.dwFlags = SCAN_CODE_FLAGS;
    }

    SendInput(1, &input, sizeof(INPUT));
}

static void ReleaseKeyBinded(WORD Vk_key)
{
    INPUT input = {};

    // Check if Vk corresponds to M1, M2, or M3
    if (Vk_key == VK_LBUTTON || Vk_key == VK_RBUTTON || Vk_key == VK_MBUTTON)
    {
        input.type = INPUT_MOUSE;
        switch (Vk_key)
        {
        case VK_LBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            break;
        case VK_RBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
            break;
        case VK_MBUTTON:
            input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            break;
        }
    }
    else
    {
		Vk_key = MapVirtualKeyEx(Vk_key, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = Vk_key;
        input.ki.dwFlags = RELEASE_FLAGS;
    }

    SendInput(1, &input, sizeof(INPUT));
}


// Move your mouse to any coordinate
static void MoveMouse(int dx, int dy)
{
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dx = (dx * display_scale) / 100;
	input.mi.dy = (dy * display_scale) / 100;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;

	SendInput(1, &input, sizeof(INPUT));
}

static void PasteText(const std::string &text)
{
	for (char c : text) {
        // Key down event
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 0;
        input.ki.wScan = c;
        input.ki.dwFlags = KEYEVENTF_UNICODE;  // Unicode key down
        SendInput(1, &input, sizeof(INPUT));

        // Key up event
        input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;  // Unicode key up
        SendInput(1, &input, sizeof(INPUT));
    }
}

// This is ran in a separate thread to avoid interfering with other functions

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        // Reset AFK on any key activity
        isafk.store(false, std::memory_order_relaxed);
        
        const KBDLLHOOKSTRUCT* pkbhs = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        if (pkbhs->vkCode == vk_bunnyhopkey) {
            if ((pkbhs->flags & LLKHF_INJECTED) == 0) {
                // Use relaxed for the bunnyhop flag
                g_isVk_BunnyhopHeldDown.store((wParam & 1) == 0, std::memory_order_relaxed);
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

static void KeyboardHookThread() 
{
	HINSTANCE hMod = GetModuleHandle(NULL);
	g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hMod, 0);
	// Message handler for this thread for the keyboard hook only
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	UnhookWindowsHookEx(g_keyboardHook);
}

static void ItemDesyncLoop()
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	while (true) { // Efficient variable checking method
		while (!isdesyncloop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (macrotoggled && notbinding && section_toggles[1]) {
			HoldKey(desync_slot + 1);
			ReleaseKey(desync_slot + 1);
			HoldKey(desync_slot + 1);
			ReleaseKey(desync_slot + 1);
		}
	}
}

static void Speedglitchloop()
{
    int sleep1 = 16, sleep2 = 16;
    int last_fps = 0;
    const float EPSILON = 0.008f; // Small value to account for floating-point imprecision
    
    while (true) {
        if (last_fps != RobloxFPS.load(std::memory_order_relaxed)) {
            float delay_float = 1000.0f / RobloxFPS.load(std::memory_order_relaxed);
            int delay_floor = static_cast<int>(delay_float);
            int delay_ceil = delay_floor + 1;
            float fractional = delay_float - delay_floor;

            // More robust floating-point comparisons
            if (fractional < 0.33f - EPSILON) {
                sleep1 = sleep2 = delay_floor;
            }
            else if (fractional > 0.66f + EPSILON) {
                sleep1 = sleep2 = delay_ceil;
            }
            else {
                sleep1 = delay_floor;
                sleep2 = delay_ceil;
            }

            last_fps = RobloxFPS.load(std::memory_order_relaxed);
        }
        
        while (!isspeed.load(std::memory_order_relaxed)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        if (macrotoggled && notbinding && section_toggles[3]) {
            MoveMouse(speed_strengthx, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep1));
            MoveMouse(speed_strengthy, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep2));
        }
    }
}


static void SpeedglitchloopHHJ()
{
    int sleep1 = 16, sleep2 = 16;
    int last_fps = 0;
    const float EPSILON = 0.008f;
    
    while (true) {
        if (last_fps != RobloxFPS.load(std::memory_order_relaxed)) {
            float delay_float = 1000.0f / RobloxFPS.load(std::memory_order_relaxed);
            int delay_floor = static_cast<int>(delay_float);
            int delay_ceil = delay_floor + 1;
            float fractional = delay_float - delay_floor;

            if (fractional < 0.33f - EPSILON) {
                sleep1 = sleep2 = delay_floor;
            }
            else if (fractional > 0.66f + EPSILON) {
                sleep1 = sleep2 = delay_ceil;
            }
            else {
                sleep1 = delay_floor;
                sleep2 = delay_ceil;
            }

            last_fps = RobloxFPS.load(std::memory_order_relaxed);
        }
        
        while (!isHHJ.load(std::memory_order_relaxed)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        
        if (macrotoggled && notbinding && section_toggles[2]) {
            MoveMouse(speed_strengthx, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep1));
            MoveMouse(speed_strengthy, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep2));
        }
    }
}

static void SpamKeyLoop()
{
	while (true) {
		while (!isspamloop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (macrotoggled && notbinding && section_toggles[11]) {
			HoldKeyBinded(vk_spamkey);
			std::this_thread::sleep_for(std::chrono::milliseconds(real_delay));
			ReleaseKeyBinded(vk_spamkey);
			std::this_thread::sleep_for(std::chrono::milliseconds(real_delay));
		}
	}
}

static void ItemClipLoop()
{
	while (true) {
		while (!isitemloop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (macrotoggled && notbinding && section_toggles[8]) {
			HoldKey(clip_slot + 1);
			std::this_thread::sleep_for(std::chrono::milliseconds(clip_delay / 2));
			ReleaseKey(clip_slot + 1);
			std::this_thread::sleep_for(std::chrono::milliseconds(clip_delay / 2));
		}
	}
}

static void WallWalkLoop()
{
	int delay = 16;
	while (true) {
		while (!iswallwalkloop) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			delay = static_cast<unsigned int>(((1000.0f / RobloxFPS.load(std::memory_order_relaxed)) + .5) * 1.1);
		}
		if (macrotoggled && notbinding && section_toggles[10]) {
			if (wallwalktoggleside) {
				MoveMouse(-wallwalk_strengthx, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(delay));
				MoveMouse(-wallwalk_strengthy, 0);
				std::this_thread::sleep_for(std::chrono::microseconds(RobloxWallWalkValueDelay));
			} else {
				MoveMouse(wallwalk_strengthx, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(delay));
				MoveMouse(wallwalk_strengthy, 0);
				std::this_thread::sleep_for(std::chrono::microseconds(RobloxWallWalkValueDelay));
			}
		}
	}
}

static void BhopLoop()
{
    while (true) {
        while (!isbhoploop.load(std::memory_order_acquire)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        HoldKeyBinded(vk_bunnyhopkey);
        std::this_thread::sleep_for(std::chrono::milliseconds(BunnyHopDelay / 2));
        ReleaseKeyBinded(vk_bunnyhopkey);
        std::this_thread::sleep_for(std::chrono::milliseconds(BunnyHopDelay / 2));
    }
}

static void WallhopThread() {
    while (true) {
		while (!iswallhopthread.load(std::memory_order_acquire)) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		const bool lastpressed = lastpressedAD;

		if (wallhopswitch) {

			if (!lastpressed && autoflickdir) {
				MoveMouse(wallhop_dx, 0);
			} else {
				MoveMouse(-wallhop_dx, 0);
			}

		} else {
			if (lastpressed && autoflickdir) {
				MoveMouse(-wallhop_dx, 0);
			} else {
				MoveMouse(wallhop_dx, 0);
			}
		}

		if (toggle_flick) {
			if (WallhopBonusDelay > 0 && WallhopBonusDelay < WallhopDelay) {
				std::this_thread::sleep_for(std::chrono::milliseconds(WallhopBonusDelay));

				if (toggle_jump) {
					HoldKey(0x39);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(WallhopDelay - WallhopBonusDelay));
			} else {
				if (toggle_jump) {
					HoldKey(0x39);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(WallhopDelay));
			}

			if (wallhopswitch) {
				if (!lastpressed && autoflickdir) {
					MoveMouse(-wallhop_dx, 0);
				} else {
					MoveMouse(wallhop_dx, 0);
				}
			} else {
				if (lastpressed && autoflickdir) {
					MoveMouse(wallhop_dx, 0);
				} else {
					MoveMouse(-wallhop_dx, 0);
				}
			}
		} else {
			if (toggle_jump) {
				HoldKey(0x39);
			}
		}

		if (toggle_jump) {
            if (100 - WallhopDelay > 0) {
			    std::this_thread::sleep_for(std::chrono::milliseconds(100 - WallhopDelay));
            }
			ReleaseKey(0x39);
		}

		iswallhopthread = false;
	}
}

static void PressKeyThread() {
    while (true) {
		while (!ispresskeythread.load(std::memory_order_relaxed)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

		if (vk_zkey == vk_dkey) {
			ReleaseKeyBinded(vk_zkey);
		}

		HoldKeyBinded(vk_dkey);
		std::this_thread::sleep_for(std::chrono::milliseconds(PressKeyDelay));
		ReleaseKeyBinded(vk_dkey);

		ispresskeythread = false;
    }
}

static bool IsMainWindow(HWND hwnd)
{
	return (IsWindowVisible(hwnd) && GetWindow(hwnd, GW_OWNER) == NULL);
}

static std::vector<HWND> FindWindowByProcessHandle(const std::vector<HANDLE> &handles)
{
    std::vector<HWND> windows;
    for (HANDLE hProcess : handles) {
        DWORD targetPID = GetProcessId(hProcess);
        HWND rbxhwnd = FindWindowEx(NULL, NULL, NULL, NULL);
        while (rbxhwnd != NULL) {
            DWORD windowPID = 0;
            GetWindowThreadProcessId(rbxhwnd, &windowPID);
            if (windowPID == targetPID && IsMainWindow(rbxhwnd)) {
                windows.push_back(rbxhwnd);
                break; // Assume one main window per process
            }
            rbxhwnd = FindWindowEx(NULL, rbxhwnd, NULL, NULL);
        }
    }
    return windows;
}

// Find Process ID of latest Process in selection
static std::vector<DWORD> GetProcessIdByName(bool takeallprocessids)
{
    // Convert target name from settingsBuffer to wide string
    std::string targetName = settingsBuffer;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring targetNameW = converter.from_bytes(targetName);

    // Create a snapshot of all processes
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        processFound = false;
        return std::vector<DWORD>(); // Return empty vector on failure
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    std::vector<DWORD> result;

    if (takeallprocessids)
    {
        // Collect all matching process IDs
        if (Process32First(hSnapshot, &pe))
        {
            do
            {
                if (_wcsicmp(pe.szExeFile, targetNameW.c_str()) == 0)
                {
                    result.push_back(pe.th32ProcessID);
                }
            } while (Process32Next(hSnapshot, &pe));
        }
        processFound = !result.empty();
    }
    else
    {
        // Find the newest matching process
        DWORD selectedPID = 0;
        ULONGLONG newestCreationTime = 0;
        bool foundAny = false;

        if (Process32First(hSnapshot, &pe))
        {
            do
            {
                if (_wcsicmp(pe.szExeFile, targetNameW.c_str()) == 0)
                {
                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
                    if (hProcess)
                    {
                        FILETIME ftCreation, ftExit, ftKernel, ftUser;
                        if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser))
                        {
                            ULONGLONG creationTime = (static_cast<ULONGLONG>(ftCreation.dwHighDateTime) << 32) | ftCreation.dwLowDateTime;
                            if (!foundAny || creationTime > newestCreationTime)
                            {
                                newestCreationTime = creationTime;
                                selectedPID = pe.th32ProcessID;
                                foundAny = true;
                            }
                        }
                        CloseHandle(hProcess);
                    }
                }
            } while (Process32Next(hSnapshot, &pe));
        }

        if (foundAny)
        {
            result.push_back(selectedPID);
        }
        processFound = foundAny;
    }

    CloseHandle(hSnapshot);
    return result;
}

static std::vector<HANDLE> GetProcessHandles(const std::vector<DWORD> &pids, DWORD accessRights)
{
    std::vector<HANDLE> handles;
    for (DWORD pid : pids) {
        HANDLE hProcess = OpenProcess(accessRights, FALSE, pid);
        if (hProcess != NULL) { // Check if the handle is valid
            handles.push_back(hProcess);
        }
    }
    return handles;
}

static HWND FindNewestProcessWindow(const std::vector<HWND> &hwnds)
{
    if (hwnds.empty()) {
        return NULL;
    }

    DWORD newestPID = 0;
    ULONGLONG newestCreationTime = 0;
    HWND newestHWND = NULL;
    bool foundAny = false;

    for (HWND hwnd : hwnds) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);

        // Open the process to get its creation time
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
        if (hProcess) {
            FILETIME ftCreation, ftExit, ftKernel, ftUser;
            if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
                ULONGLONG creationTime = (static_cast<ULONGLONG>(ftCreation.dwHighDateTime) << 32) | ftCreation.dwLowDateTime;
                if ((!foundAny || creationTime > newestCreationTime) && IsMainWindow(hwnd)) {
                    newestCreationTime = creationTime;
                    newestPID = pid;
                    newestHWND = hwnd;
                    foundAny = true;
                }
            }
            CloseHandle(hProcess);
        }
    }

    return foundAny ? newestHWND : NULL;
}

static bool IsForegroundWindowProcess(const std::vector<HANDLE> &handles)
{
    HWND foreground = GetForegroundWindow();
    if (!foreground) return false; // No foreground window

    DWORD foregroundPID = 0;
    GetWindowThreadProcessId(foreground, &foregroundPID);

    for (HANDLE hProcess : handles) {
        DWORD targetPID = GetProcessId(hProcess);
        if (foregroundPID == targetPID) {
            return true; // Found a match
        }
    }
    return false; // No match
}


static std::string Trim(const std::string &str)
{ // Trim a string
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        ++start;
    }

    auto end = str.end();
    do {
        --end;
    } while (end != start && std::isspace(*end));

    return std::string(start, end + 1);
}

static size_t OutputReleaseVersion(void *contents, size_t size, size_t nmemb, std::string *output)
{
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}


// Generic function to get the content of a URL as a string.
static std::string GetStringFromUrl(const wchar_t* url)
{
    DWORD timeout = 500;
    
    HINTERNET hInternet = InternetOpen(L"Spencer-Macro-Utilities-Updater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";

    InternetSetOption(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOption(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    HINTERNET hConnect = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[4096];
    DWORD bytesRead;
    std::string response;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return response;
}

static std::string GetRemoteVersion()
{
	return GetStringFromUrl(
		L"https://raw.githubusercontent.com/BLOCKCE/Spencer-Macro-Utilities-plus/main/version");
}

static std::string GetRemoteUpdateUrlTemplate()
{
	return GetStringFromUrl(
		L"https://raw.githubusercontent.com/BLOCKCE/Spencer-Macro-Utilities-plus/main/.github/autoupdaterurl");
}


// Generate file name for update
static std::wstring GenerateRandomHexString()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 15);

    std::wstringstream wss;
    for (int i = 0; i < 16; ++i) {
	wss << std::hex << distrib(gen);
    }
    return wss.str();
}

bool DownloadToMemory(const std::wstring& url, std::vector<char>& data) {
    HINTERNET hInternet = InternetOpen(L"Spencer-Macro-Utilities-Updater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        return false;
    }

    HINTERNET hConnect = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    char buffer[4096];
    DWORD bytesRead;
    
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        data.insert(data.end(), buffer, buffer + bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    
    return !data.empty();
}

bool ExtractFileFromMemory(const std::vector<char>& zipBuffer, const std::string& fileNameToExtract, std::vector<char>& extractedData) {
    mz_zip_archive zip_archive;
    mz_zip_zero_struct(&zip_archive);

    if (!mz_zip_reader_init_mem(&zip_archive, zipBuffer.data(), zipBuffer.size(), 0)) {
        return false;
    }

    int file_index = mz_zip_reader_locate_file(&zip_archive, fileNameToExtract.c_str(), NULL, 0);
    if (file_index < 0) {
        mz_zip_reader_end(&zip_archive);
        return false; // File not found in archive
    }

    size_t uncompressed_size = 0;
    void* pBuffer = mz_zip_reader_extract_to_heap(&zip_archive, file_index, &uncompressed_size, 0);
    if (!pBuffer) {
        mz_zip_reader_end(&zip_archive);
        return false; // Failed to extract
    }
    
    extractedData.assign(static_cast<char*>(pBuffer), static_cast<char*>(pBuffer) + uncompressed_size);
    
    mz_free(pBuffer);
    mz_zip_reader_end(&zip_archive);

    return true;
}

void PerformUpdate(const std::string& newVersion, const std::string& localVersion) {
    // 1. Get remote update URL template and construct the final download URL
    std::string urlTemplateAnsi = GetRemoteUpdateUrlTemplate();
    if (urlTemplateAnsi.empty()) {
        MessageBox(NULL, L"Failed to retrieve update URL configuration.", L"Update Error", MB_OK | MB_ICONERROR);
        return;
    }

    // --- FIX 1: Replace ALL instances of the placeholder ---
    std::string placeholder = "{VERSION}";
    size_t start_pos = 0;
    while((start_pos = urlTemplateAnsi.find(placeholder, start_pos)) != std::string::npos) {
        urlTemplateAnsi.replace(start_pos, placeholder.length(), newVersion);
        start_pos += newVersion.length();
    }
    
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring downloadUrl = converter.from_bytes(urlTemplateAnsi);

    // 2. Download the zip file into a memory buffer
    std::vector<char> zipData;
    if (!DownloadToMemory(downloadUrl, zipData)) {
        MessageBox(NULL, L"Failed to download the update. Please check your internet connection.", L"Update Error", MB_OK | MB_ICONERROR);
        return;
    }

    // 3. Extract the target executable from the memory buffer into another memory buffer
    std::vector<char> exeData;
    const std::string exeNameInZip = "suspend.exe"; // The name of the EXE inside the .zip file
    if (!ExtractFileFromMemory(zipData, exeNameInZip, exeData)) {
        MessageBox(NULL, L"Failed to extract the update from the downloaded package. It may be corrupt.", L"Update Error", MB_OK | MB_ICONERROR);
        return;
    }

    // 4. Define paths and generate a random temporary filename
    wchar_t currentExePathArr[MAX_PATH];
    GetModuleFileNameW(NULL, currentExePathArr, MAX_PATH);
    std::wstring currentExePath = currentExePathArr;
    std::wstring currentExeName = PathFindFileNameW(currentExePathArr);

    wchar_t workingDirArr[MAX_PATH];
    wcscpy_s(workingDirArr, currentExePathArr);
    PathRemoveFileSpecW(workingDirArr);
    std::wstring workingDir = workingDirArr;

    std::wstring randomFileName = GenerateRandomHexString();
    std::wstring tempExePath = workingDir + L"\\" + randomFileName + L".tmp";

    // 5. Write the in-memory EXE data to the temporary file on disk
    HANDLE hFile = CreateFileW(tempExePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, L"Could not create temporary update file. Please check folder permissions.", L"Update Error", MB_OK | MB_ICONERROR);
        return;
    }
    DWORD bytesWritten;
    if (!WriteFile(hFile, exeData.data(), exeData.size(), &bytesWritten, NULL) || bytesWritten != exeData.size()) {
        CloseHandle(hFile);
        DeleteFileW(tempExePath.c_str());
        MessageBox(NULL, L"Failed to write update data to temporary file.", L"Update Error", MB_OK | MB_ICONERROR);
        return;
    }
    CloseHandle(hFile);

    // 6. Generate the appropriate batch script (conditionally)
    std::wstring batchScriptContent;
    std::wstring wLocalVersion = converter.from_bytes(localVersion);
    std::wstring wNewVersion = converter.from_bytes(newVersion);

    // --- START: Re-integrated folder rename logic ---
    bool shouldRenameFolder = false;
    std::wstring currentFolderPath = workingDir;
    wchar_t folderNameBuffer[MAX_PATH];
    wcscpy_s(folderNameBuffer, currentFolderPath.c_str());
    PathStripPathW(folderNameBuffer);
    std::wstring currentFolderName = folderNameBuffer;
    
    // Check if the folder name ends with the local version string
    if (!currentFolderName.empty() && currentFolderName.length() > wLocalVersion.length() && 
        currentFolderName.substr(currentFolderName.length() - wLocalVersion.length()) == wLocalVersion) {
        // Check for a common separator before the version string to be safer
        wchar_t separator = currentFolderName[currentFolderName.length() - wLocalVersion.length() - 1];
        if (separator == L'-' || separator == L'_' || separator == L' ' || separator == 'V' || separator == 'v') {
            shouldRenameFolder = true;
        }
    }
    // --- END: Re-integrated folder rename logic ---

    wchar_t tempDir[MAX_PATH];
    GetTempPathW(MAX_PATH, tempDir);
    std::wstring batchFilePath = std::wstring(tempDir) + L"updater-" + GenerateRandomHexString() + L".bat";

    if (shouldRenameFolder) {
        // --- BATCH SCRIPT GENERATION: WITH FOLDER RENAME ---
        std::wstring newFolderName = currentFolderName.substr(0, currentFolderName.length() - wLocalVersion.length()) + wNewVersion;
        
        wchar_t parentOfCurrentArr[MAX_PATH];
        wcscpy_s(parentOfCurrentArr, currentFolderPath.c_str());
        PathRemoveFileSpecW(parentOfCurrentArr);
        
        std::wstring newFolderPath = std::wstring(parentOfCurrentArr) + L"\\" + newFolderName;
        std::wstring newExePathInRenamedFolder = newFolderPath + L"\\" + currentExeName;

		std::wstring tempExePathAfterRename = newFolderPath + L"\\" + randomFileName + L".tmp";
		std::wstring finalExePathInNewFolder = newFolderPath + L"\\" + currentExeName;
		// --- BATCH SCRIPT GENERATION: RENAME ---
		batchScriptContent =
			L"@echo off\n"
			L"pushd \"%~dp0\"\n\n"
			L"echo Updating and renaming folder...\n"
			L"timeout /t 2 /nobreak > NUL\n\n"
			// Use 'move' with the full destination path to rename the folder.
			L"move \"" + currentFolderPath + L"\" \"" + newFolderPath + L"\"\n\n"
			// 2. Delete the old executable (which is now at its new path).
			L"del /F /Q \"" + finalExePathInNewFolder + L"\"\n"
			// Rename the .tmp file to the final executable name.
			L"move \"" + tempExePathAfterRename + L"\" \"" + finalExePathInNewFolder + L"\"\n\n"
			// 4. Relaunch the application from its final location.
			L"start \"\" \"" + finalExePathInNewFolder + L"\"\n"
			// 5. Self-delete the batch script.
			L"(goto) 2>nul & del \"%~f0\"";
    } else {
        // --- BATCH SCRIPT GENERATION: STANDARD (NO FOLDER RENAME) ---
        batchScriptContent =
            L"@echo off\n"
			L"pushd \"%~dp0\"\n\n"
            L"echo Updating in progress...\n"
            L"timeout /t 2 /nobreak > NUL\n"
            // Delete the original executable: %1 is current exe path
            L"del /F /Q \"%~1\"\n"
            // Rename the new temporary file (%2) to the original executable's name (%3)
			L"move \"%~2\" \"%~1\"\n\n"
            // Relaunch the newly updated application
            L"start \"\" \"%~1\"\n"
            // Self-delete the batch script
            L"(goto) 2>nul & del \"%~f0\"";
    }

    // 7. Write and execute the batch script
    HANDLE hBatchFile = CreateFileW(batchFilePath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hBatchFile != INVALID_HANDLE_VALUE) {
        std::string mbBatchScript = converter.to_bytes(batchScriptContent);
        DWORD batchBytesWritten;
        WriteFile(hBatchFile, mbBatchScript.c_str(), mbBatchScript.length(), &batchBytesWritten, NULL);
        CloseHandle(hBatchFile);

        // Prepare parameters for ShellExecute. Each parameter must be quoted.
        // For the standard case, we pass parameters. For the rename case, we don't, as the paths are hardcoded.
        std::wstring params = L"";
        if (!shouldRenameFolder) {
            params = L"\"" + currentExePath + L"\" \"" + tempExePath + L"\" \"" + currentExeName + L"\"";
        }
        
        ShellExecuteW(NULL, L"open", batchFilePath.c_str(), params.c_str(), NULL, SW_HIDE);
        exit(0); // Exit the current application immediately
    } else {
        MessageBox(NULL, L"Could not create the updater script. Please check permissions.", L"Update Error", MB_OK | MB_ICONERROR);
        DeleteFileW(tempExePath.c_str());
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true; // Forward ImGUI-related messages

    switch (msg) {
    case WM_SIZE:
		// Get window size
		GetWindowRect(hWnd, &screen_rect);

		screen_width = screen_rect.right - screen_rect.left;
		screen_height = screen_rect.bottom - screen_rect.top;

		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;

    case WM_GETMINMAXINFO: {
        MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);

        // Set the minimum screen size
        mmi->ptMinTrackSize.x = 1147;
        mmi->ptMinTrackSize.y = 780;
        return 0;
    }

    case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
    case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
    case WM_CLOSE:
		done = true;
		PostQuitMessage(0);
		return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CreateDeviceD3D(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2; // Use double buffering
    sd.BufferDesc.Width = window_width;
    sd.BufferDesc.Height = window_height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 120;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT res = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, NULL, &g_pd3dDeviceContext);

    if (FAILED(res))
		return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) {
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
    }
    if (g_pd3dDeviceContext) {
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = NULL;
    }
    if (g_pd3dDevice) {
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
    }
}

void CreateRenderTarget()
{
    ID3D11Texture2D *pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = NULL;
    }
}

static std::string TrimNullChars(const char *buffer, size_t size)
{
    size_t length = std::strlen(buffer);
    if (length > size)
		length = size;
    return std::string(buffer, length);
}

using NumericVar = std::variant<int*, float*, unsigned int*>;

// Boolean variables to save
const std::unordered_map<std::string, bool *> bool_vars = {
	{"macrotoggled", &macrotoggled},
	{"shiftswitch", &shiftswitch},
	{"wallhopswitch", &wallhopswitch},
	{"wallhopcamfix", &wallhopcamfix},
	{"unequiptoggle", &unequiptoggle},
	{"isspeedswitch", &isspeedswitch},
	{"isfreezeswitch", &isfreezeswitch},
	{"iswallwalkswitch", &iswallwalkswitch},
	{"isspamswitch", &isspamswitch},
	{"isitemclipswitch", &isitemclipswitch},
	{"autotoggle", &autotoggle},
	{"toggle_jump", &toggle_jump},
	{"toggle_flick", &toggle_flick},
	{"camfixtoggle", &camfixtoggle},
	{"wallwalktoggleside", &wallwalktoggleside},
	{"antiafktoggle", &antiafktoggle},
	{"fasthhj", &fasthhj},
	{"wallesslhjswitch", &wallesslhjswitch},
	{"chatoverride", &chatoverride},
	{"bounceautohold", &bounceautohold},
	{"bouncerealignsideways", &bouncerealignsideways},
	{"bouncesidetoggle", &bouncesidetoggle},
	{"laughmoveswitch", &laughmoveswitch},
	{"freezeoutsideroblox", &freezeoutsideroblox},
	{"takeallprocessids", &takeallprocessids},
	{"ontoptoggle", &ontoptoggle},
	{"bunnyhopsmart", &bunnyhopsmart},
	{"presskeyinroblox", &presskeyinroblox},
	{"unequipinroblox", &unequipinroblox},

	//////////////////////////
	{"islagswitch", &islagswitch},
	{"autoflickdir", &autoflickdir},
	{"lagoverlayswitch", &lagoverlayswitch},
	//////////////////////////
};

// Numeric variables to save
const std::unordered_map<std::string, NumericVar> numeric_vars = {
	{"scancode_shift", &scancode_shift},
	{"vk_f5", &vk_f5},
	{"vk_f6", &vk_f6},
	{"vk_f8", &vk_f8},
	{"vk_mbutton", &vk_mbutton},
	{"vk_xbutton1", &vk_xbutton1},
	{"vk_xbutton2", &vk_xbutton2},
	{"vk_leftbracket", &vk_leftbracket},
	{"vk_spamkey", &vk_spamkey},
	{"vk_zkey", &vk_zkey},
	{"vk_dkey", &vk_dkey},
	{"vk_xkey", &vk_xkey},
	{"vk_clipkey", &vk_clipkey},
	{"vk_laughkey", &vk_laughkey},
	{"vk_bouncekey", &vk_bouncekey},
	{"vk_bunnyhopkey", &vk_bunnyhopkey},
	{"selected_dropdown", &selected_dropdown},
	{"vk_wallkey", &vk_wallkey},
	{"PreviousWallWalkSide", &PreviousWallWalkSide},
	{"speed_slot", &speed_slot},
	{"desync_slot", &desync_slot},
	{"clip_slot", &clip_slot},
	{"spam_delay", &spam_delay},
	{"real_delay", &real_delay},
	{"wallhop_dx", &wallhop_dx},
	{"wallhop_dy", &wallhop_dy},
	{"PreviousWallWalkValue", &PreviousWallWalkValue},
	{"maxfreezetime", &maxfreezetime},
	{"maxfreezeoverride", &maxfreezeoverride},
	{"RobloxWallWalkValueDelay", &RobloxWallWalkValueDelay},
	{"speed_strengthx", &speed_strengthx},
	{"speedoffsetx", &speedoffsetx},
	{"speed_strengthy", &speed_strengthy},
	{"speedoffsety", &speedoffsety},
	{"clip_delay", &clip_delay},
	{"RobloxPixelValue", &RobloxPixelValue},
	{"PreviousSensValue", &PreviousSensValue},
	{"windowOpacityPercent", &windowOpacityPercent},
	{"AntiAFKTime", &AntiAFKTime},
	{"display_scale", &display_scale},
	{"WindowPosX", &WindowPosX},
	{"WindowPosY", &WindowPosY},

	//////////////////////////
	{"vk_Lagkey", &vk_Lagkey},
	{"maxlagtime", &maxlagtime},
	{"ovarlayhorisontaloffset", &ovarlayhorisontaloffset},
	{"ovarlayverticaloffset", &ovarlayverticaloffset},
	{"g_textSize", &g_textSize},
	//////////////////////////
};

// Char variables to save
const std::vector<std::pair<std::string, std::pair<char*, size_t>>> char_arrays = {
    {"settingsBuffer", {settingsBuffer, sizeof(settingsBuffer)}},
    {"ItemDesyncSlot", {ItemDesyncSlot, sizeof(ItemDesyncSlot)}},
    {"ItemSpeedSlot", {ItemSpeedSlot, sizeof(ItemSpeedSlot)}},
    {"ItemClipSlot", {ItemClipSlot, sizeof(ItemClipSlot)}},
    {"ItemClipDelay", {ItemClipDelay, sizeof(ItemClipDelay)}},
	{"BunnyHopDelayChar", {BunnyHopDelayChar, sizeof(BunnyHopDelayChar)}},
    {"RobloxSensValue", {RobloxSensValue, sizeof(RobloxSensValue)}},
    {"RobloxWallWalkValueChar", {RobloxWallWalkValueChar, sizeof(RobloxWallWalkValueChar)}},
    {"RobloxWallWalkValueDelayChar", {RobloxWallWalkValueDelayChar, sizeof(RobloxWallWalkValueDelayChar)}},
    {"WallhopPixels", {WallhopPixels, sizeof(WallhopPixels)}},
    {"SpamDelay", {SpamDelay, sizeof(SpamDelay)}},
    {"RobloxPixelValueChar", {RobloxPixelValueChar, sizeof(RobloxPixelValueChar)}},
    {"ChatKeyChar", {ChatKeyChar, sizeof(ChatKeyChar)}},
    {"CustomTextChar", {CustomTextChar, sizeof(CustomTextChar)}},
	{"RobloxFPSChar", {RobloxFPSChar, sizeof(RobloxFPSChar)}},
	{"AntiAFKTimeChar", {AntiAFKTimeChar, sizeof(AntiAFKTimeChar)}},
	{"WallhopDelayChar", {WallhopDelayChar, sizeof(WallhopDelayChar)}},
	{"WallhopBonusDelayChar", {WallhopBonusDelayChar, sizeof(WallhopBonusDelayChar)}},
	{"PressKeyDelayChar", {PressKeyDelayChar, sizeof(PressKeyDelayChar)}},
};

void SaveSettings(const std::string& filepath, const std::string& profile_name) {
    json current_profile_data; // JSON object for the settings of the current profile

	if (profile_name.empty() || profile_name == "(default)") {
		return;
    }

	std::string profile_name_modified = profile_name;

	// Secret code to save a default profile
	if (profile_name == "SAVE_DEFAULT_90493") {
		profile_name_modified = "(default)";
	}

    // Save booleans
    for (const auto& [key, ptr] : bool_vars) {
        if (ptr) current_profile_data[key] = *ptr;
    }

    // Save numerics
    for (const auto& [key, var] : numeric_vars) {
        std::visit([&](auto&& arg) {
            if (arg) current_profile_data[key] = *arg;
        }, var);
    }

    // Save char arrays
    for (const auto& [key, cfg] : char_arrays) {
        if (cfg.first) current_profile_data[key] = TrimNullChars(cfg.first, cfg.second);
    }

    // Special cases
    if (section_amounts > 0) {
        current_profile_data["section_toggles"] = std::vector<bool>(section_toggles, section_toggles + section_amounts);
        current_profile_data["section_order_vector"] = std::vector<int>(section_order, section_order + section_amounts);
    }

    current_profile_data["text"] = text;
    current_profile_data["screen_width"] = screen_width;
    current_profile_data["screen_height"] = screen_height;

	// Save theme data
    current_profile_data["current_theme_index"] = current_theme_index;
    current_profile_data["show_theme_editor"] = show_theme_editor;
    
    // Save custom theme if it exists
    json custom_theme_json;
    custom_theme_json["name"] = custom_theme.name;
    custom_theme_json["bg_dark"] = {custom_theme.bg_dark.x, custom_theme.bg_dark.y, custom_theme.bg_dark.z, custom_theme.bg_dark.w};
    custom_theme_json["bg_medium"] = {custom_theme.bg_medium.x, custom_theme.bg_medium.y, custom_theme.bg_medium.z, custom_theme.bg_medium.w};
    custom_theme_json["bg_light"] = {custom_theme.bg_light.x, custom_theme.bg_light.y, custom_theme.bg_light.z, custom_theme.bg_light.w};
    custom_theme_json["accent_primary"] = {custom_theme.accent_primary.x, custom_theme.accent_primary.y, custom_theme.accent_primary.z, custom_theme.accent_primary.w};
    custom_theme_json["accent_secondary"] = {custom_theme.accent_secondary.x, custom_theme.accent_secondary.y, custom_theme.accent_secondary.z, custom_theme.accent_secondary.w};
    custom_theme_json["text_primary"] = {custom_theme.text_primary.x, custom_theme.text_primary.y, custom_theme.text_primary.z, custom_theme.text_primary.w};
    custom_theme_json["text_secondary"] = {custom_theme.text_secondary.x, custom_theme.text_secondary.y, custom_theme.text_secondary.z, custom_theme.text_secondary.w};
    custom_theme_json["success_color"] = {custom_theme.success_color.x, custom_theme.success_color.y, custom_theme.success_color.z, custom_theme.success_color.w};
    custom_theme_json["warning_color"] = {custom_theme.warning_color.x, custom_theme.warning_color.y, custom_theme.warning_color.z, custom_theme.warning_color.w};
    custom_theme_json["error_color"] = {custom_theme.error_color.x, custom_theme.error_color.y, custom_theme.error_color.z, custom_theme.error_color.w};
    custom_theme_json["border_color"] = {custom_theme.border_color.x, custom_theme.border_color.y, custom_theme.border_color.z, custom_theme.border_color.w};
    custom_theme_json["window_rounding"] = custom_theme.window_rounding;
    custom_theme_json["frame_rounding"] = custom_theme.frame_rounding;
    custom_theme_json["button_rounding"] = custom_theme.button_rounding;
    current_profile_data["custom_theme"] = custom_theme_json;

    current_profile_data["themes"] = {};

    auto vecDiff = [](const ImVec4 &v1, const ImVec4 &v2) {
	    return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
    };

    for (size_t i = 0; i < themes.size(); ++i) {
        const Theme& a = themes[i];
        const Theme& b = defaultthemes[i];

        if (
            a.name != b.name ||
            vecDiff(a.bg_dark, b.bg_dark) ||
            vecDiff(a.bg_medium, b.bg_medium) ||
            vecDiff(a.bg_light, b.bg_light) ||
            vecDiff(a.accent_primary, b.accent_primary) ||
            vecDiff(a.accent_secondary, b.accent_secondary) ||
            vecDiff(a.text_primary, b.text_primary) ||
            vecDiff(a.text_secondary, b.text_secondary) ||
            vecDiff(a.success_color, b.success_color) ||
            vecDiff(a.warning_color, b.warning_color) ||
            vecDiff(a.error_color, b.error_color) ||
            vecDiff(a.border_color, b.border_color) ||
            a.window_rounding != b.window_rounding ||
            a.frame_rounding != b.frame_rounding ||
            a.button_rounding != b.button_rounding
        ) {
            json a_json;
            a_json["name"] = a.name;
            a_json["bg_dark"] = {a.bg_dark.x, a.bg_dark.y, a.bg_dark.z, a.bg_dark.w};
            a_json["bg_medium"] = {a.bg_medium.x, a.bg_medium.y, a.bg_medium.z, a.bg_medium.w};
            a_json["bg_light"] = {a.bg_light.x, a.bg_light.y, a.bg_light.z, a.bg_light.w};
            a_json["accent_primary"] = {a.accent_primary.x, a.accent_primary.y, a.accent_primary.z, a.accent_primary.w};
            a_json["accent_secondary"] = {a.accent_secondary.x, a.accent_secondary.y, a.accent_secondary.z, a.accent_secondary.w};
            a_json["text_primary"] = {a.text_primary.x, a.text_primary.y, a.text_primary.z, a.text_primary.w};
            a_json["text_secondary"] = {a.text_secondary.x, a.text_secondary.y, a.text_secondary.z, a.text_secondary.w};
            a_json["success_color"] = {a.success_color.x, a.success_color.y, a.success_color.z, a.success_color.w};
            a_json["warning_color"] = {a.warning_color.x, a.warning_color.y, a.warning_color.z, a.warning_color.w};
            a_json["error_color"] = {a.error_color.x, a.error_color.y, a.error_color.z, a.error_color.w};
            a_json["border_color"] = {a.border_color.x, a.border_color.y, a.border_color.z, a.border_color.w};
            a_json["window_rounding"] = a.window_rounding;
            a_json["frame_rounding"] = a.frame_rounding;
            a_json["button_rounding"] = a.button_rounding;
            current_profile_data["themes"][a.name] = a_json;
        }
    }

    // --- Profile Handling ---
    json root_json_output = json::object(); // This will be written to the file

    std::ifstream infile(filepath);
    if (infile.is_open()) {
        try {
            json existing_file_data;
            infile >> existing_file_data;
            infile.close();

            if (existing_file_data.is_object() && !existing_file_data.empty()) {
                bool is_already_profile_format = false;
                // Check if any top-level key looks like a profile name (e.g., starts with "Profile ")
                // This is a heuristic; adjust if your profile names have a different pattern.
                for (const auto& item : existing_file_data.items()) {
                    if (item.key().rfind("Profile ", 0) == 0 && item.value().is_object()) {
                        is_already_profile_format = true;
                        break;
                    }
                }

                if (is_already_profile_format) {
                    root_json_output = existing_file_data; // It's already in the new profile format
                } else {
                    // It's an object, but not in "Profile X" format.
                    // Assume it's an old flat format if it contains recognizable settings.
                    // This check helps differentiate an old config from an empty or unrelated JSON object.
                    if ((!bool_vars.empty() && !bool_vars.begin()->first.empty() && existing_file_data.contains(bool_vars.begin()->first)) ||
                        (!numeric_vars.empty() && !numeric_vars.begin()->first.empty() && existing_file_data.contains(numeric_vars.begin()->first)))
                    {
                        std::cout << "Old format detected during save. Converting. Old data will be under 'Profile 1'." << std::endl;
                        root_json_output["Profile 1"] = existing_file_data; // Convert old data to "Profile 1"
                    } else {
                        if (!existing_file_data.empty()) {
                             std::cerr << "Warning: Existing settings file '" << filepath << "' contains an unrecognized JSON object structure. "
                                       << "It will be treated as a base for new profile data." << std::endl;
                             root_json_output = existing_file_data; // Keep it if it was some other map
                        }
                        // If existing_file_data was empty, root_json_output is already a new empty object.
                    }
                }
            }
            // If file was empty or not a JSON object, existing_file_data might be null or non-object.
            // In such cases, root_json_output remains a fresh json::object().
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse error reading '" << filepath << "' for save: " << e.what() << ". Starting with a new profile structure." << std::endl;
            infile.close(); // Ensure file is closed on error
            // root_json_output will be a fresh json::object()
        }
    }
    // If file didn't exist, infile.is_open() was false, root_json_output is a fresh json::object().

    // Add/update the current profile's settings into the root JSON
    root_json_output[profile_name_modified] = current_profile_data;

    if (!G_CURRENTLY_LOADED_PROFILE_NAME.empty()) {
        if (!root_json_output.contains(METADATA_KEY) || !root_json_output[METADATA_KEY].is_object()) {
            root_json_output[METADATA_KEY] = json::object(); // Create metadata object
        }
        root_json_output[METADATA_KEY][LAST_ACTIVE_PROFILE_KEY] = G_CURRENTLY_LOADED_PROFILE_NAME;
    }

	if (profile_name != "SAVE_DEFAULT_90493") {
		// Global variables that don't change across profiles go here saved when not saving as (default)
		root_json_output[METADATA_KEY]["shortdescriptions"] = shortdescriptions;
	}

    // Write the root JSON (which now contains all profiles) to file
    std::ofstream outfile(filepath);
    if (outfile.is_open()) {
        outfile << root_json_output.dump(4); // pretty print with 4 spaces
        outfile.close();
    } else {
        std::cerr << "Error: Could not open settings file for writing: " << filepath << std::endl;
    }
}

void LoadSettings(const std::string& filepath, const std::string& profile_name) {

	if (profile_name == "") {
		return;
	}

	std::ifstream file;
	bool fileFound = false;
	std::filesystem::path real_filepath;

	// First try current directory
	real_filepath = std::filesystem::current_path() / filepath;
	file.open(real_filepath);
	if (file.is_open()) {
		fileFound = true;
	}
	else {
		// If not found, try parent directory
		std::filesystem::path current_path = std::filesystem::current_path();
		if (current_path.has_parent_path()) {
			real_filepath = current_path.parent_path() / filepath;
			file.open(real_filepath);
			if (file.is_open()) {
				fileFound = true;
			}
		}
	}

	if (!fileFound) {
		std::cerr << "Info: Settings file '" << filepath 
				 << "' not found in current or parent directory. Using default values. Saving profile 1." << std::endl;
		return;
	}

	json root_file_json;
	try {
		file >> root_file_json;
		file.close();
	} catch (const json::parse_error& e) {
		std::cerr << "JSON parse error in '" << real_filepath.string() << "': " << e.what() << std::endl;
		file.close();
		return;
	}

	// Make default profile if it doesn't exist
    if ((!root_file_json.contains("(default)") && (profile_name == "SAVE_DEFAULT_90493"))) {
        SaveSettings(filepath, "SAVE_DEFAULT_90493");
    }

    json settings_to_load; // This will hold the JSON data for the specific profile
    bool profile_data_extracted = false;

    // Try to load as new format (profile-based)
    if (root_file_json.is_object() && root_file_json.contains(profile_name)) {
        if (root_file_json.at(profile_name).is_object()) {
            settings_to_load = root_file_json.at(profile_name);
            profile_data_extracted = true;
        } else {
            std::cerr << "Warning: Profile '" << profile_name << "' in '" << filepath << "' is not a valid settings object." << std::endl;
        }
    }

    // Backwards Compatibility: If profile wasn't found AND we are trying to load the designated "legacy" profile name
    if (!profile_data_extracted && profile_name == "Profile 1") { // "Profile 1" is the designated default for old files
        bool looks_like_multi_profile_file = false;
        if (root_file_json.is_object()) {
            for (const auto& item : root_file_json.items()) {
                // Heuristic: if any key starts with "Profile " and its value is an object.
                if (item.key().rfind("Profile ", 0) == 0 && item.value().is_object()) {
                    looks_like_multi_profile_file = true;
                    break;
                }
            }
        }

        // If it doesn't look like a multi-profile file and it contains recognizable settings keys at the root,
        // then treat the root_file_json itself as the settings for "Profile 1".
        if (!looks_like_multi_profile_file && root_file_json.is_object() &&
            (root_file_json.contains("text") || // A common key from old format
             (!bool_vars.empty() && !bool_vars.begin()->first.empty() && root_file_json.contains(bool_vars.begin()->first)) ||
             (!numeric_vars.empty() && !numeric_vars.begin()->first.empty() && root_file_json.contains(numeric_vars.begin()->first))
            ))
        {
            settings_to_load = root_file_json;
            profile_data_extracted = true;
            std::cout << "Info: Loaded legacy format file as '" << profile_name << "'." << std::endl;
        }
    }

    if (!profile_data_extracted) {
		if (profile_name != "SAVE_DEFAULT_90493") {
			std::cerr << "Warning: Profile '" << profile_name << "' not found or data is invalid in '" << filepath << "'. Using current/default values." << std::endl;
		}
        return;
    }
    
    // --- Actual loading  ---
    try {
        // Load booleans
        for (const auto& [key, ptr] : bool_vars) {
            if (ptr && settings_to_load.contains(key) && settings_to_load[key].is_boolean()) {
                *ptr = settings_to_load[key].get<bool>();
            }
        }

        // Load numerics
        for (const auto& [key, var] : numeric_vars) {
            if (!settings_to_load.contains(key)) continue;
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(*arg)>;
                if (arg && settings_to_load[key].is_number()) {
                    *arg = settings_to_load[key].get<T>();
                }
            }, var);
        }

        // Load char arrays
        for (const auto& [key, cfg] : char_arrays) {
            if (cfg.first && settings_to_load.contains(key) && settings_to_load[key].is_string()) {
                std::string str_val = settings_to_load[key].get<std::string>();
                strncpy(cfg.first, str_val.c_str(), cfg.second -1); // Leave space for null terminator
                cfg.first[cfg.second - 1] = '\0'; // Ensure null-termination
            }
        }

        // Load special cases
        if (section_amounts > 0) {
            if (settings_to_load.contains("section_toggles") && settings_to_load["section_toggles"].is_array()) {
                auto toggles = settings_to_load["section_toggles"].get<std::vector<bool>>();
                size_t count = std::min(toggles.size(), static_cast<size_t>(section_amounts));
                std::copy(toggles.begin(), toggles.begin() + count, section_toggles);
            }

            if (settings_to_load.contains("section_order_vector") && settings_to_load["section_order_vector"].is_array()) {
                auto order = settings_to_load["section_order_vector"].get<std::vector<int>>();
                
                // HACKY SOLUTION TO ADD IN FUNCTIONS IN CUSTOM POSITONS BY DEFAULT!
				// Add in Bunnyhop Location to older save files
                if (std::find(order.begin(), order.end(), 13) == order.end() && order.size() >= 6) {
                     if (order.size() >= 7) {
                         order.insert(order.begin() + 7, 13); // Insert at index 6 (7th element)
                     }
				}

				////////////////////////// 
				if (std::find(order.begin(), order.end(), 14) == order.end() && order.size() >= 1) {
					if (order.size() >= 2) {
						order.insert(order.begin() + 2, 14); // Insert at index 1 (2nd element)
					}
				}
				////////////////////////// 
        
                size_t count = std::min(order.size(), static_cast<size_t>(section_amounts));
                for (size_t i = 0; i < count; ++i) {
                    section_order[i] = order[i];
                }
            }
        }

        if (settings_to_load.contains("text") && settings_to_load["text"].is_string()) {
            text = settings_to_load["text"].get<std::string>();
        }
        if (settings_to_load.contains("screen_width") && settings_to_load["screen_width"].is_number_integer()) {
            screen_width = settings_to_load.value("screen_width", screen_width);
        }
        if (settings_to_load.contains("screen_height") && settings_to_load["screen_height"].is_number_integer()) {
            screen_height = settings_to_load.value("screen_height", screen_height);
        }

		// Load global variables in metadata (applies across all profiles)
		if (root_file_json.contains(METADATA_KEY) && root_file_json[METADATA_KEY].is_object()) {
			const auto& metadata = root_file_json[METADATA_KEY];

			if (metadata.contains("shortdescriptions") && metadata["shortdescriptions"].is_boolean()) {
				shortdescriptions = metadata["shortdescriptions"].get<bool>();
			}
		}

		// Load theme data
        current_theme_index = settings_to_load.value("current_theme_index", 0);
        show_theme_editor = settings_to_load.value("show_theme_editor", false);

        // Load custom theme if it exists
        if (settings_to_load.contains("custom_theme") && current_theme_index == themes.size()) {
            json custom_theme_json = settings_to_load["custom_theme"];
            custom_theme.name = custom_theme_json.value("name", "Custom Theme");

            auto load_color = [](const json& j, const std::string& key) -> ImVec4 {
                if (j.contains(key) && j[key].is_array() && j[key].size() == 4) {
                    return ImVec4(j[key][0], j[key][1], j[key][2], j[key][3]);
                }
                return ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            };

            custom_theme.bg_dark = load_color(custom_theme_json, "bg_dark");
            custom_theme.bg_medium = load_color(custom_theme_json, "bg_medium");
            custom_theme.bg_light = load_color(custom_theme_json, "bg_light");
            custom_theme.accent_primary = load_color(custom_theme_json, "accent_primary");
            custom_theme.accent_secondary = load_color(custom_theme_json, "accent_secondary");
            custom_theme.text_primary = load_color(custom_theme_json, "text_primary");
            custom_theme.text_secondary = load_color(custom_theme_json, "text_secondary");
            custom_theme.success_color = load_color(custom_theme_json, "success_color");
            custom_theme.warning_color = load_color(custom_theme_json, "warning_color");
            custom_theme.error_color = load_color(custom_theme_json, "error_color");
            custom_theme.border_color = load_color(custom_theme_json, "border_color");
            custom_theme.window_rounding = custom_theme_json.value("window_rounding", 8.0f);
            custom_theme.frame_rounding = custom_theme_json.value("frame_rounding", 4.0f);
            custom_theme.button_rounding = custom_theme_json.value("button_rounding", 4.0f);
        }

        auto load_color = [](const json& j, const std::string& key) -> ImVec4 {
            if (j.contains(key) && j[key].is_array() && j[key].size() == 4) {
                return ImVec4(j[key][0], j[key][1], j[key][2], j[key][3]);
            }
            return ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        };

        // Load all themes from settings
        if (settings_to_load.contains("themes") && settings_to_load["themes"].is_object()) {
            for (const auto& [key, theme_json] : settings_to_load["themes"].items()) {
                std::string name = theme_json.value("name", key);

                Theme loaded_theme = {
                    name,
                    load_color(theme_json, "bg_dark"),
                    load_color(theme_json, "bg_medium"),
                    load_color(theme_json, "bg_light"),
                    load_color(theme_json, "accent_primary"),
                    load_color(theme_json, "accent_secondary"),
                    load_color(theme_json, "text_primary"),
                    load_color(theme_json, "text_secondary"),
                    load_color(theme_json, "success_color"),
                    load_color(theme_json, "warning_color"),
                    load_color(theme_json, "error_color"),
                    load_color(theme_json, "border_color"),
                    theme_json.value("window_rounding", 8.0f),
                    theme_json.value("frame_rounding", 4.0f),
                    theme_json.value("button_rounding", 4.0f)
                };

                // Check if theme with same name exists
                bool found = false;
                for (auto& theme : themes) {
                    if (theme.name == name) {
                        theme = loaded_theme; // Replace existing
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    themes.push_back(loaded_theme); // Append if not found
                }
            }
        }

    } catch (const json::exception& e) {
        std::cerr << "Load error processing profile '" << profile_name << "': " << e.what() << '\n';
    }
}

std::vector<std::string> GetProfileNames(const std::string& filepath) {
    std::vector<std::string> names;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return names;
    }
    json root_json;
    try {
        file >> root_json;
        if (root_json.is_object()) {
            for (auto& el : root_json.items()) {
				// Skip Metadata
				if (el.key() == METADATA_KEY) {
                    continue;
				}

                if (el.value().is_object()) { // Is it a profile
                    names.push_back(el.key());
                }
            }
        }
    } catch (const json::parse_error& e) {
        std::cerr << "GetProfileNames: JSON parse error in '" << filepath << "': " << e.what() << std::endl;
    }
    file.close();
    // Sort the names alphabetically
    std::sort(names.begin(), names.end());

    // Check if "default" exists in the names
    auto it = std::find(names.begin(), names.end(), "(default)");
    if (it != names.end()) {
        // Move "default" to the front
        names.erase(it);
        names.insert(names.begin(), "(default)");
    }

    return names;
}

bool WriteJsonToFile(const std::string& filepath, const json& data) {
    std::ofstream outfile(filepath);
    if (outfile.is_open()) {
        outfile << data.dump(4);
        outfile.close();
        return true;
    }
    std::cerr << "Error: Could not open file for writing: " << filepath << std::endl;
    return false;
}

bool DeleteProfileFromFile(const std::string& filepath, const std::string& profile_name) {
    std::ifstream infile(filepath);
    if (!infile.is_open()) return false;
    json root_json;
    try { infile >> root_json; } catch (const json::parse_error&) { infile.close(); return false; }
    infile.close();

	if (profile_name == "(default)") {
		return false;
	}

    if (root_json.is_object() && root_json.contains(profile_name)) {
        root_json.erase(profile_name);
        if (WriteJsonToFile(filepath, root_json)) {
            if (G_CURRENTLY_LOADED_PROFILE_NAME == profile_name) {
                G_CURRENTLY_LOADED_PROFILE_NAME = "";
                std::cout << "Info: Deleted profile '" << profile_name << "' was active." << std::endl;
            }
            return true;
        }
    }
    return false;
}

bool RenameProfileInFile(const std::string& filepath, const std::string& old_name, const std::string& new_name) {
    if (old_name == new_name) return true;
    std::ifstream infile(filepath);
    if (!infile.is_open()) return false;
    json root_json;
    try { infile >> root_json; } catch (const json::parse_error&) { infile.close(); return false; }
    infile.close();

	if (old_name == "(default)" || new_name == "(default)") {
		return false;
	}

    if (root_json.is_object() && root_json.contains(old_name)) {
        if (root_json.contains(new_name)) {
             std::cerr << "Rename Error: Target name '" << new_name << "' already exists in file." << std::endl;
            return false;
        }
		
        json profile_data = root_json[old_name];
        root_json.erase(old_name);
        root_json[new_name] = profile_data;
        if (WriteJsonToFile(filepath, root_json)) {
            if (G_CURRENTLY_LOADED_PROFILE_NAME == old_name) {
                G_CURRENTLY_LOADED_PROFILE_NAME = new_name;
            }
            return true;
        }
    }
    return false;
}

bool DuplicateProfileInFile(const std::string& filepath, const std::string& source_name, const std::string& new_name) {
    std::ifstream infile(filepath);
    if (!infile.is_open()) return false;
    json root_json;
    try { infile >> root_json; } catch (const json::parse_error&) { infile.close(); return false; }
    infile.close();

    if (root_json.is_object() && root_json.contains(source_name)) {
        if (root_json.contains(new_name)) {
            std::cerr << "Duplicate Error: Target name '" << new_name << "' already exists in file." << std::endl;
            return false;
        }
        json profile_data_copy = root_json[source_name];
        root_json[new_name] = profile_data_copy;
        return WriteJsonToFile(filepath, root_json);
    }
    return false;
}

std::string GenerateUniqueProfileName(const std::string& base_name, const std::vector<std::string>& existing_names) {
    std::string candidate_name = base_name + " (Copy)";
    if (std::find(existing_names.begin(), existing_names.end(), candidate_name) == existing_names.end()) {
        return candidate_name;
    }
    int i = 2;
    while (true) {
        candidate_name = base_name + " (Copy " + std::to_string(i) + ")";
        if (std::find(existing_names.begin(), existing_names.end(), candidate_name) == existing_names.end()) {
            return candidate_name;
        }
        i++;
        if (i > 1000) return base_name + " (Copy Error)"; // Safety break
    }
}

std::string GenerateNewDefaultProfileName(const std::vector<std::string>& existing_names) {
    int i = 1;
    while (true) {
        std::string candidate_name = "Profile " + std::to_string(i);
        if (std::find(existing_names.begin(), existing_names.end(), candidate_name) == existing_names.end()) {
            return candidate_name;
        }
        i++;
        if (i > 1000) return "Profile Error"; // Safety break
    }
}

bool TryLoadLastActiveProfile(const std::string& filepath) {

    std::ifstream file;
	bool fileFound = false;
	std::filesystem::path real_filepath;

	// First try current directory
	real_filepath = std::filesystem::current_path() / filepath;
	file.open(real_filepath);
	if (file.is_open()) {
		fileFound = true;
	}
	else {
		// If not found, try parent directory
		std::filesystem::path current_path = std::filesystem::current_path();
		if (current_path.has_parent_path()) {
			real_filepath = current_path.parent_path() / filepath;
			file.open(real_filepath);
			if (file.is_open()) {
				fileFound = true;
				LoadSettings(real_filepath.string(), "(default)");
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				G_CURRENTLY_LOADED_PROFILE_NAME = "Profile 1";
				SaveSettings(filepath, "Profile 1");

				std::cerr << "Found Parent Save File!" << std::endl;
				return true;
			}
		}
	}

    if (!fileFound) {
        std::cerr << "TryLoadLastActiveProfile: Settings file '" << filepath << "' not found." << std::endl;
        return false;
    }

    json root_json;
    try {
        file >> root_json;
        file.close();
    } catch (const json::parse_error& e) {
        std::cerr << "TryLoadLastActiveProfile: JSON parse error in '" << filepath << "': " << e.what() << std::endl;
        file.close();
        return false;
    }

    if (root_json.is_object() && root_json.contains(METADATA_KEY) && root_json[METADATA_KEY].is_object()) {
        const auto& metadata = root_json[METADATA_KEY];
        if (metadata.contains(LAST_ACTIVE_PROFILE_KEY) && metadata[LAST_ACTIVE_PROFILE_KEY].is_string()) {
            std::string last_active_name = metadata[LAST_ACTIVE_PROFILE_KEY].get<std::string>();

            // Check if this profile actually exists in the file
            if (!last_active_name.empty() && root_json.contains(last_active_name) && root_json[last_active_name].is_object()) {
                LoadSettings(filepath, last_active_name); 
                G_CURRENTLY_LOADED_PROFILE_NAME = last_active_name;
                std::cout << "Successfully loaded last active profile: " << last_active_name << std::endl;
                return true;
            } else {
                std::cerr << "TryLoadLastActiveProfile: Last active profile '" << last_active_name << "' not found or invalid in settings file." << std::endl;
            }
        } else {
            // std::cout << "TryLoadLastActiveProfile: No 'last_active_profile' key found in metadata." << std::endl;
        }
    } else {
		// Import old settings in
		if (root_json.is_object()) {
			LoadSettings(filepath, "Profile 1");
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			SaveSettings(filepath, "Profile 1");
			G_CURRENTLY_LOADED_PROFILE_NAME = "Profile 1";
		}
    }
    return false;
}

namespace ProfileUI {
    static bool s_expanded = false;
    static std::vector<std::string> s_profile_names;
    static int s_selected_profile_idx = -1;
    
    static int s_editing_profile_idx = -1;
    static char s_edit_buffer[256] = "";
    static float s_last_click_time = -1.0f;
    static int s_last_clicked_item_idx = -1;

    static bool s_profiles_initialized = false;
    static std::string s_rename_error_msg = "";

	// Delete button
	static int s_delete_button_confirmation_stage = 0; // 0: Normal, 1: Yellow (confirm), 2: Red (deleted)
	static float s_delete_button_stage_timer = 0.0f;
	static int s_target_profile_idx_for_confirm = -1;
	static std::string s_G_LOADED_PROFILE_NAME_at_confirm_start = "";
	bool delete_action_requested_this_frame = false;

    void RefreshProfileListAndSelection() {
        std::string previously_selected_name;
        if (s_selected_profile_idx >= 0 && s_selected_profile_idx < s_profile_names.size()) {
            previously_selected_name = s_profile_names[s_selected_profile_idx];
        }
        
        s_profile_names = GetProfileNames(G_SETTINGS_FILEPATH); // Reads and sorts

        s_selected_profile_idx = -1; // Reset selection
        if (!previously_selected_name.empty()) {
            auto it = std::find(s_profile_names.begin(), s_profile_names.end(), previously_selected_name);
            if (it != s_profile_names.end()) {
                s_selected_profile_idx = std::distance(s_profile_names.begin(), it);
            }
        }
        // If nothing was selected, or previous selection is gone, try to select G_CURRENTLY_LOADED_PROFILE_NAME
        if (s_selected_profile_idx == -1 && !G_CURRENTLY_LOADED_PROFILE_NAME.empty()) {
            auto it = std::find(s_profile_names.begin(), s_profile_names.end(), G_CURRENTLY_LOADED_PROFILE_NAME);
            if (it != s_profile_names.end()) {
                s_selected_profile_idx = std::distance(s_profile_names.begin(), it);
            }
        }
    }


    void InitializeProfiles() {
        if (!s_profiles_initialized) {
            RefreshProfileListAndSelection();
            s_profiles_initialized = true;
        }
    }

    void DrawProfileManagerUI() {
        InitializeProfiles();
		ImVec4 current_button_bg_color = ImGui::GetStyle().Colors[ImGuiCol_Button];
		ImVec4 current_button_text_color = ImGui::GetStyle().Colors[ImGuiCol_Text];

        if (ImGui::Button(s_expanded ? "Profiles <" : "Profiles >", ImVec2(270, 0))) {
            s_expanded = !s_expanded;
            if (s_expanded) {
                RefreshProfileListAndSelection(); // Refresh list when opening
                s_rename_error_msg = ""; // Clear previous rename errors
            } else {
                s_editing_profile_idx = -1; // Close editor if dropdown closes
            }
        }


        if (s_expanded) {
            ImVec2 buttonPos = ImGui::GetItemRectMin();
            float menuWidth = ImGui::GetItemRectSize().x;
            
            float list_item_height = ImGui::GetTextLineHeightWithSpacing();
            int num_items_to_show = std::min(10, (int)s_profile_names.size());
            if (num_items_to_show == 0) num_items_to_show = 3; // Min height for empty list
            float buttons_height = ImGui::GetFrameHeightWithSpacing() * 2.0f + ImGui::GetStyle().ItemSpacing.y * 2.0f;
            float list_height = num_items_to_show * list_item_height + ImGui::GetStyle().WindowPadding.y * 2;
            float menuHeight = buttons_height + list_height + ImGui::GetStyle().SeparatorTextAlign.y; // Approx
			int old_s_selected_profile_idx = -1;
            menuHeight = std::min(menuHeight, 300.0f);

            //////////////////////////
            ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y + ImGui::GetStyle().WindowPadding.y));
            //////////////////////////
            ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight));

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
            ImGui::Begin("##ProfilesDropUpMenu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

            float actionButtonWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2) / 3.0f;
            bool profile_is_selected = (s_selected_profile_idx != -1 && s_selected_profile_idx < s_profile_names.size());

			// Save current profile every time s_selected_profile_idx is updated
			if (old_s_selected_profile_idx != s_selected_profile_idx) {
				SaveSettings(G_SETTINGS_FILEPATH, G_CURRENTLY_LOADED_PROFILE_NAME);
			}

			if (profile_is_selected) {
				old_s_selected_profile_idx = s_selected_profile_idx;
			}

            // Buttons
            if (ImGui::Button("Save To", ImVec2(actionButtonWidth, 0))) {
                std::string profileToSave;

                if (profile_is_selected) {
                    profileToSave = s_profile_names[s_selected_profile_idx];
                } else {
                    profileToSave = GenerateNewDefaultProfileName(s_profile_names);
                }

				G_CURRENTLY_LOADED_PROFILE_NAME = profileToSave;
                SaveSettings(G_SETTINGS_FILEPATH, profileToSave);
                RefreshProfileListAndSelection(); // Update list
                // Ensure the saved profile is selected
                auto it = std::find(s_profile_names.begin(), s_profile_names.end(), profileToSave);
                if (it != s_profile_names.end()) s_selected_profile_idx = std::distance(s_profile_names.begin(), it);

                std::cout << "Saved to profile: " << profileToSave << std::endl;
                s_editing_profile_idx = -1; // Ensure editing stops
            }

            ImGui::SameLine();
            if (!profile_is_selected) ImGui::BeginDisabled();

            if (ImGui::Button("Load", ImVec2(actionButtonWidth, 0))) {
                if (profile_is_selected) {
                    LoadSettings(G_SETTINGS_FILEPATH, s_profile_names[s_selected_profile_idx]);
                    G_CURRENTLY_LOADED_PROFILE_NAME = s_profile_names[s_selected_profile_idx];
                    s_editing_profile_idx = -1;
                }
            }

            if (!profile_is_selected) ImGui::EndDisabled();
            ImGui::SameLine();

            if (!profile_is_selected) ImGui::BeginDisabled();

			ImGuiIO &io = ImGui::GetIO();

			// Manage Delete Color
			if (s_delete_button_confirmation_stage != 0) {
				s_delete_button_stage_timer -= io.DeltaTime;
				bool should_reset_state = false;
				if (s_delete_button_stage_timer <= 0.0f) {
					should_reset_state = true; // Timer expired
				}
				if (s_target_profile_idx_for_confirm != -1 && s_selected_profile_idx != s_target_profile_idx_for_confirm) {
					should_reset_state = true;
				}
				if (!s_G_LOADED_PROFILE_NAME_at_confirm_start.empty() && 
					G_CURRENTLY_LOADED_PROFILE_NAME != s_G_LOADED_PROFILE_NAME_at_confirm_start) {
					should_reset_state = true;
				}
				if (should_reset_state) {
					s_delete_button_confirmation_stage = 0;
					s_target_profile_idx_for_confirm = -1;
					s_G_LOADED_PROFILE_NAME_at_confirm_start = "";
					s_delete_button_stage_timer = 0.0f;
				}
			}

			// Update Delete Button Color
			if (profile_is_selected && s_selected_profile_idx == s_target_profile_idx_for_confirm) {
				if (s_delete_button_confirmation_stage == 1) {
					current_button_bg_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
					current_button_text_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black
				}
			}

			ImGui::PushStyleColor(ImGuiCol_Text, current_button_text_color);
			ImGui::PushStyleColor(ImGuiCol_Button, current_button_bg_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, current_button_bg_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, current_button_bg_color);

            if (ImGui::Button("Delete", ImVec2(actionButtonWidth, 0))) {
                if (profile_is_selected) {
                    delete_action_requested_this_frame = true;
                }
            }
			ImGui::PopStyleColor(4);


			if (delete_action_requested_this_frame) {
				if (profile_is_selected) {
					if (s_delete_button_confirmation_stage == 0 || s_selected_profile_idx != s_target_profile_idx_for_confirm) {
						s_delete_button_confirmation_stage = 1; // Yellow
						s_delete_button_stage_timer = 0.7f;
						s_target_profile_idx_for_confirm = s_selected_profile_idx;
						s_G_LOADED_PROFILE_NAME_at_confirm_start = G_CURRENTLY_LOADED_PROFILE_NAME;
					} else if (s_delete_button_confirmation_stage == 1 && s_selected_profile_idx == s_target_profile_idx_for_confirm) {
						s_delete_button_confirmation_stage = 2;

						// Delete Profile
						std::string name_to_delete = s_profile_names[s_target_profile_idx_for_confirm];
						if (DeleteProfileFromFile(G_SETTINGS_FILEPATH, name_to_delete)) {
							RefreshProfileListAndSelection();
							// Delete your profile if you deleted your last save file
							std::ifstream infile(G_SETTINGS_FILEPATH);

							if (!infile.is_open()) return;
							json root_json;
							try { infile >> root_json; } catch (const json::parse_error&) { infile.close(); return; }
							infile.close();

							if (root_json.is_object() && root_json.size() == 1) {
								std::filesystem::remove(G_SETTINGS_FILEPATH);
							}
						}
						s_editing_profile_idx = -1;
					}
				}
			}
	
			delete_action_requested_this_frame = false;

            if (!profile_is_selected) ImGui::EndDisabled();

            if (!profile_is_selected) ImGui::BeginDisabled();
            if (ImGui::Button("Duplicate", ImVec2(-FLT_MIN, 0))) {
                if (profile_is_selected) {
                    std::string source_name = s_profile_names[s_selected_profile_idx];
                    std::string new_name = GenerateUniqueProfileName(source_name, s_profile_names);
                    if (DuplicateProfileInFile(G_SETTINGS_FILEPATH, source_name, new_name)) {
						// Load New Profile
                        LoadSettings(G_SETTINGS_FILEPATH, new_name);
                        G_CURRENTLY_LOADED_PROFILE_NAME = new_name;
                        RefreshProfileListAndSelection();

                        auto it = std::find(s_profile_names.begin(), s_profile_names.end(), new_name);
                        if (it != s_profile_names.end()) s_selected_profile_idx = std::distance(s_profile_names.begin(), it);

                    }
                    s_editing_profile_idx = -1;
                }
            }
            if (!profile_is_selected) ImGui::EndDisabled();
            ImGui::Separator();

            // --- Scrollable selectable list ---
            ImGui::BeginChild("##ProfilesOptionsList", ImVec2(0, ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y - (s_rename_error_msg.empty() ? 0 : ImGui::GetTextLineHeightWithSpacing())) , true, ImGuiWindowFlags_HorizontalScrollbar);
            {
                for (int i = 0; i < s_profile_names.size(); ++i) {
                    bool is_editing_this_item = (s_editing_profile_idx == i);
                    bool is_selected_this_item = (s_selected_profile_idx == i);

                    bool is_currently_loaded = (!G_CURRENTLY_LOADED_PROFILE_NAME.empty() && G_CURRENTLY_LOADED_PROFILE_NAME == s_profile_names[i]);

                    if (is_currently_loaded) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
                    }

					if (s_profile_names[i] == "(default)") {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.54f, 0.0f, 1.0f)); // Orange
					}

                    if (is_editing_this_item) {
                        ImGui::SetKeyboardFocusHere();
                        ImGui::SetNextItemWidth(-FLT_MIN); 

                        if ((ImGui::InputText("##EditProfileName", s_edit_buffer, IM_ARRAYSIZE(s_edit_buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) || (ImGui::IsMouseClicked(0) && !ImGui::IsItemHovered())) {
                            std::string new_name_candidate = s_edit_buffer;
                            s_rename_error_msg = ""; // Clear previous error
                            
							if (new_name_candidate == "(default)") {
								s_rename_error_msg = "Name cannot be default.";
							}

                            if (new_name_candidate.empty()) {
                                s_rename_error_msg = "Name cannot be empty.";
                            } else {
                                bool name_exists_for_another_profile = false;
                                for (int j = 0; j < s_profile_names.size(); ++j) {
                                    if (j != i && s_profile_names[j] == new_name_candidate) {
                                        name_exists_for_another_profile = true;
                                        break;
                                    }
                                }
                                if (name_exists_for_another_profile) {
                                     s_rename_error_msg = "Name already exists.";
                                }
                            }
                            
                            if (s_rename_error_msg.empty()) { // If no error
                                if (RenameProfileInFile(G_SETTINGS_FILEPATH, s_profile_names[i], new_name_candidate)) {
                                    s_profile_names[i] = new_name_candidate; // Update local list immediately
                                    RefreshProfileListAndSelection();
                                    // Re-find the selected index after sort
                                    auto it = std::find(s_profile_names.begin(), s_profile_names.end(), new_name_candidate);
                                    if (it != s_profile_names.end()) s_selected_profile_idx = std::distance(s_profile_names.begin(), it);
                                    
                                    if (G_CURRENTLY_LOADED_PROFILE_NAME == s_profile_names[i] && s_profile_names[i] != new_name_candidate) {
                                        // This case should be handled by RenameProfileInFile updating G_CURRENTLY_LOADED_PROFILE_NAME
                                    }
                                    std::cout << "Renamed profile to: " << new_name_candidate << std::endl;
                                }

                                s_editing_profile_idx = -1; // Exit editing mode on success or file failure
                            }
                            // If s_rename_error_msg is set, stay in editing mode for user to fix.
                        }

                    } else {
                        if (ImGui::Selectable(s_profile_names[i].c_str(), is_selected_this_item, ImGuiSelectableFlags_AllowDoubleClick)) {
                            s_selected_profile_idx = i;
                            if (ImGui::IsMouseDoubleClicked(0)) { // Check for double click on this item
                                s_editing_profile_idx = i;
                                strncpy(s_edit_buffer, s_profile_names[i].c_str(), sizeof(s_edit_buffer) -1);
                                s_edit_buffer[sizeof(s_edit_buffer) - 1] = '\0';
                                s_last_click_time = -1.0f; // Reset double-click state
                                s_rename_error_msg = ""; // Clear rename error when starting new edit
                            } else { // Single click
                                // Handled by s_selected_profile_idx = i;
                                s_last_clicked_item_idx = i; // For your original double click logic if needed
                                s_last_click_time = ImGui::GetTime();
                            }
                        }
                         if (is_selected_this_item) ImGui::SetItemDefaultFocus();
                    }

					if (is_currently_loaded) {
                        ImGui::PopStyleColor();
                    }

					if (s_profile_names[i] == "(default)") {
						ImGui::PopStyleColor();
					}
                }
            }
            ImGui::EndChild(); // ##ProfilesOptionsList

            if (!s_rename_error_msg.empty()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                ImGui::TextWrapped("%s", s_rename_error_msg.c_str());
                ImGui::PopStyleColor();
            }

            ImGui::End(); // ##ProfilesDropUpMenu
            ImGui::PopStyleVar();
        }
    }
}

// Define sections

struct Section {
    std::string title;
    std::string description;
    bool optionA; 
    float settingValue; 
};

struct SectionConfig {
    const char *title;
    const char *description;
};

std::vector<Section> sections;

// Title + Description of Sections

constexpr std::array<SectionConfig, section_amounts> SECTION_CONFIGS = {{
    {"Freeze", "Automatically Tab Glitch With a Button"},
    {"Item Desync", "Enable Item Collision (Hold Item Before Pressing)"},
    {"Helicopter High Jump", "Use COM Offset to Catapult Yourself Into The Air by Aligning your Back Angled to the Wall and Jumping and Letting Your Character Turn"},
    {"Speedglitch", "Use COM offset to Massively Increase Midair Speed"},
    {"Item Unequip COM Offset", "Automatically Do a /e dance2 Item COM Offset Where You Unequip the Item"},
    {"Press a Button", "Whenever You Press Your Keybind, it Presses the Other Button for One Frame"},
    {"Wallhop/Rotation", "Automatically Flick and Jump to easily Wallhop On All FPS"},
    {"Walless LHJ", "Lag High Jump Without a Wall by Offsetting COM Downwards or to the Right"},
    {"Item Clip", "Clip through 2-3 Stud Walls Using Gears"},
    {"Laugh Clip", "Automatically Perform a Laugh Clip"},
    {"Wall-Walk", "Walk Across Wall Seams Without Jumping"},
    {"Spam a Key", "Whenever You Press Your Keybind, it Spams the Other Button"},
    {"Ledge Bounce", "Briefly Falls off a Ledge to Then Bounce Off it While Falling"},
    {"Smart Bunnyhop", "Intelligently enables or disables Bunnyhop for any Key"},

    //////////////////////////
	{"Lag Switch", "Lags the game"},
	//////////////////////////
}};

static void InitializeSections()
{
    sections.clear();
    if (shortdescriptions) {
        for (size_t i = 0; i < SECTION_CONFIGS.size(); ++i) {
			sections.push_back({
				SECTION_CONFIGS[i].title, "",
				false, // Fallback Option1
				50.0f  // Fallback Option2
			});
		}
	} else {
		for (size_t i = 0; i < SECTION_CONFIGS.size(); ++i) {
			sections.push_back({
				SECTION_CONFIGS[i].title, SECTION_CONFIGS[i].description,
				false, // Fallback Option1
				50.0f  // Fallback Option2
			});
		}
	}
}

static unsigned int BindKeyMode(unsigned int currentkey)
{
    static int lastSelectedSection = -1; // Initialize with a value that won't match any valid section

    if (bindingMode) {
		rebindtime = std::chrono::steady_clock::now();
        for (int key = 0; key < 255; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                bindingMode = false;
                std::string currentkeystr = std::format("{:02x}", key); // Convert key into string
                unsigned int currentkeyint = std::stoul(currentkeystr, nullptr, 16); // Convert string into unsigned int
                std::snprintf(KeyBuffer, sizeof(KeyBuffer), "0x%02x", currentkeyint); // Update KeyBuffer text to the key
                return currentkeyint;
            }
        }
    } else { 
        // Check if the selected_section has changed
        if (selected_section != lastSelectedSection) {
            std::snprintf(KeyBuffer, sizeof(KeyBuffer), "0x%02x", currentkey); // Update KeyBuffer for the new section
            lastSelectedSection = selected_section; // Update lastSelectedSection to the current
        }
		
        KeyButtonText = "Click to Bind Key";

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedtime = currentTime - rebindtime;

        if (elapsedtime.count() >= 0.2) { // .2 Second Delay between binds
            notbinding = true;
        }

        return currentkey;
    }
}


static unsigned int BindKeyModeAlt(unsigned int currentkey)
{
    static int lastSelectedSection = -1; // Initialize with a value that won't match any valid section

    if (bindingModealt) {
		rebindtime = std::chrono::steady_clock::now();
        for (int key = 0; key < 255; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                bindingModealt = false;
                std::string currentkeystr = std::format("{:02x}", key); // Convert key into string
                unsigned int currentkeyint = std::stoul(currentkeystr, nullptr, 16); // Convert string into unsigned int
                std::snprintf(KeyBufferalt, sizeof(KeyBufferalt), "0x%02x", currentkeyint); // Update KeyBuffer text to your key
                return currentkeyint;
            }
        }
    } else { 
        // Check if the selected_section has changed
        if (selected_section != lastSelectedSection) {
            std::snprintf(KeyBufferalt, sizeof(KeyBufferalt), "0x%02x", currentkey); // Update KeyBuffer for the new section
            lastSelectedSection = selected_section; // Update lastSelectedSection to the current
        }
        
        KeyButtonTextalt = "Click to Bind Key";

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedtime = currentTime - rebindtime;

        if (elapsedtime.count() >= 0.2) { // .2 Second Delay between binds
            notbinding = true;
        }

        return currentkey;
    }
}


static void GetKeyNameFromHex(unsigned int hexKeyCode)
{
    // Clear the buffer
    memset(KeyBufferhuman, 0, 256);

    // Map the virtual key code to a scan code
    UINT scanCode = MapVirtualKey(hexKeyCode, MAPVK_VK_TO_VSC);

	if (bindingMode) { // Turn off Key-Finding while binding
	    return;
	}

    // Attempt to get the readable key name
    if (GetKeyNameTextA(scanCode << 16, KeyBufferhuman, 256) > 0) {
        // Successfully retrieved the key name
        return; // No further action needed
    } else {
        // If GetKeyNameText fails, try to find the VK_ name
        auto it = vkToString.find(hexKeyCode);
        if (it != vkToString.end()) {
            strncpy(KeyBufferhuman, it->second.c_str(), 256);
        } else {
            // If not found, return a default hex representation
            snprintf(KeyBufferhuman, 256, "0x%X", hexKeyCode);
        }
    }
}

static void GetKeyNameFromHexAlt(unsigned int hexKeyCode)
{
    // Clear the buffer
    memset(KeyBufferhumanalt, 0, 256);

    // Map the virtual key code to a scan code
    UINT scanCode = MapVirtualKey(hexKeyCode, MAPVK_VK_TO_VSC);

	if (bindingModealt) { // Turn off Key-Finding while binding
	    return;
	}

    // Attempt to get the readable key name
	if (GetKeyNameTextA(scanCode << 16, KeyBufferhumanalt, 256) > 0) {
        // Successfully retrieved the key name
        return; // No further action needed
    } else {
        // If GetKeyNameText fails, try to find the VK_ name
        auto it = vkToString.find(hexKeyCode);
        if (it != vkToString.end()) {
            strncpy(KeyBufferhumanalt, it->second.c_str(), 256);
        } else {
            // If not found, return a default hex representation
            snprintf(KeyBufferhumanalt, 256, "0x%X", hexKeyCode);
        }
    }
}

UINT ChatKeyCharToVK(const char* input) {
    if (!input || strlen(input) == 0) {
        return 0;
    }

    wchar_t wideChar;
    if (MultiByteToWideChar(CP_ACP, 0, input, 1, &wideChar, 1) == 0) {
        return 0;
    }

    HKL keyboardLayout = GetKeyboardLayout(0);
    
    SHORT vkAndShift = VkKeyScanExW(wideChar, keyboardLayout);
    if (vkAndShift == -1) {
        return 0; // Character not available
    }

    // Extract the virtual key code (low byte)
    return static_cast<UINT>(LOBYTE(vkAndShift));
}

unsigned int vk_chatkey = ChatKeyCharToVK(ChatKeyChar);

// Make the Title Bar Black
static bool SetTitleBarColor(HWND hwnd, COLORREF color) {
    BOOL value = TRUE;
    HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
    if (FAILED(hr)) {
        return false;
    }
    hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
    if (FAILED(hr)) {
        return false;
    }
    return true;
}

// Disable windows auto-optimizations
static void DisablePowerThrottling() {
    PROCESS_POWER_THROTTLING_STATE state = {};
    state.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    state.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED |
                        PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
    state.StateMask = 0;

    SetProcessInformation(GetCurrentProcess(),
                          ProcessPowerThrottling,
                          &state,
                          sizeof(state));
}

void CheckDisplayScale(HWND hwnd, int display_scale) {
    // Per-Monitor DPI Awareness
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    UINT dpi = GetDpiForWindow(hwnd);
    int currentScalePercent = (int)(dpi * 100 / USER_DEFAULT_SCREEN_DPI); // 96 DPI base

    if (currentScalePercent != 100 && display_scale != currentScalePercent) {
        std::wstring msg = L"Your display scaling doesn't match the program's settings. "
                           L"Your current display scale is " + std::to_wstring(currentScalePercent) +
                           L"%, the Macro's display scale is " + std::to_wstring(display_scale) +
                           L"%. Make these two equal by either updating the Macro's Settings or your Windows Settings.";

        MessageBox(hwnd,
                   msg.c_str(),
                   L"Display Scaling Mismatch",
                   MB_OK | MB_ICONWARNING);
    }
}

// START OF PROGRAM
static void RunGUI()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);

	// Initialize a basic Win32 window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Roblox Macro Client"), NULL };

	// Load icons
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// Override old default profile to new one
	SaveSettings(G_SETTINGS_FILEPATH, "SAVE_DEFAULT_90493");
	
	TryLoadLastActiveProfile(G_SETTINGS_FILEPATH);

	// Load Settings
	LoadSettings(G_SETTINGS_FILEPATH, "SAVE_DEFAULT_90493"); // Only check for existence of default

	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(wc.lpszClassName, _T("Spencer Macro Client Plus"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	::hwnd = hwnd;
	SetTitleBarColor(hwnd, RGB(0, 0, 0));

	// Load Window coordinates and remove invalid coordinates

	if (WindowPosX < 0) {
		WindowPosX = 0;
	}

	if (WindowPosY < 0) {
		WindowPosY = 0;
	}

	if (WindowPosX == 0 && WindowPosY == 0) {
		SetWindowPos(hwnd, NULL, 0, 0, screen_width, screen_height, SWP_NOZORDER | SWP_NOMOVE);
	} else {
		SetWindowPos(hwnd, NULL, WindowPosX, WindowPosY, screen_width, screen_height, SWP_NOZORDER);
	}

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

	// Show the window
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED);

    float alphaFraction = windowOpacityPercent / 100.0f;
    BYTE alphaByte = static_cast<BYTE>(alphaFraction * 255.0f);
    SetLayeredWindowAttributes(hwnd, 0, alphaByte, LWA_ALPHA); // Set to user opacity

	if (ontoptoggle) {
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

	HRSRC hRes = FindResource(NULL, TEXT("LSANS_TTF"), RT_RCDATA);
    HGLOBAL hMem = LoadResource(NULL, hRes);
	LPVOID pData = LockResource(hMem);
    DWORD size = SizeofResource(NULL, hRes);

	ImFont *mainfont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(pData, size, 20.0f);

    // Initialize ImGui for Win32 and DirectX 11
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	auto lastTime = std::chrono::steady_clock::now();
	constexpr float targetFrameTime = 1.0f / 90.0f;  // 90 FPS target
	auto nextFrameTime = lastTime + std::chrono::duration<float>(targetFrameTime);

	InitializeSections();

	MSG msg;

	// Update Specific Variables on startup

	#define SAFE_CONVERT_INT(var, src) \
		try { var = std::stoi(src); } catch (...) {}

	#define SAFE_CONVERT_FLOAT(var, src) \
		try { var = std::stof(src); } catch (...) {}

	#define SAFE_CONVERT_DOUBLE(var, src) \
		try { var = std::stod(src); } catch (...) {}

	SAFE_CONVERT_INT(WallhopDelay, WallhopDelayChar);
	SAFE_CONVERT_INT(clip_slot, ItemClipSlot);
	SAFE_CONVERT_INT(desync_slot, ItemDesyncSlot);
	SAFE_CONVERT_INT(clip_delay, ItemClipDelay);
	SAFE_CONVERT_INT(vk_chatkey, ChatKeyChar);
	SAFE_CONVERT_INT(RobloxFPS, RobloxFPSChar);
	SAFE_CONVERT_INT(AntiAFKTime, AntiAFKTimeChar);

	SAFE_CONVERT_DOUBLE(BunnyHopDelay, BunnyHopDelayChar);

	// Special Cases
	try {
		speed_strengthx = std::stoi(RobloxPixelValueChar);
		speed_strengthy = -speed_strengthx;
	} catch (...) {}

	try {
		spam_delay = std::stof(SpamDelay);
		real_delay = static_cast<int>((spam_delay + 0.5f) / 2);
	} catch (...) {}

    // Attach the GUI thread to the input of the main thread
    DWORD mainThreadId = GetWindowThreadProcessId(hwnd, NULL);
    DWORD guiThreadId = GetCurrentThreadId();
    AttachThreadInput(mainThreadId, guiThreadId, TRUE); // Attach the threads

    // Set window flags to disable resizing, moving, and title bar
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoTitleBar |
										ImGuiWindowFlags_NoScrollbar |
										ImGuiWindowFlags_NoScrollWithMouse |
										ImGuiWindowFlags_NoBringToFrontOnFocus;

	bool amIFocused = true;
	bool processFoundOld = false;

	while (running) {
		// Process all pending messages first
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!running) break;

		// RENDER ONLY IF YOU'RE FOCUSED OR PROCESSFOUND CHANGES
		if (hwnd) { 
			amIFocused = (GetForegroundWindow() == hwnd);
		}

		if (processFoundOld != processFound) {
			amIFocused = true;
		}

		processFoundOld = processFound;

		if (!amIFocused) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			continue; // Skip the rest of this iteration
		}


		// Check if it's time to render
		auto currentTime = std::chrono::steady_clock::now();
		if (currentTime >= nextFrameTime) {

			// Update frame timing
			lastTime = currentTime;
			nextFrameTime = currentTime + std::chrono::duration<float>(targetFrameTime);

			// Start ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

            // ImGui window dimensions
            ImVec2 display_size = ImGui::GetIO().DisplaySize;

			// Apply current theme
            const Theme& current_theme = (current_theme_index < themes.size()) ? themes[current_theme_index] : custom_theme;
            ApplyTheme(current_theme);

            // Set the size of the main ImGui window to fill the screen, fitting to the top left
            ImGui::SetNextWindowSize(display_size, ImGuiCond_Always);
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            
            ImGui::Begin("##MainWindow", nullptr, 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_NoNav);

            // Header section
            ImGui::BeginChild("Header", ImVec2(display_size.x - 32, 105), true);

            // Title and status
            ImGui::PushFont(mainfont);
            ImGui::TextColored(current_theme.accent_primary, "SPENCER MACRO UTILITIES PLUS");
            ImGui::PopFont();

            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

            // Status indicator
            ImGui::PushStyleColor(ImGuiCol_Text, macrotoggled ? current_theme.success_color : current_theme.error_color);
            ImGui::Text(macrotoggled ? "ACTIVE" : "INACTIVE");
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

            // Process status
            ImGui::Text("Process: ");
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, processFound ? current_theme.success_color : current_theme.error_color);
            ImGui::Text(processFound ? "Connected" : "Not Found");
            ImGui::PopStyleColor();

            // Version info
            ImGui::SameLine(ImGui::GetWindowWidth() - 200);
            if (UserOutdated) {
                ImGui::PushStyleColor(ImGuiCol_Text, current_theme.warning_color);
                ImGui::Text("UPDATE AVAILABLE");
                ImGui::PopStyleColor();
            } else {
                ImGui::TextColored(current_theme.text_secondary, "v3.0.3.2");
            }

            static bool show_settings_menu = false;

            
            
            // Quick controls
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

            ImGui::Checkbox("Macro Toggle", &macrotoggled);
            ImGui::SameLine();
            ImGui::Checkbox("Anti-AFK", &antiafktoggle);
            ImGui::SameLine();
            ImGui::Checkbox("Always On Top", &ontoptoggle);
            ImGui::SameLine();

            // Opacity slider
            ImGui::Text("Opacity:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            if (ImGui::SliderFloat("##Opacity", &windowOpacityPercent, 20.0f, 100.0f, "%.0f%%")) {
                float alphaFraction = windowOpacityPercent / 100.0f;
                BYTE alphaByte = static_cast<BYTE>(alphaFraction * 255.0f);
                SetLayeredWindowAttributes(hwnd, 0, alphaByte, LWA_ALPHA);
            }


            

            ImGui::SameLine(ImGui::GetWindowWidth() - 383);

            if (ImGui::Button("Settings")) {
	            show_settings_menu = !show_settings_menu;
            }

            ImGui::SameLine();

            ProfileUI::DrawProfileManagerUI();
            


            if (show_settings_menu) {
	            // Get the main window size
	            ImVec2 main_window_size = ImGui::GetIO().DisplaySize;
	            float child_width = main_window_size.x * 0.5f;
	            float child_height = main_window_size.y * 0.5f;

	            // Calculate position to center the child window
	            ImVec2 child_pos = ImVec2(
		            (main_window_size.x * 0.4f),
		            (main_window_size.y - child_height - 90) * 0.5f
	            );

	            // Set the next window's position and size
	            ImGui::SetNextWindowPos(child_pos, ImGuiCond_Once);
	            ImGui::SetNextWindowSize(ImVec2(child_width, child_height), ImGuiCond_Always);

	            // Begin the child window (non-draggable)
	            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	            if (ImGui::Begin("Settings Menu", &show_settings_menu, window_flags)) {
		            // Begin a scrollable child region for the settings list
		            ImGui::BeginChild("SettingsList", ImVec2(0, 0), true);

		            // Setting: Windows Display Scale
		            ImGui::AlignTextToFramePadding();
                    ImGui::Text("Roblox Executable:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(200);
                    ImGui::InputText("##RobloxExe", settingsBuffer, sizeof(settingsBuffer), ImGuiInputTextFlags_CharsNoBlank);
        
		            ImGui::Separator();

                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Sensitivity:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(80);
                    if (ImGui::InputText("##Sensitivity", RobloxSensValue, sizeof(RobloxSensValue), ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank)) {
                        PreviousSensValue = -1;
                    }

                    ImGui::SameLine();
                    ImGui::TextWrapped("FPS:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(40.0f);
                    if (ImGui::InputText("##FPS", RobloxFPSChar, sizeof(RobloxFPSChar),
	                         ImGuiInputTextFlags_CharsDecimal |
		                         ImGuiInputTextFlags_CharsNoBlank)) {
                        try {
                            RobloxFPS = std::stoi(RobloxFPSChar);
                        } catch (const std::invalid_argument &e) {
                        } catch (const std::out_of_range &e) {
                        }
                    }
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text("Cam-Fix:");
                    ImGui::SameLine();
                    if (ImGui::Checkbox("##CamFix", &camfixtoggle) || PreviousSensValue == -1) {
                        wallhopupdate = false;
                        if (PreviousSensValue != -1) {
                            wallhopupdate = true;
                        }
                        PreviousSensValue = -1;
                        PreviousWallWalkValue = -1;

                        try {
                            if (wallhopupdate) {
                                float factor = camfixtoggle ? 1.388888889f : 1.0f / 1.388888889f;
                                if (wallhopswitch) {
                                    wallhop_dx = std::round(std::stoi(WallhopPixels) * (camfixtoggle ? -factor : factor));
                                    wallhop_dy = std::round(std::stoi(WallhopPixels) * (camfixtoggle ? factor : -factor));
                                } else {
                                    wallhop_dx = std::round(std::stoi(WallhopPixels) * factor);
                                    wallhop_dy = std::round(std::stoi(WallhopPixels) * -factor);
                                    sprintf(WallhopPixels, "%d", wallhop_dx);
                                }
                            }
                        } catch (...) {}

                        float CurrentWallWalkValue = atof(RobloxSensValue);
                        float baseValue = camfixtoggle ? 500.0f : 360.0f;
                        wallwalk_strengthx = -static_cast<int>(std::round((baseValue / CurrentWallWalkValue) * 0.13f));
                        wallwalk_strengthy = static_cast<int>(std::round((baseValue / CurrentWallWalkValue) * 0.13f));
                        sprintf(RobloxWallWalkValueChar, "%d", wallwalk_strengthx);

                        float CurrentSensValue = atof(RobloxSensValue);
                        try {
                            float baseValue = camfixtoggle ? 500.0f : 360.0f;
                            float multiplier = (359.0f / 360.0f) * (359.0f / 360.0f);
                            RobloxPixelValue = static_cast<int>(std::round((baseValue / CurrentSensValue) * multiplier));
                        } catch (...) {}

                        PreviousSensValue = CurrentSensValue;
                        sprintf(RobloxPixelValueChar, "%d", RobloxPixelValue);
                        try {
                            chatkey = ChatKeyChar;
                            speed_strengthx = std::stoi(RobloxPixelValueChar);
                            speed_strengthy = -std::stoi(RobloxPixelValueChar);
                        } catch (...) {}
                    }

                    ImGui::SameLine(ImGui::GetWindowWidth() - 154);

                    if (ImGui::Button("Theme Editor")) {
                        show_theme_editor = !show_theme_editor;
                    }

                    ImGui::Separator();

		            ImGui::Text("Your Current Windows Display Scale Value (10-500%):");
		            ImGui::SetNextItemWidth(150);

		            if (ImGui::InputInt("##DisplayScale", &display_scale)) {
			            if (display_scale < 10)
				            display_scale = 10;
			            if (display_scale > 500)
				            display_scale = 500;
		            }
		            ImGui::SameLine();
		            ImGui::Text("%%");

		            ImGui::Separator();

		            ImGui::AlignTextToFramePadding();
		            ImGui::Text("Amount of Minutes Between Anti-AFK Runs:");
		            ImGui::SameLine();
		            ImGui::SetNextItemWidth(30.0f);
		            if (ImGui::InputText("##AntiAFKTime", AntiAFKTimeChar, sizeof(AntiAFKTimeChar), ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank)) {
			            try {
				            AntiAFKTime = std::stoi(AntiAFKTimeChar);
			            } catch (const std::invalid_argument &e) {
			            } catch (const std::out_of_range &e) {
			            }
		            }

		            ImGui::Separator();

		            ImGui::Checkbox("Switch Macro From \"Left Shift\" to \"Control\" for Shiftlock", &shiftswitch); // Checkbox for toggling

		            ImGui::Separator();

		            ImGui::Checkbox("Force-Set Chat Open Key to \"/\" (Most Stable)", &chatoverride);

		            ImGui::AlignTextToFramePadding();
                    ImGui::Text("Chat Key:");
		            ImGui::SameLine();
		            ImGui::SetNextItemWidth(50);
		            if (ImGui::InputText("##ChatKeyGlobal", ChatKeyChar,
				                 sizeof(ChatKeyChar),
				                 ImGuiInputTextFlags_CharsNoBlank)) {
			            if (strlen(ChatKeyChar) > 1) {
				            ChatKeyChar[1] = '\0';
			            }
			            vk_chatkey = ChatKeyCharToVK(ChatKeyChar);
		            }

		            ImGui::Separator();

		            if (ImGui::Checkbox("Remove Side-Bar Macro Descriptions", &shortdescriptions)) {
			            InitializeSections();
		            }

		            ImGui::Separator();



		            // End the scrollable child region
		            ImGui::EndChild();
	            }
	            ImGui::End();
            }

            ImGui::EndChild();

            // Global Settings section
            

            // Main content area
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);

            // Two-column layout
            float left_panel_width = display_size.x * 0.35f - 16;
            float right_panel_width = display_size.x - left_panel_width - 43;

            // Left panel - Macro list
            ImGui::BeginChild("MacroList", ImVec2(left_panel_width, display_size.y - 154), true);

            ImGui::PushFont(mainfont);
            ImGui::TextColored(current_theme.accent_primary, "MACROS");
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::Spacing();

            // Macro buttons with modern styling
            for (size_t display_index = 0; display_index < section_amounts; ++display_index) {
                int i = section_order[display_index];

                ImGui::PushID(i);

                // Button styling based on state
                if (section_toggles[i]) {
                    ImGui::PushStyleColor(ImGuiCol_Button, current_theme.accent_primary);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, current_theme.accent_secondary);
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Button, current_theme.bg_light);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(current_theme.bg_light.x + 0.05f, current_theme.bg_light.y + 0.05f, current_theme.bg_light.z + 0.05f, 1.0f));
                }

                // Calculate button size with proper text fitting
                float buttonWidth = left_panel_width * 0.85f; // Reduce width to 85% of panel
                float padding = 8.0f; // Reduced padding from 16.0f to 8.0f
                float availableWidth = buttonWidth - (padding * 2);

                // Calculate text sizes with wrapping
                ImVec2 titleSize = ImGui::CalcTextSize(sections[i].title.c_str());

                //////////////////////////
                ImVec2 descSize(0.0f, 0.0f);
		        if (!sections[i].description.empty()) {
                    descSize = ImGui::CalcTextSize(sections[i].description.c_str());
                }
                //////////////////////////

                

                // Check if text needs wrapping
                std::string wrappedTitle = sections[i].title;
                std::string wrappedDesc = sections[i].description;

                if (titleSize.x > availableWidth) {
                    // Wrap title text
                    wrappedTitle = "";
                    std::string currentLine = "";
                    std::istringstream iss(sections[i].title);
                    std::string word;
                    while (iss >> word) {
                        std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;
                        ImVec2 testSize = ImGui::CalcTextSize(testLine.c_str());
                        if (testSize.x > availableWidth && !currentLine.empty()) {
                            wrappedTitle += currentLine + "\n";
                            currentLine = word;
                        } else {
                            currentLine = testLine;
                        }
                    }
                    wrappedTitle += currentLine;
                    titleSize = ImGui::CalcTextSize(wrappedTitle.c_str());
                }

                if (descSize.x > availableWidth) {
                    // Wrap description text
                    wrappedDesc = "";
                    std::string currentLine = "";
                    std::istringstream iss(sections[i].description);
                    std::string word;
                    while (iss >> word) {
                        std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;
                        ImVec2 testSize = ImGui::CalcTextSize(testLine.c_str());
                        if (testSize.x > availableWidth && !currentLine.empty()) {
                            wrappedDesc += currentLine + "\n";
                            currentLine = word;
                        } else {
                            currentLine = testLine;
                        }
                    }
                    wrappedDesc += currentLine;
                    descSize = ImGui::CalcTextSize(wrappedDesc.c_str());
                }

                // Calculate total button height - reduced spacing
                float buttonHeight = titleSize.y + descSize.y + padding * 2 + 4; // Reduced spacing from 8 to 4

                if (ImGui::Button("", ImVec2(buttonWidth, buttonHeight))) {
                    selected_section = i;
                }

                // Drag and drop
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                    ImGui::SetDragDropPayload("DND_SECTION", &display_index, sizeof(int));
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_SECTION")) {
                        int payload_index = *(const int *)payload->Data;
                        std::swap(section_order[payload_index], section_order[display_index]);
                    }
                    ImGui::EndDragDropTarget();
                }

                // Custom text rendering with proper positioning
                ImVec2 buttonPos = ImGui::GetItemRectMin();
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // Render title
                ImVec2 titlePos = ImVec2(buttonPos.x + padding, buttonPos.y + padding);
                drawList->AddText(titlePos, IM_COL32(255, 255, 255, 255), wrappedTitle.c_str());

                // Render description - reduced spacing
                ImVec2 descPos = ImVec2(buttonPos.x + padding, titlePos.y + titleSize.y + 2); // Reduced from 8 to 2
                drawList->AddText(descPos, IM_COL32(200, 200, 200, 200), wrappedDesc.c_str());

                ImGui::PopStyleColor(2);
                ImGui::PopID();

                ImGui::Spacing(); // Reduced from multiple ImGui::Spacing() calls to just one
            }

            ImGui::EndChild();

            // Right panel - Settings
            ImGui::SameLine();
            ImGui::BeginChild("SettingsPanel", ImVec2(right_panel_width, display_size.y - 154), true);

            if (selected_section >= 0 && selected_section < sections.size()) {
                // Section header
                ImGui::PushFont(mainfont);
                ImGui::TextColored(current_theme.accent_primary, "%s", sections[selected_section].title.c_str());
                ImGui::PopFont();
                ImGui::TextColored(current_theme.text_secondary, "%s", sections[selected_section].description.c_str());
                ImGui::Separator();
                ImGui::Spacing();

                // Toggle for this section
                ImGui::Checkbox("Enable This Macro", &section_toggles[selected_section]);
                ImGui::Spacing();

                // Keybind section
                ImGui::TextColored(current_theme.text_primary, "KEYBIND SETTINGS");
                ImGui::Spacing();

                ImGui::Text("Primary Key:");
                ImGui::SameLine();

                if (ImGui::Button(KeyButtonText.c_str(), ImVec2(155, 30))) {
                    notbinding = false;
                    bindingMode = true;
                    KeyButtonText = "Press a Key...";
                }

                ImGui::SameLine();

                // Handle key bindings
                if (section_to_key.count(selected_section)) {
                    unsigned int* key = section_to_key.at(selected_section);
                    *key = BindKeyMode(*key);
                    GetKeyNameFromHex(*key);
                }

                ImGui::SetNextItemWidth(150);
                ImGui::InputText("##KeyName", KeyBufferhuman, sizeof(KeyBufferhuman), ImGuiInputTextFlags_ReadOnly);

                ImGui::SameLine();
                ImGui::TextColored(current_theme.text_secondary, "(Human Readable)");

                ImGui::SetNextItemWidth(80);
                ImGui::InputText("##KeyHex", KeyBuffer, sizeof(KeyBuffer), ImGuiInputTextFlags_CharsNoBlank);

                ImGui::SameLine();
                ImGui::TextColored(current_theme.text_secondary, "(Hex)");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // Section-specific settings
                ImGui::TextColored(current_theme.text_primary, "MACRO SETTINGS");
                ImGui::Spacing();

                // Render section-specific UI
                RenderSectionSettings(selected_section);

            } else {
                ImGui::TextColored(current_theme.text_secondary, "Select a macro from the left panel to configure its settings.");
            }

            ImGui::EndChild();

            ImGui::End();

            // Render theme editor
            RenderThemeEditor();

            // Render
            ImGui::Render();
            const float clear_color[] = { 0.45f, 0.55f, 0.60f, 1.00f };
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_pSwapChain->Present(1, 0);  // Vsync present for smooth rendering
			// Wait until next frame
			std::this_thread::sleep_until(nextFrameTime);

        }

        // No rendering needed
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    AttachThreadInput(mainThreadId, guiThreadId, FALSE);
}

static void RenderSectionSettings(int section) {
    const Theme& current_theme = (current_theme_index < themes.size()) ? themes[current_theme_index] : custom_theme;

    switch (section) {
        case 0: // Freeze
            ImGui::Text("Freeze Duration (seconds):");
            ImGui::SetNextItemWidth(200);
            ImGui::SliderFloat("##FreezeTime", &maxfreezetime, 0.0f, 9.8f, "%.2f s");

            ImGui::Text("Unfreeze Override (ms):");
            ImGui::SetNextItemWidth(200);
            ImGui::InputInt("##FreezeOverride", &maxfreezeoverride);

            ImGui::Spacing();
            ImGui::Checkbox("Freeze Outside Roblox", &freezeoutsideroblox);
            ImGui::Checkbox("Toggle Mode (instead of hold)", &isfreezeswitch);
            ImGui::Checkbox("Freeze All Processes", &takeallprocessids);
            ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("Hold the hotkey to freeze your game, let go of it to release it. Suspending your game also pauses "
								"ALL network and physics activity that the server sends or recieves from you.");
            break;

        case 1: // Item Desync
            ImGui::Text("Gear Slot:");
            ImGui::SetNextItemWidth(100);
            ImGui::InputText("##DesyncSlot", ItemDesyncSlot, sizeof(ItemDesyncSlot), ImGuiInputTextFlags_CharsDecimal);
            try {
                desync_slot = std::stoi(ItemDesyncSlot);
            } catch (...) {}
            ImGui::Separator();
			ImGui::TextWrapped("Equip your item inside of the slot you have picked here, then hold the keybind for 4-7 seconds");
			ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("This Macro rapidly sends number inputs to your roblox client, enough that the server begins to throttle "
								"you. The item that you're holding must not make a serverside sound, else desyncing yourself will be "
								"very buggy, and you will be unable to send any physics data to the server. Once you have desynced, "
								"the server will assume you're not holding an item, but your client will, which permanently enables "
								"client-side collision on the item.");
			ImGui::Separator();
			ImGui::TextWrapped(
								"Also, for convenience sake, you cannot activate desync unless you're tabbed into roblox, You will "
								"most likely crash any other program if you activate it in there.");
            break;

        case 2: // HHJ
            ImGui::Checkbox("Auto Timing", &autotoggle);
            ImGui::Checkbox("Fast Mode (Speedrunning)", &fasthhj);
            ImGui::Separator();
			ImGui::TextWrapped("This module abuses Roblox's conversion from angular velocity to regular velocity, and its flawed centre of mass calculation.");
			ImGui::Separator();
			ImGui::TextWrapped("IMPORTANT:");
			ImGui::TextWrapped("Have your Sensitivity and Cam-Fix options set before using this module.");
			ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("Assuming unequip com offset set to /e dance2 is used prior to offset com, to perform a Helicopter High Jump, "
								"you want to align yourself with your back against the wall, and rotate slightly to the left (around 5-15 degrees). "
								"Now, turn your camera to face directly towards the wall, turn it towards the left a similar amount (5-15 degrees), "
								"in such a way that when you hold W, you turn INTO the wall, instead of away from it (the smaller the angle, the more "
								"successful you'll be). Now, still keeping the alignment and camera angle, perform a normal lag high jump without "
								"holding any movement keys. Instead of lagging, hold w, and press the assigned hotkey.");

			ImGui::Separator();
			ImGui::TextWrapped("If you are struggling with the lag high jump timing part, you can try using the \"Automatically time inputs\" feature. "
								"Align in the exact same way as stated above, but instead doing the lhj motion, just press the assigned key. This should time "
								"the two jumps, as well as the w tap for you. This can also act as a demonstration for what to do, when using manual activation of the module.");
            break;

        case 3: // Speedglitch
            ImGui::Text("Pixel Value (180� turn):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##SpeedPixels", RobloxPixelValueChar, sizeof(RobloxPixelValueChar), ImGuiInputTextFlags_CharsDecimal);
            try {
                speed_strengthx = std::stoi(RobloxPixelValueChar);
                speed_strengthy = -std::stoi(RobloxPixelValueChar);
            } catch (...) {}

            ImGui::Spacing();
            ImGui::Checkbox("Hold Mode", &isspeedswitch);
            ImGui::Separator();
			ImGui::TextWrapped("This module abuses Roblox's conversion from angular velocity to regular velocity, and its flawed centre of mass calculation.");
			ImGui::Separator();
			ImGui::TextWrapped("IMPORTANT: Have your Sensitivity and Cam-Fix options set before using this module.");
			ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();

			ImGui::TextWrapped("Assuming unequip \"/e dance2\" is used prior to offset com, to activate a speed glitch, enable shiftlock mode "
								"(found in roblox settings), and press the keybind once to start the macro (or hold down if you are using the hold key option). "
								"Note that the macro should rotate you exactly 180 degrees. If not, verify your Roblox sensitivity in the settings matches the Macros sensitivity value, "
								"also, test out speedglitch with the \"Cam-Fix\" at the top left set to both true and false. Once the macro is activated, simply jump, and hold w. "
								"As long as you are in the air, you will start to gain immense velocity towards the direction you are facing (assuming shiftlock has been held, and "
								", and you are holding w). ");
            break;

        case 4: // Gear Unequip
            ImGui::Text("Gear Slot:");
            ImGui::SetNextItemWidth(100);
            ImGui::InputText("##SpeedSlot", ItemSpeedSlot, sizeof(ItemSpeedSlot), ImGuiInputTextFlags_CharsDecimal);
            try {
                speed_slot = std::stoi(ItemSpeedSlot);
            } catch (...) {}

            ImGui::Text("Custom Chat Message:");
            ImGui::SetNextItemWidth(300);
            ImGui::InputText("##CustomText", CustomTextChar, sizeof(CustomTextChar));

            ImGui::SetNextItemWidth(150.0f);
			if (ImGui::BeginCombo("Select Emote", optionsforoffset[selected_dropdown])) {
				for (int i = 0; i < IM_ARRAYSIZE(optionsforoffset); i++) {
					bool is_selected = (selected_dropdown == i);
					if (ImGui::Selectable(optionsforoffset[i], is_selected)) {
						selected_dropdown = i;  // Update the selected option
						text = optionsforoffset[selected_dropdown];
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();  // Ensure the selected item has focus
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Checkbox("Let the macro Keep the item equipped", &unequiptoggle);
			ImGui::Checkbox("Make Unequip Com only work while tabbed into Roblox", &unequipinroblox);
			ImGui::Separator();
			ImGui::TextWrapped("This module allows you to trick Roblox into thinking your centre of mass is elsewhere. This is used in the Helicopter "
								"High Jump, Speed Glitch and Walless LHJ modules (may change in the future).");
			ImGui::Separator();
			ImGui::TextWrapped("IMPORTANT: This ONLY works in R6. Although the glitch is possible in R15, the macro isn't built around that rig type. "
								"An Item is also required, and must be placed in the corresponding gear slot (3 by default).");
			ImGui::NewLine();
			ImGui::TextWrapped("Usage:");
			ImGui::TextWrapped("Assuming you have a gear ready, to get an offset com, put the gear into the corresponding gear slot (set above), "
								"and press the keybind (F8 is used with the fn key). Note if the emote bugs out such as restarting halfway through, "
								"or starting late due to a delay, your com may not be in its most offset state. Reusing the macro until the emote "
								"plays out error free will fix this.");
			ImGui::NewLine();
			ImGui::TextWrapped(
				"In most cases, you will be using the \"/e dance2\" emote, as that provides you with the furthest offset, although the other emotes "
				"are still useful occasionaly, such as \"/e laugh\" for wraparounds, and \"/e cheer\" for walless lhjs.");

            break;

        case 5: // Press Key
            ImGui::Text("Secondary Key:");
            ImGui::SameLine();
            if (ImGui::Button((KeyButtonTextalt + "##").c_str(), ImVec2(120, 30))) {
                notbinding = false;
                bindingModealt = true;
                KeyButtonTextalt = "Press a Key...";
            }
            ImGui::SameLine();
            vk_dkey = BindKeyModeAlt(vk_dkey);
            GetKeyNameFromHexAlt(vk_dkey);
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##SecondaryKey", KeyBufferhumanalt, sizeof(KeyBufferhumanalt), ImGuiInputTextFlags_ReadOnly);
            ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("It will press the second keybind for a single frame whenever you press the first keybind. "
								"This is most commonly used for micro-adjustments while moving, especially if you do this while jumping.");
            break;

        case 6: // Wallhop
            
            ImGui::SetNextItemWidth(150);
            ImGui::TextWrapped("Flick Degrees:");

			ImGui::SetNextItemWidth(70.0f);
			snprintf(WallhopDegrees, sizeof(WallhopDegrees), "%d", static_cast<int>(360 * (std::atof(WallhopPixels) * std::atof(RobloxSensValue)) / (camfixtoggle ? 1000 : 720)));
					
			if (ImGui::InputText("##WallhopDegrees", WallhopDegrees, sizeof(WallhopDegrees), ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank)) {
				float pixels = std::atof(WallhopDegrees) * (camfixtoggle ? 1000.0f : 720.0f) / (360.0f * std::atof(RobloxSensValue));
				snprintf(WallhopPixels, sizeof(WallhopPixels), "%.0f", pixels);
				try {
					wallhop_dx = std::round(std::stoi(WallhopPixels));
					wallhop_dy = -std::round(std::stoi(WallhopPixels));
				} catch (const std::invalid_argument &e) {
				} catch (const std::out_of_range &e) {
				}
			}


            ImGui::Text("Flick Pixels:");

            ImGui::InputText("##WallhopPixels", WallhopPixels, sizeof(WallhopPixels), ImGuiInputTextFlags_CharsDecimal);
            try {
                wallhop_dx = std::round(std::stoi(WallhopPixels));
                wallhop_dy = -std::round(std::stoi(WallhopPixels));
            } catch (...) {}

            ImGui::TextWrapped("Wallhop Length (ms):");
			ImGui::SetNextItemWidth(70.0f);
			ImGui::InputText("##WallhopDelay", WallhopDelayChar, sizeof(WallhopDelayChar), ImGuiInputTextFlags_CharsDecimal);
			try {
				WallhopDelay = std::round(std::stoi(WallhopDelayChar));
			} catch (...) {}

			ImGui::TextWrapped("Delay Before Jumping (ms):");
			ImGui::SetNextItemWidth(70.0f);
			ImGui::InputText("##WallhopBonusDelay", WallhopBonusDelayChar, sizeof(WallhopBonusDelayChar), ImGuiInputTextFlags_CharsDecimal);
			try {
				WallhopBonusDelay = std::round(std::stoi(WallhopBonusDelayChar));
			} catch (...) {}

            ImGui::Spacing();
            ImGui::Checkbox("Left-Flick Mode", &wallhopswitch);
            ImGui::Checkbox("Auto Jump", &toggle_jump);
            ImGui::Checkbox("Auto Flick-Back", &toggle_flick);
            ImGui::Checkbox("Auto flick direction", &autoflickdir);
            ImGui::Separator();
			ImGui::TextWrapped("IMPORTANT:");
			ImGui::TextWrapped("THE ANGLE THAT YOU TURN IS DIRECTLY RELATED TO YOUR ROBLOX SENSITIVITY. "
								"If you want to pick a SPECIFIC ANGLE, heres how. "
								"For games without the cam-fix module, 180 degrees is equal to 360 divided by your Roblox Sensitivity. "
								"For games with the cam-fix module, 180 degrees is equal to 500 divided by your Roblox Sensitivity. "
								"Ex: 0.6 sens with no cam fix = 600 pixels, which means 600 / 4 (150) is equal to a 45 degree turn.");
			ImGui::TextWrapped("INTEGERS ONLY!");
			ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("This Macro automatically flicks your screen AND jumps at the same time, performing a wallhop.");
            break;

        case 7: // Walless LHJ
            ImGui::Checkbox("Left-Sided LHJ", &wallesslhjswitch);
            ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("If you offset your center of mass to any direction EXCEPT directly upwards, you will be able to perform "
								"14 stud jumps using this macro. However, you need at LEAST one FULL FOOT on the platform "
								"in order to do it.");
            break;

        case 8: // Item Clip
            ImGui::Text("Item Slot:");
            ImGui::SetNextItemWidth(100);
            ImGui::InputText("##ClipSlot", ItemClipSlot, sizeof(ItemClipSlot), ImGuiInputTextFlags_CharsDecimal);
            try {
                clip_slot = std::stoi(ItemClipSlot);
            } catch (...) {}

            ImGui::Text("Clip Delay (ms):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##ClipDelay", ItemClipDelay, sizeof(ItemClipDelay), ImGuiInputTextFlags_CharsDecimal);
            try {
                clip_delay = std::stoi(ItemClipDelay);
            } catch (...) {}

            ImGui::Spacing();
            ImGui::Checkbox("Hold Mode", &isitemclipswitch);
            ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("This macro will equip and unequip your item in the amount of milliseconds you put in. "
								"It's recommended to shiftlock, jump, and hold W while staying at the wall. "
								"This lets you clip through walls in both R6 and R15, however, it is EXTREMELY RNG. "
								"There are way too factors that control this, the delay, fps, the item's size, your animation, etc. "
								"The item in the best scenario should be big and stretch far into the wall. ");
			ImGui::TextWrapped("Also, for convenience sake, you cannot activate item clip unless you're tabbed into roblox.");
            break;

        case 9: // Laugh Clip
            ImGui::Checkbox("Disable S Key (Weaker but less intrusive)", &laughmoveswitch);
            ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("MUST BE ABOVE 60 FPS AND IN R6!");
			ImGui::TextWrapped("Go against a wall unshiftlocked and angle your camera DIRECTLY OPPOSITE TO THE WALL. "
								"The Macro will Automatically type out /e laugh using the settings inside of the \"Unequip Com\" section. "
								"It will automatically time your shiftlock and jump to laugh clip through up to ~1.3 studs.");
            break;

        case 10: // Wall-Walk
            ImGui::Text("Wall-Walk Pixels:");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##WallWalkPixels", RobloxWallWalkValueChar, sizeof(RobloxWallWalkValueChar), ImGuiInputTextFlags_CharsDecimal);
            try {
                wallwalk_strengthx = std::stoi(RobloxWallWalkValueChar);
                wallwalk_strengthy = -std::stoi(RobloxWallWalkValueChar);
            } catch (...) {}

            ImGui::Text("Flick Delay (�s):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##WallWalkDelay", RobloxWallWalkValueDelayChar, sizeof(RobloxWallWalkValueDelayChar), ImGuiInputTextFlags_CharsDecimal);
            try {
                RobloxWallWalkValueDelay = atof(RobloxWallWalkValueDelayChar);
            } catch (...) {}

            ImGui::Spacing();
            ImGui::Checkbox("Left-Flick Mode", &wallwalktoggleside);
            ImGui::Checkbox("Hold Mode", &iswallwalkswitch);
            ImGui::Separator();
			ImGui::TextWrapped("IMPORTANT: FOR MOST OPTIMAL RESULTS, INPUT YOUR ROBLOX INGAME SENSITIVITY!");
			ImGui::TextWrapped("THE HIGHER FPS YOU ARE, THE MORE STABLE IT GETS, HOWEVER 60 FPS IS ENOUGH FOR INFINITE DISTANCE");
			ImGui::TextWrapped("TICK OR UNTICK THE CHECKBOX DEPENDING ON WHETHER THE GAME USES CAM-FIX MODULE OR NOT. "
								"If you don't know, do BOTH and check which one provides you with a 180 degree rotation. "
								"You can also toggle whether it's right facing or left facing (Makes its respective side easier) "
								"Also, for convenience sake, you cannot activate speedglitch unless you're tabbed into roblox.");
			ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("This macro abuses the way leg raycast physics work to permanently keep wallhopping, without jumping "
								"you can walk up to a wall, maybe at a bit of an angle, and hold W and D or A to slowly walk across.");
            break;

        case 11: // Spam Key
            ImGui::Text("Key to Spam:");
            ImGui::SameLine();
            if (ImGui::Button((KeyButtonTextalt + "##").c_str(), ImVec2(120, 30))) {
                bindingModealt = true;
                notbinding = false;
                KeyButtonTextalt = "Press a Key...";
            }
            ImGui::SameLine();
            vk_spamkey = BindKeyModeAlt(vk_spamkey);
            GetKeyNameFromHexAlt(vk_spamkey);
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##SpamKey", KeyBufferhumanalt, sizeof(KeyBufferhumanalt), ImGuiInputTextFlags_ReadOnly);

            ImGui::Text("Spam Delay (ms):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##SpamDelay", SpamDelay, sizeof(SpamDelay), ImGuiInputTextFlags_CharsDecimal);
            try {
                spam_delay = std::stof(SpamDelay);
                real_delay = static_cast<int>((spam_delay * 1000.0f + 0.5f) / 2);
            } catch (...) {}

            ImGui::Spacing();
            ImGui::Checkbox("Hold Mode", &isspamswitch);
            ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("This macro will spam the second key with a millisecond delay. "
								"This can be used as an autoclicker for any games you want, or a key-spam.");
            break;

        case 12: // Ledge Bounce
            ImGui::Checkbox("Left-Sided Bounce", &bouncesidetoggle);
            ImGui::Checkbox("Stay Horizontal After Bounce", &bouncerealignsideways);
            ImGui::Checkbox("Auto Hold Movement", &bounceautohold);
            ImGui::Separator();
			ImGui::TextWrapped("IMPORTANT:");
			ImGui::TextWrapped("PLEASE SET YOUR ROBLOX SENS AND CAM-FIX CORRECTLY SO IT CAN ACTUALLY DO THE PROPER TURNS!");
			ImGui::TextWrapped("Also, if you set it to automatically hold movement keys, PLEASE HOLD THE KEY YOURSELF AS WELL, else it will keep moving forever.");
			ImGui::Separator();
			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped(
							"Walk up to a ledge with your camera sideways, about half of your left foot should be on the platform. "
							"The Macro will Automatically flick your camera 90 degrees, let you fall, and then flick back. "
							"This will boost you up slightly into the air, and you can even jump after it.");
            break;

        case 13: // Bunnyhop
            ImGui::Text("Bunnyhop Delay (ms):");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##BhopDelay", BunnyHopDelayChar, sizeof(BunnyHopDelayChar), ImGuiInputTextFlags_CharsDecimal);
            try {
                BunnyHopDelay = atof(BunnyHopDelayChar);
            } catch (...) {}

            ImGui::Spacing();
            ImGui::Checkbox("Smart Mode (Auto-disable in chat)", &bunnyhopsmart);
            ImGui::Separator();
			ImGui::TextWrapped("If Intelligent Auto-Toggle is on, pressing your chat key will temporarily disable bhop "
								"until you press left click or enter to leave the chat.");
			ImGui::Separator();

			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped(
							"This Macro will automatically spam your key (typically space) with a specified delay whenever space is held down. "
							"This is created as a more functional Spamkey implementation specifically for Bhop/Bunnyhop.");

			ImGui::TextWrapped("This will not be active unless you are currently inside of the target program.");
            break;
        case 14: // Lag
            ImGui::SetNextItemWidth(60.0f);
			ImGui::InputFloat("##LagFloat", &maxlagtime, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(300.0f);
			ImGui::SliderFloat("##LagSlider", &maxlagtime, 0.0f, 9.8f, "%.2f Seconds");

			ImGui::Text("Lag Override (ms):");
			ImGui::SetNextItemWidth(200);
			ImGui::InputInt("##LagOverride", &maxlagoverride);

			ImGui::Checkbox("Switch from Hold Key to Toggle Key", &islagswitch);
			ImGui::Separator();
			ImGui::Checkbox("Overlay", &lagoverlayswitch);
			ImGui::Text("Modify Horizontal Offset");
			ImGui::SetNextItemWidth(200);
			ImGui::InputInt("##horisontal", &ovarlayhorisontaloffset);
			ImGui::Text("Modify Vertical Offset");
			ImGui::SetNextItemWidth(200);
			ImGui::InputInt("##vertical", &ovarlayverticaloffset);
			ImGui::Text("Modify Text Size");
			ImGui::SetNextItemWidth(200);
			ImGui::InputInt("##size", &g_textSize);
			ImGui::Separator();

			ImGui::TextWrapped("Explanation:");
			ImGui::NewLine();
			ImGui::TextWrapped("This will turn off the wifi for roblox");
			ImGui::NewLine();
			ImGui::TextWrapped("This can be used to reset COM");
            break;
    }
}

static void SetWorkingDirectoryToExecutablePath() // Allows non-standard execution for save files
{
    char exePath[MAX_PATH];
    if (GetModuleFileNameA(nullptr, exePath, MAX_PATH)) {
	// Remove the executable name to get the directory
	char *lastSlash = strrchr(exePath, '\\');
	if (lastSlash) {
	    *lastSlash = '\0'; // Terminate the string at the last backslash
	    SetCurrentDirectoryA(exePath);
		}
    }
}

void DbgPrintf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args); // Use vsnprintf for safety
    va_end(args);
    OutputDebugStringA(buffer); // Send to debug output
}

void CreateDebugConsole() {
    if (AllocConsole()) {
        FILE* pCout;
        freopen_s(&pCout, "CONOUT$", "w", stdout); // Redirect stdout
        FILE* pCerr;
        freopen_s(&pCerr, "CONOUT$", "w", stderr); // Redirect stderr
        FILE* pCin;
        freopen_s(&pCin, "CONIN$", "r", stdin);   // Redirect stdin

        // Optional: Set console title
        SetConsoleTitle(L"Debug Console");

        std::cout.sync_with_stdio(true);
    }
}

// START OF CODE THREAD

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    DisablePowerThrottling();
	// Create Debug Console, use DbgPrintf, printf, or cout to use
    // CreateDebugConsole();

	// Run timers with max precision
    timeBeginPeriod(1);

	// I LOVE THREAD PRIORITY!!!!!!!!!!!!!!!
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	// Set working directory
    SetWorkingDirectoryToExecutablePath();

	// Check for any updates
    std::string remoteVersion = GetRemoteVersion();

    if (!remoteVersion.empty()) 
    {
        remoteVersion = Trim(remoteVersion);
        std::string localVersion = "3.0.3.2";

        if (remoteVersion != localVersion) 
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring wRemoteVersion = converter.from_bytes(remoteVersion);
            std::wstring wLocalVersion = converter.from_bytes(localVersion);
            
            std::wstring message = L"An update is available!\n\n"
                                   L"Current version: " + wLocalVersion + L"\n"
                                   L"Latest version:  " + wRemoteVersion + L"\n\n"
                                   L"Would you like to update now?";

            int result = MessageBoxW(NULL,
                message.c_str(),
                L"Update Available",
                MB_YESNOCANCEL | MB_ICONINFORMATION | MB_DEFBUTTON1 | MB_APPLMODAL);

            if (result == IDYES) 
            {
				PerformUpdate(remoteVersion, localVersion);
            }

			if (result == IDNO) 
			{
				UserOutdated = true;
			}
		
			if (result == IDCANCEL) 
			{
				exit(0);
			}
        }
    }

	// Setup suspension
	const HMODULE hNtdll = GetModuleHandleA("ntdll");
	NtSuspendProcess pfnSuspend = reinterpret_cast<NtSuspendProcess>(GetProcAddress(hNtdll, "NtSuspendProcess"));
	NtResumeProcess pfnResume = reinterpret_cast<NtResumeProcess>(GetProcAddress(hNtdll, "NtResumeProcess"));
	

	std::thread actionThread(Speedglitchloop); // Start a separate thread for item desync loop, lets functions run alongside
	std::thread actionThread2(ItemDesyncLoop);
	std::thread actionThread3(SpeedglitchloopHHJ);
	std::thread actionThread4(SpamKeyLoop);
	std::thread actionThread5(ItemClipLoop);
	std::thread actionThread6(WallWalkLoop);
	std::thread actionThread7(BhopLoop);
	std::thread actionThread8(WallhopThread);
	std::thread actionThread9(PressKeyThread);
	
	std::thread guiThread(RunGUI);
	std::thread KeyboardThread(KeyboardHookThread);
	MSG msg;

	std::vector<HANDLE> hProcess = GetProcessHandles(GetProcessIdByName(takeallprocessids), PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME);
	std::vector<HWND> rbxhwnd = FindWindowByProcessHandle(hProcess); // SET ROBLOX WINDOW HWND RAHHHHH

	// These variables are used for "one-click" functionalies for macros, so you can press a key and it runs every time that key is pressed (without overlapping itself)
	bool isdesync = false;
	bool isSuspended = false; 
	bool islhj = false;
	bool ispresskey = false;
	bool iswallhop = false;
	bool isspeedglitch = false;
	bool isunequipspeed = false;
	bool HHJ = false;
	bool isspam = false;
	bool isclip = false;
	bool iswallwalk = false;
	bool islaugh = false;
	bool isbounce = false;
	bool isbhop = false;
	bool bhoplocked = false;
	static const float targetFrameTime = 1.0f / 90.0f; // Targeting 90 FPS
	auto lastPressTime = std::chrono::steady_clock::now();
	auto lastProcessCheck = std::chrono::steady_clock::now();
	auto startTime = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	auto processchecktime = std::chrono::steady_clock::now();
	static int counter = 0;

    //////////////////////////
	bool isLag = false;
	////////////////////////// 

	while (!done) {
	bool tabbedintoroblox = IsForegroundWindowProcess(hProcess);
    {
		tabbedintoroblox = IsForegroundWindowProcess(hProcess);
		// Freeze
		if ((macrotoggled && notbinding && section_toggles[0])) {
			bool isMButtonPressed = GetAsyncKeyState(vk_mbutton) & 0x8000;

			if (isfreezeswitch) {  // Toggle mode
				if (isMButtonPressed && !wasMButtonPressed && (freezeoutsideroblox || tabbedintoroblox)) {  // Detect button press edge
					isSuspended = !isSuspended;  // Toggle the freeze state
					SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, isSuspended);

					if (isSuspended) {
						suspendStartTime = std::chrono::steady_clock::now();  // Start the timer
					}
				}
			} else {  // Hold mode
				if (isMButtonPressed && (freezeoutsideroblox || tabbedintoroblox)) {
					if (!isSuspended) {
						SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, true);  // Freeze on hold
						isSuspended = true;
						suspendStartTime = std::chrono::steady_clock::now();  // Start the timer
					}
				} else if (isSuspended) {
					SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, false);  // Unfreeze on release
					isSuspended = false;
				}
			}

			// Common timer logic for both toggle and hold modes
			if (isSuspended) {
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - suspendStartTime).count();

				if (elapsed >= (maxfreezetime * 1000)) {
					// Unsuspend for 50 ms
					SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, false);
					std::this_thread::sleep_for(std::chrono::milliseconds(maxfreezeoverride));
					SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, true);

					// Reset the timer
					suspendStartTime = std::chrono::steady_clock::now();
				}
			}

			// Update the previous state
			wasMButtonPressed = isMButtonPressed;
		}

		// Item Desync Macro with anti-idiot design
		if ((GetAsyncKeyState(vk_f5) & 0x8000) && tabbedintoroblox && macrotoggled && notbinding && section_toggles[1]) {
			if (!isdesync) {
				isdesyncloop.store(true, std::memory_order_relaxed);
				isdesync = true;
			}
		} else {
			isdesync = false;
			isdesyncloop.store(false, std::memory_order_relaxed);
		}

		// PressKey
		if ((GetAsyncKeyState(vk_zkey) & 0x8000) && macrotoggled && notbinding && section_toggles[5] && (!presskeyinroblox || tabbedintoroblox)) {
			if (!ispresskey) {
				ispresskeythread.store(true, std::memory_order_relaxed);
				ispresskey = true;
			}
		} else {
			ispresskey = false;
			ispresskeythread.store(false, std::memory_order_relaxed);
		}

		// Wallhop (Ran in separate thread)
		if ((GetAsyncKeyState(vk_xbutton2) & 0x8000) && macrotoggled && notbinding && section_toggles[6]) {
			if (!iswallhop) {
				iswallhopthread.store(true, std::memory_order_relaxed);
				iswallhop = true;
				}
		} else {
			iswallhopthread.store(false, std::memory_order_relaxed);
			iswallhop = false;
		}

		// Walless LHJ (REQUIRES COM OFFSET AND .5 STUDS OF A FOOT ON A PLATFORM)
		if ((GetAsyncKeyState(vk_f6) & 0x8000) && macrotoggled && notbinding && section_toggles[7]) {
			if (!islhj) {
				if (wallesslhjswitch) {
					HoldKey(0x1E);
				} else {
					HoldKey(0x20);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
				HoldKey(0x39);
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
				SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, true);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				if (wallesslhjswitch) {
					ReleaseKey(0x1E);
				} else {
					ReleaseKey(0x20);
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				HoldKey(scancode_shift);
				SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, false);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				ReleaseKey(scancode_shift);
				ReleaseKey(0x39);
				islhj = true;
			}
		} else {
			islhj = false;
		}

		// Speedglitch
		if ((GetAsyncKeyState(vk_xkey) & 0x8000) && tabbedintoroblox && macrotoggled && notbinding && section_toggles[3]) {
			if (!isspeedglitch) {
				isspeed = !isspeed;
				isspeedglitch = true;
			}
		} else {
			isspeedglitch = false;
			if (isspeedswitch) {
				isspeed = false;
			}
		}

		// Gear Unequip COM Speed
		if ((GetAsyncKeyState(vk_f8) & 0x8000) && macrotoggled && notbinding && section_toggles[4] && (!unequipinroblox || tabbedintoroblox)) {
			if (!isunequipspeed) {
				if (chatoverride) {
					HoldKey(0x35);
				} else {
					PasteText(chatkey);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				if (chatoverride) {
					ReleaseKey(0x35);
				}

				if (strlen(CustomTextChar) >= 1) {
					PasteText(CustomTextChar);
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
					HoldKey(0x1C);

					std::this_thread::sleep_for(std::chrono::milliseconds(35));
					ReleaseKey(0x1C);
					isunequipspeed = true;
					continue;
				}

				PasteText(text);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				HoldKey(0x1C);

				std::this_thread::sleep_for(std::chrono::milliseconds(35));
				ReleaseKey(0x1C);
				if (selected_dropdown == 2) { // Cheer
					std::this_thread::sleep_for(std::chrono::milliseconds(16));
				} else {
					std::this_thread::sleep_for(std::chrono::milliseconds(65));
				}

				if (selected_dropdown == 0) { // Dance
					std::this_thread::sleep_for(std::chrono::milliseconds(815));
				}
				if (selected_dropdown == 1) { // Laugh
					std::this_thread::sleep_for(std::chrono::milliseconds(175));
				}
				HoldKey(speed_slot + 1);
				std::this_thread::sleep_for(std::chrono::milliseconds(4));
				ReleaseKey(speed_slot + 1);
				std::this_thread::sleep_for(std::chrono::milliseconds(4));
				if (!unequiptoggle) {
					HoldKey(speed_slot + 1);
				}
				ReleaseKey(speed_slot + 1);
				isunequipspeed = true;
			}
		} else {
			isunequipspeed = false;
		}

		// Helicopter High jump
		if ((GetAsyncKeyState(vk_xbutton1) & 0x8000) && macrotoggled && notbinding && section_toggles[2]) {
			if (!HHJ) {

				if (autotoggle) { // Auto-Key-Timer
					HoldKey(0x39);
					std::this_thread::sleep_for(std::chrono::milliseconds(550));
					HoldKey(0x11);
					std::this_thread::sleep_for(std::chrono::milliseconds(68));
				}

				SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, true);
				if (!fasthhj) {
					std::this_thread::sleep_for(std::chrono::milliseconds(300));
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(200));

				if (autotoggle) { // Auto-Key-Timer
					ReleaseKey(0x39);
					ReleaseKey(0x11);
				}

				SuspendOrResumeProcess(pfnSuspend, pfnResume, hProcess, false);

				std::this_thread::sleep_for(std::chrono::milliseconds(9));
				HoldKey(scancode_shift);
				std::this_thread::sleep_for(std::chrono::milliseconds(17));

				isHHJ.store(true, std::memory_order_relaxed);
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
				ReleaseKey(scancode_shift);
				std::this_thread::sleep_for(std::chrono::milliseconds(243));
				isHHJ.store(false, std::memory_order_relaxed);
				HHJ = true;
			}
		} else {
			HHJ = false;
		}

		// Spamkey
		if ((GetAsyncKeyState(vk_leftbracket) & 0x8000) && macrotoggled && notbinding && section_toggles[11]) {
			if (!isspam) {
				isspamloop = !isspamloop;
				isspam = true;
			}
		} else {
			isspam = false;
			if (isspamswitch) {
				isspamloop = false;
			}
		}

		// Laughkey
		if ((GetAsyncKeyState(vk_laughkey) & 0x8000) && tabbedintoroblox && macrotoggled && notbinding && section_toggles[9]) {
			if (!islaugh) {
				if (chatoverride) {
					HoldKey(0x35);
				} else {
					PasteText(chatkey);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				if (chatoverride) {
					ReleaseKey(0x35);
				}

				PasteText("/e laugh");
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				HoldKey(0x1C);

				std::this_thread::sleep_for(std::chrono::milliseconds(35));
				ReleaseKey(0x1C);

				std::this_thread::sleep_for(std::chrono::milliseconds(248));

				HoldKey(0x39); // Jump
				HoldKey(scancode_shift);

				if (!laughmoveswitch) {
					HoldKey(0x1F); // S
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(25));
				ReleaseKey(0x39);
				ReleaseKey(scancode_shift);
				ReleaseKey(0x1C);
				std::this_thread::sleep_for(std::chrono::milliseconds(25));

				if (!laughmoveswitch) {
					ReleaseKey(0x1F); // S
				}

				islaugh = true;
			}
		} else {
			islaugh = false;
		}


		// Ledge Bounce
		if ((GetAsyncKeyState(vk_bouncekey) & 0x8000) && tabbedintoroblox && macrotoggled && notbinding && section_toggles[12]) {
			if (!isbounce) {
				int turn90 = (camfixtoggle ? 250 : 180) / atof(RobloxSensValue);
				int skey = 0x1F; // S key
				int dkey = 0x20; // D key
				int wkey = 0x11; // W key

				if (bouncesidetoggle) {
					turn90 = -turn90;
					dkey = 0x1E; // A Key
				}

				MoveMouse(-turn90, 0); // Turn Left
				std::this_thread::sleep_for(std::chrono::milliseconds(90));
				HoldKey(skey); // Hold S
				std::this_thread::sleep_for(std::chrono::milliseconds(40));
				ReleaseKey(skey); // Release S
				MoveMouse(turn90, 0); // Turn Right
				HoldKey(dkey);    // Hold D


				std::this_thread::sleep_for(std::chrono::milliseconds(16));

				if (bouncerealignsideways) {
					ReleaseKey(dkey); // Release D
					HoldKey(wkey); // Hold W
				}

				if (!bouncerealignsideways) {
					ReleaseKey(dkey);
				}

				// After Bounce
				if (bouncerealignsideways) {
					MoveMouse(turn90, 0); // Turn Right to face towards ledge
					std::this_thread::sleep_for(std::chrono::milliseconds(70));
					ReleaseKey(wkey);      // Release W
					MoveMouse(-turn90, 0); // Turn Left to face normally
					// Right facing end
					if (bounceautohold) {
						HoldKey(dkey); // Hold D
					}
				} else {
					// Front Facing End
					if (bounceautohold) {
						HoldKey(wkey); // Hold W
					}
					MoveMouse(turn90, 0); // Turn Right
				}

				isbounce = true;
			}
		} else {
			isbounce = false;
		}


		// Item Clip
		if ((GetAsyncKeyState(vk_clipkey) & 0x8000) && tabbedintoroblox && macrotoggled && notbinding && section_toggles[8]) {
			if (!isclip) {
				isitemloop = !isitemloop;
				isclip = true;
			}
		} else {
			isclip = false;
			if (isitemclipswitch) {
				isitemloop = false;
			}
		}


		// WallWalk
		if ((GetAsyncKeyState(vk_wallkey) & 0x8000) && tabbedintoroblox && macrotoggled && notbinding && section_toggles[10]) {
			if (!iswallwalk) {
				iswallwalkloop = !iswallwalkloop;
				iswallwalk = true;
			}
		} else {
			iswallwalk = false;
			if (iswallwalkswitch) {
				iswallwalkloop = false;
			}
		}

		bool can_process_bhop = GetAsyncKeyState(vk_bunnyhopkey) && tabbedintoroblox && section_toggles[13] && macrotoggled && notbinding;

		if (GetAsyncKeyState(vk_chatkey) & 0x8000) {
			bhoplocked = true;
		}

		if (bhoplocked) {
			if (GetAsyncKeyState(VK_RETURN) & 0x8000 || GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
				bhoplocked = false;
			}
		}

		if (can_process_bhop) {
			bool is_bunnyhop_key_considered_held = g_isVk_BunnyhopHeldDown.load(std::memory_order_relaxed);

			if (bunnyhopsmart) {
				if (!bhoplocked && is_bunnyhop_key_considered_held) {
					if (!isbhop) {
						isbhoploop = true;
						isbhop = true;
					}
				} else {
					if (isbhop) {
						isbhoploop = false;
						isbhop = false;
					}
				}
			} else {
				if (is_bunnyhop_key_considered_held) {
					if (!isbhop) {
						isbhoploop = true;
						isbhop = true;
					}
				} else {
					if (isbhop) {
						isbhoploop = false;
						isbhop = false;
					}
				}
			}
		}

        ////////////////////////// 
		// Lag
		if ((macrotoggled && notbinding && section_toggles[14])) {
			bool isLagPressed = GetAsyncKeyState(vk_Lagkey) & 0x8000;

			if (islagswitch) { // Toggle mode
				if (isLagPressed && !wasLagPressed) {
					isLag = !isLag; // Toggle the freeze state
					LagOrUnLagProcess(hProcess, isLag);
					lagStartTime =
						std::chrono::steady_clock::now(); // Start the timer
				}
			} else { // Hold mode
				if (isLagPressed) {
					if (!isLag) {

						LagOrUnLagProcess(hProcess, true);
						isLag = true;
						lagStartTime = std::chrono::steady_clock::
							now(); // Start the timer
					}
				} else if (isLag) {
					LagOrUnLagProcess(hProcess, false);
					isLag = false;
				}
			}

			if (isLag) {
				auto elapsed =
					std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::steady_clock::now() - lagStartTime)
						.count();

				if (elapsed >= (maxlagtime * 1000)) {
					// Unsuspend for 50 ms
					LagOrUnLagProcess(hProcess, false);
					std::this_thread::sleep_for(
						std::chrono::milliseconds(maxlagoverride));
					LagOrUnLagProcess(hProcess, true);

					// Reset the timer
					lagStartTime = std::chrono::steady_clock::now();
				}
			}

			// Update the previous state
			

			HWND targetHwnd = GetWindowFromProcess(hProcess);
			bool shouldShowOverlay = (targetHwnd && tabbedintoroblox &&
						  !IsIconic(targetHwnd));

			if (lagoverlayswitch && shouldShowOverlay) {
				if (!g_overlay) {
					HINSTANCE hInstance = GetModuleHandle(nullptr);
					CreateOverlay(hInstance);
				}
				UpdateOverlayToWindow(g_overlay, targetHwnd);
			} else {
				if (g_overlay) {
					DestroyOverlay();
				}
			}

			if (isLag) {
				g_textColor = RGB(0, 255, 0); // green
				g_overlayText = L"LagSwitch ON";
			} else {
				g_textColor = RGB(255, 0, 0); // red
				g_overlayText = L"LagSwitch OFF";
			}

			if (g_overlay && lastLag != isLag) {
				InvalidateRect(g_overlay, nullptr, TRUE);
				UpdateWindow(g_overlay);

			}

			wasLagPressed = isLagPressed;
			lastLag = isLag;

		}

		bool isDPressed = GetAsyncKeyState(VkKeyScanEx('D', GetKeyboardLayout(0)) & 0xFF) & 0x8000;
		bool isAPressed = GetAsyncKeyState(VkKeyScanEx('A', GetKeyboardLayout(0)) & 0xFF) & 0x8000;

		if (isAPressed) {
			lastpressedAD = true;
		} else if (isDPressed) {
			lastpressedAD = false;
		}
		

		////////////////////////// 

		// Every second, check if roblox continues to exist.
		if (++counter % 100 == 0) {  // Check time every 100th iteration
			now = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::seconds>(now - processchecktime).count() >= 1) {
				hProcess = GetProcessHandles(GetProcessIdByName(takeallprocessids), PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME);
				rbxhwnd = FindWindowByProcessHandle(hProcess);
				counter = 0;
				processchecktime = std::chrono::steady_clock::now();
			}
		}

		// Anti AFK (MUST STAY AT THE LOWEST PART OF THE LIST!!!)
		if (!isafk && tabbedintoroblox) {
			// Not Afk, reset lastpresstime
			lastPressTime = std::chrono::steady_clock::now();
		} else {
			if (processFound && antiafktoggle && isafk) {
				// Check if AntiAFKTime has passed
				auto elapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(now - lastPressTime).count();
				if (elapsedMinutes >= AntiAFKTime) {
					HWND originalHwnd = GetForegroundWindow();
					INPUT input = {0};
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					if (!IsForegroundWindowProcess(hProcess)) { // Extremely long process to simulate going to roblox and typing .
						HWND windowhwnd = FindNewestProcessWindow(rbxhwnd);
						SetWindowPos(windowhwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(windowhwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						Sleep(1000);
						SendInput(1, &input, sizeof(INPUT));
						input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
						Sleep(50);
						SendInput(1, &input, sizeof(INPUT));
						Sleep(500);
						if (chatoverride) {
							HoldKey(0x35);
						} else {
							PasteText(chatkey);
						}
						if (chatoverride) {
							Sleep(20);
							ReleaseKey(0x35);
						}
						Sleep(20);
						HoldKey(0x34);
						Sleep(20);
						ReleaseKey(0x34);
						Sleep(20);
						HoldKey(0x1C);
						Sleep(20);
						ReleaseKey(0x1C);
						Sleep(500);
						SetWindowPos(originalHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						SetWindowPos(originalHwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						Sleep(1000);
						SetForegroundWindow(originalHwnd);
						lastPressTime = std::chrono::steady_clock::now();
					}
					if (IsForegroundWindowProcess(hProcess)) {
						if (chatoverride) {
							HoldKey(0x35);
						} else {
							PasteText(chatkey);
						}
						if (chatoverride) {
							Sleep(20);
							ReleaseKey(0x35);
						}
						Sleep(20);
						HoldKey(0x34);
						Sleep(20);
						ReleaseKey(0x34);
						Sleep(20);
						HoldKey(0x1C);
						Sleep(20);
						ReleaseKey(0x1C);
						Sleep(500);
						lastPressTime = std::chrono::steady_clock::now();
					}
				}
			}
		}
		// You are technically always AFK until the keyboard thread says otherwise
		isafk = true;
	}
	
	// Automatically turn off these 4 modules if you leave roblox window (so it isn't annoying)
	if (!tabbedintoroblox) {
		isbhoploop = false;
		iswallwalkloop = false;
		isitemloop = false;
		isdesyncloop = false;
		isspeed = false;
	}

	std::this_thread::sleep_for(std::chrono::microseconds(50)); // Delay between main code loop (so your cpu doesn't die instantly)

}

	// Save Window Positions and Size before closing
	RECT windowrect;
	GetWindowRect(hwnd, &windowrect);

	if (windowrect.left < 0) {
		WindowPosX = 0;
	} else {
		WindowPosX = windowrect.left;
	}

	if (windowrect.top < 0) {
		WindowPosY = 0;
	} else {
		WindowPosY = windowrect.top;
	}

	RECT screen_rect;

	GetWindowRect(hwnd, &screen_rect);

	screen_width = screen_rect.right - screen_rect.left;
	screen_height = screen_rect.bottom - screen_rect.top;

	// If save file, save normally, if not, save as Profile 1
	if (!G_CURRENTLY_LOADED_PROFILE_NAME.empty()) {
		SaveSettings(G_SETTINGS_FILEPATH, G_CURRENTLY_LOADED_PROFILE_NAME);
	} else {
		std::ifstream file(G_SETTINGS_FILEPATH);
		if (!file.is_open()) {
			G_CURRENTLY_LOADED_PROFILE_NAME = "Profile 1";
			SaveSettings(G_SETTINGS_FILEPATH, G_CURRENTLY_LOADED_PROFILE_NAME);
		}
	}

	if (g_keyboardHook) {
		UnhookWindowsHookEx(g_keyboardHook);
		g_keyboardHook = NULL;
	}


	guiThread.join();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
	DestroyWindow(hwnd);
	timeEndPeriod(1);
	exit(0);

	return 0;
}

static void ApplyTheme(const Theme &theme)
{
	ImGuiStyle &style = ImGui::GetStyle();

	// Apply modern styling
	style.WindowPadding = ImVec2(16, 16);
	style.FramePadding = ImVec2(8, 6);
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.ScrollbarSize = 12;
	style.GrabMinSize = 8;
	style.WindowRounding = theme.window_rounding;
	style.ChildRounding = theme.window_rounding * 0.75f;
	style.FrameRounding = theme.frame_rounding;
	style.PopupRounding = theme.window_rounding * 0.75f;
	style.ScrollbarRounding = theme.frame_rounding;
	style.GrabRounding = theme.button_rounding;
	style.TabRounding = theme.frame_rounding;

	// Colors
	style.Colors[ImGuiCol_WindowBg] = theme.bg_dark;
	style.Colors[ImGuiCol_ChildBg] = theme.bg_medium;
	style.Colors[ImGuiCol_PopupBg] = theme.bg_medium;
	style.Colors[ImGuiCol_Border] = theme.border_color;
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = theme.bg_light;
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(
		theme.bg_light.x + 0.05f, theme.bg_light.y + 0.05f, theme.bg_light.z + 0.05f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = theme.accent_primary;
	style.Colors[ImGuiCol_TitleBg] = theme.bg_dark;
	style.Colors[ImGuiCol_TitleBgActive] = theme.bg_dark;
	style.Colors[ImGuiCol_TitleBgCollapsed] = theme.bg_dark;
	style.Colors[ImGuiCol_MenuBarBg] = theme.bg_medium;
	style.Colors[ImGuiCol_ScrollbarBg] = theme.bg_medium;
	style.Colors[ImGuiCol_ScrollbarGrab] = theme.accent_primary;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = theme.accent_secondary;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = theme.accent_secondary;
	style.Colors[ImGuiCol_CheckMark] = theme.accent_primary;
	style.Colors[ImGuiCol_SliderGrab] = theme.accent_primary;
	style.Colors[ImGuiCol_SliderGrabActive] = theme.accent_secondary;
	style.Colors[ImGuiCol_Button] = theme.accent_primary;
	style.Colors[ImGuiCol_ButtonHovered] = theme.accent_secondary;
	style.Colors[ImGuiCol_ButtonActive] = theme.accent_secondary;
	style.Colors[ImGuiCol_Header] = theme.accent_primary;
	style.Colors[ImGuiCol_HeaderHovered] = theme.accent_secondary;
	style.Colors[ImGuiCol_HeaderActive] = theme.accent_secondary;
	style.Colors[ImGuiCol_Separator] = theme.border_color;
	style.Colors[ImGuiCol_SeparatorHovered] = theme.accent_primary;
	style.Colors[ImGuiCol_SeparatorActive] = theme.accent_secondary;
	style.Colors[ImGuiCol_ResizeGrip] = theme.accent_primary;
	style.Colors[ImGuiCol_ResizeGripHovered] = theme.accent_secondary;
	style.Colors[ImGuiCol_ResizeGripActive] = theme.accent_secondary;
	style.Colors[ImGuiCol_Tab] = theme.bg_light;
	style.Colors[ImGuiCol_TabHovered] = theme.accent_primary;
	style.Colors[ImGuiCol_TabActive] = theme.accent_primary;
	style.Colors[ImGuiCol_TabUnfocused] = theme.bg_light;
	style.Colors[ImGuiCol_TabUnfocusedActive] = theme.accent_primary;
	style.Colors[ImGuiCol_Text] = theme.text_primary;
	style.Colors[ImGuiCol_TextDisabled] = theme.text_secondary;
	style.Colors[ImGuiCol_NavHighlight] = theme.accent_primary;
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.60f);
}

bool tempb = true;
float tempf = 10.0f;

// Theme editor UI
static void RenderThemeEditor()
{
	if (!show_theme_editor)
		return;
	ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Theme Editor", &show_theme_editor, ImGuiWindowFlags_NoCollapse)) {
		// Theme selector
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "SELECT THEME");
		ImGui::Separator();
		std::vector<const char *> theme_names;
		for (const auto &theme : themes)
			theme_names.push_back(theme.name.c_str());
		theme_names.push_back("Custom Theme");
		if (ImGui::Combo("Theme", &current_theme_index, theme_names.data(),
				 theme_names.size()))
			theme_modified = true;
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "CUSTOMIZE COLORS");
		ImGui::Separator();
		Theme *editing_theme = (current_theme_index < themes.size())
					       ? &themes[current_theme_index]
					       : &custom_theme;
		if (editing_theme) {
			ImGui::BeginChild("ColorEditor", ImVec2(0, 300), true);
			ImGui::Text("Background Colors");
			ImGui::ColorEdit4("Dark Background", (float *)&editing_theme->bg_dark);
			ImGui::ColorEdit4("Medium Background", (float *)&editing_theme->bg_medium);
			ImGui::ColorEdit4("Light Background", (float *)&editing_theme->bg_light);
			ImGui::Spacing();
			ImGui::Text("Accent Colors");
			ImGui::ColorEdit4("Primary Accent",
					  (float *)&editing_theme->accent_primary);
			ImGui::ColorEdit4("Secondary Accent",
					  (float *)&editing_theme->accent_secondary);
			ImGui::Spacing();
			ImGui::Text("Text Colors");
			ImGui::ColorEdit4("Primary Text", (float *)&editing_theme->text_primary);
			ImGui::ColorEdit4("Secondary Text",
					  (float *)&editing_theme->text_secondary);
			ImGui::Spacing();
			ImGui::Text("Status Colors");
			ImGui::ColorEdit4("Success", (float *)&editing_theme->success_color);
			ImGui::ColorEdit4("Warning", (float *)&editing_theme->warning_color);
			ImGui::ColorEdit4("Error", (float *)&editing_theme->error_color);
			ImGui::Spacing();
			ImGui::Text("Border Color");
			ImGui::ColorEdit4("Border", (float *)&editing_theme->border_color);
			ImGui::EndChild();
			ImGui::Spacing();
			ImGui::Text("Rounding Settings");
			ImGui::SliderFloat("Window Rounding", &editing_theme->window_rounding, 0.0f,
					   20.0f, "%.1f");
			ImGui::SliderFloat("Frame Rounding", &editing_theme->frame_rounding, 0.0f,
					   20.0f, "%.1f");
			ImGui::SliderFloat("Button Rounding", &editing_theme->button_rounding, 0.0f,
					   20.0f, "%.1f");
			ImGui::Spacing();
			if (current_theme_index == themes.size()) {
				ImGui::Text("Custom Theme Name");
				static char theme_name[256];
				strncpy(theme_name, editing_theme->name.c_str(),
					sizeof(theme_name) - 1);
				if (ImGui::InputText("##ThemeName", theme_name, sizeof(theme_name)))
					editing_theme->name = theme_name;
			}
			ImGui::Spacing();
			if (ImGui::Button("Reset to Default", ImVec2(150, 30))) {
				if (current_theme_index < themes.size())
					themes[current_theme_index] = defaultthemes[current_theme_index];
				else
					custom_theme = defaultthemes[0];
				theme_modified = true;
			}
			ImGui::Spacing();
			ImGui::Text("Preview");
			ImGui::Separator();
			ImGui::BeginChild("Preview", ImVec2(0, 135), true);
			ApplyTheme(*editing_theme);
			ImGui::Text("Sample Text");
			ImGui::Button("Sample Button");
			ImGui::SameLine();
			
			ImGui::Checkbox("Sample Checkbox", &tempb);
			
			ImGui::SliderFloat("Sample Slider", &tempf, 0.0f, 20.0f);
			ImGui::EndChild();
		}
	}
	ImGui::End();
}