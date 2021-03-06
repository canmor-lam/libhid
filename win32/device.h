#ifndef	HID_WIN32_DEVICE_H
#define	HID_WIN32_DEVICE_H

#include <string>

#include <device.h>

#if (WINVER < 0x0500)
#error Need to compile for Win2K or later
#endif

extern "C"
{
#include <windows.h>
#include <setupapi.h>

#ifdef __GNUC__			// MinGW
#include <ddk/hidpi.h>
#include <ddk/hidsdi.h>
#else				// WinDDK
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <hidpi.h>
#include <hidsdi.h>
#endif
}

#include "element.h"

typedef std::basic_string<TCHAR> tstring; // string of TCHARs

namespace HID
{
    namespace win32
    {
	class device_type;
    }
}

class HID::win32::device_type : public HID::device_type
{
    const tstring _tpath;
    HANDLE	handle;
    HIDD_ATTRIBUTES	_attributes;
    OVERLAPPED		overlapped;
    OVERLAPPED		_write_overlapped;
    PHIDP_PREPARSED_DATA    _preparsedData;
    HIDP_CAPS*	_capabilities;

    uint8_t*	_bufferFeatureReport;
    uint8_t*	_bufferInputReport;
    uint8_t*	_bufferOutputReport;

    // Private getters
    uint8_t*	bufferFeatureReport();
    uint8_t*	bufferInputReport();
    uint8_t*	bufferOutputReport();
    const HIDD_ATTRIBUTES& attributes();
    HIDP_CAPS*	capabilities();
    PHIDP_PREPARSED_DATA preparsedData();
    void button_elements(elements_type&);
    void value_elements(elements_type&);
    void link_subcollections(elements_type&, std::vector<HIDP_LINK_COLLECTION_NODE>&, size_t index = 0);

public:
    device_type(const TCHAR*);
    device_type(const TCHAR*, const HIDD_ATTRIBUTES&);
    virtual ~device_type()
    {
	if( _bufferFeatureReport )
	    delete[] _bufferFeatureReport;
	if( _bufferInputReport )
	    delete[] _bufferInputReport;
	if( _bufferOutputReport )
	    delete[] _bufferOutputReport;
	if( _capabilities )
	    delete[] _capabilities;
	if( _preparsedData )
	    HidD_FreePreparsedData(_preparsedData);
	close();
    }

    virtual void close();
    virtual bool open(OpenMode);
    virtual bool read(buffer_type&);
    virtual bool write(const buffer_type&);

    // Report elements
    virtual elements_type& elements();

    // Reports
    virtual bool feature(unsigned reportID, buffer_type&);
    virtual buffer_type feature(unsigned reportID);
    virtual buffer_type input(unsigned reportID);
    virtual bool output(unsigned reportID, buffer_type&);

    // Getters
    const std::string& manufacturer();
    const std::string& product();
    uint16_t productID() { return attributes().ProductID; }
    uint16_t usage();
    uint16_t usagePage();
    uint16_t vendorID()  { return attributes().VendorID; }
    uint16_t versionNumber() { return attributes().VersionNumber; }
};

#endif	// HID_WIN32_DEVICE_H
