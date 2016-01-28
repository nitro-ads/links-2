#pragma once

// Win HTTP
#include "winhttprequest/include/httprequest_h.h"
#import <winhttprequest/lib/WinHttpRequest.tlb>

namespace WinHttp
{
	inline bool Get(
		const ATL::CString& _url,
		ATL::CString& _responseBytes,
		long& _httpStatusCode)
	{
		// Create a request object
		IWinHttpRequest* pRequest = NULL;
		HRESULT hr = ::CoCreateInstance(
			CLSID_WinHttpRequest, 
			NULL, 
			CLSCTX_INPROC_SERVER, 
			IID_IWinHttpRequest, 
			(void**)&pRequest);
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(pRequest);
		if (FAILED(hr) || !pRequest)
		{
			_com_error e(hr);
			LOG_ERROR(_T("Failed creating IWinHttpRequest instance w/err 0x%08lx: %s"), e.Error(), e.ErrorMessage());
			return false;
		}

		// Take ownership
		CComPtr<IWinHttpRequest> spRequest(pRequest);
		if (!spRequest)
		{
			LOG_ERROR(_T("Failed creating CComPtr<IWinHttpRequest> instance"));
			return false;
		}

		// Open a synchronous connection
		hr = spRequest->Open(
			_bstr_t(_T("GET")),
			_bstr_t(_url),
			CComVariant(VARIANT_FALSE));
		_ASSERTE(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			_com_error e(hr);
			LOG_ERROR(_T("Failed opening request w/err 0x%08lx: %s"), e.Error(), e.ErrorMessage());
			return false;
		}

		// Send the request and wait for response
		CComVariant varPostData;
		hr = spRequest->Send(varPostData);
		_ASSERTE(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			_com_error e(hr);
			LOG_ERROR(_T("Failed sending HTTP request w/err 0x%08lx: %s"), e.Error(), e.ErrorMessage());
			return false;
		}

		// Get response status
		_httpStatusCode = spRequest->GetStatus();
		CComBSTR bstrStatusText;
		hr = spRequest->get_StatusText(&bstrStatusText);
		_ASSERTE(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			_com_error e(hr);
			LOG_WARNING(_T("get_StatusText failed w/err 0x%08lx: %s"), e.Error(), e.ErrorMessage());
		}

		// Get response bytes
		CComBSTR bstrResponseBytes;
		hr = spRequest->get_ResponseText(&bstrResponseBytes);
		_ASSERTE(SUCCEEDED(hr));
		if (FAILED(hr))
		{
			_com_error e(hr);
			LOG_WARNING(_T("get_ResponseText failed w/err 0x%08lx: %s"), e.Error(), e.ErrorMessage());
		}

		// Output
		_responseBytes = bstrResponseBytes;
		//_responseBytes.assign(bstrResponseBytes.m_str, (bstrResponseBytes.m_str + bstrResponseBytes.Length()));

		return true;
	}
} // namespace WinHttp
