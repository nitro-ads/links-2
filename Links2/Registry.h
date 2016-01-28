#pragma once

namespace Registry
{
	// ------------------------------------------------------------------------
	inline bool OpenForReading(
		ATL::CRegKey& _rk,
		const HKEY& _key,
		const ATL::CString& _subKey, 
		DWORD _wow)
	{
		LONG rv = _rk.Open(_key, _subKey, KEY_READ | _wow);
		return (ERROR_SUCCESS == rv);
	}

	// ------------------------------------------------------------------------
	inline bool OpenForWriting(
		ATL::CRegKey& _rk,
		const HKEY& _key,
		const ATL::CString& _subKey, 
		DWORD _wow)
	{
		LONG rv = _rk.Open(_key, _subKey, KEY_WRITE | _wow);
		if (ERROR_SUCCESS == rv)
			return true;
		else if (ERROR_FILE_NOT_FOUND == rv)
		{
			// Create the key if it doesn't exist
			rv = _rk.Create(
				_key, 
				_subKey, 
				NULL, 
				REG_OPTION_NON_VOLATILE, 
				KEY_WRITE | _wow);
			// We may need admin rights to access this key
			ATLASSERT(ERROR_ACCESS_DENIED != rv);
			if (ERROR_SUCCESS == rv)
			{
				rv = _rk.Open(_key, _subKey, KEY_WRITE | _wow);
				return (ERROR_SUCCESS == rv);
			}
		}
		return false;
	}

	// ------------------------------------------------------------------------
	inline bool DeleteValue(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		const ATL::CString& _valName,
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForWriting(rk, _key, _subKey, _wow);
		if (rv)
		{
			LONG rv = rk.DeleteValue(_valName);
			return (ERROR_SUCCESS == rv);
		}
		return false;

	}

	// ------------------------------------------------------------------------
	inline bool DeleteSubKey(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForWriting(rk, _key, _subKey, _wow);
		if (rv)
		{
			LONG rv = rk.DeleteSubKey(_subKey);
			return (ERROR_SUCCESS == rv);
		}
		return false;

	}

	// ------------------------------------------------------------------------
	inline bool RecurseDeleteKey(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForWriting(rk, _key, _subKey, _wow);
		if (rv)
		{
			LONG rv = rk.RecurseDeleteKey(_subKey);
			return (ERROR_SUCCESS == rv);
		}
		return false;

	}

	// ------------------------------------------------------------------------
	inline bool QueryStringValue(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		const ATL::CString& _valName,
		ATL::CString& _value,
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForReading(rk, _key, _subKey, _wow);
		if (rv)
		{
			TCHAR szBuf[1024] = {0};
			DWORD nBufSize = sizeof(szBuf); // Size in bytes
			LONG rv = rk.QueryStringValue(_valName, szBuf, &nBufSize);
			if (ERROR_SUCCESS == rv)
			{
				_value = szBuf;
				return true;
			}
		}
		return false;
	}

	// ------------------------------------------------------------------------
	inline bool SetStringValue(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		const ATL::CString& _valName,
		const ATL::CString& _value,
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForWriting(rk, _key, _subKey, _wow);
		if (rv)
		{
			LONG rv = rk.SetStringValue(_valName, _value, REG_SZ);
			return (ERROR_SUCCESS == rv);
		}
		return false;
	}

	// ------------------------------------------------------------------------
	inline bool QueryDWORDValue(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		const ATL::CString& _valName,
		DWORD& _value,
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForReading(rk, _key, _subKey, _wow);
		if (rv)
		{
			LONG rv = rk.QueryDWORDValue(_valName, _value);
			return (ERROR_SUCCESS == rv);
		}
		return false;
	}

	// ------------------------------------------------------------------------
	inline bool SetDWORDValue(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		const ATL::CString& _valName,
		DWORD _value,
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForWriting(rk, _key, _subKey, _wow);
		if (rv)
		{
			LONG rv = rk.SetDWORDValue(_valName, _value);
			return (ERROR_SUCCESS == rv);
		}
		return false;
	}

	// ------------------------------------------------------------------------
	inline bool QueryQWORDValue(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		const ATL::CString& _valName,
		ULONGLONG& _value,
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForReading(rk, _key, _subKey, _wow);
		if (rv)
		{
			LONG rv = rk.QueryQWORDValue(_valName, _value);
			return (ERROR_SUCCESS == rv);
		}
		return false;
	}

	// ------------------------------------------------------------------------
	inline bool SetQWORDValue(
		const HKEY& _key,
		const ATL::CString& _subKey, 
		const ATL::CString& _valName,
		ULONGLONG _value,
		DWORD _wow = 0)
	{
		// Valid values for "_wow" are [0, KEY_WOW64_32KEY, KEY_WOW64_64KEY]
		ATLASSERT(0 == _wow || KEY_WOW64_32KEY == _wow || KEY_WOW64_64KEY == _wow);

		ATL::CRegKey rk;
		bool rv = OpenForWriting(rk, _key, _subKey, _wow);
		if (rv)
		{
			LONG rv = rk.SetQWORDValue(_valName, _value);
			return (ERROR_SUCCESS == rv);
		}
		return false;
	}
} // namespace Registry