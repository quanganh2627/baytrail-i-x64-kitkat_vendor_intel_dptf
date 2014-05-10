/******************************************************************************
** Copyright (c) 2013 Intel Corporation All Rights Reserved
**
** Licensed under the Apache License, Version 2.0 (the "License"); you may not
** use this file except in compliance with the License.
**
** You may obtain a copy of the License at
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
** WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**
** See the License for the specific language governing permissions and
** limitations under the License.
**
******************************************************************************/

#ifndef _ESIF_CCB_SYSTEM_H_
#define _ESIF_CCB_SYSTEM_H_

#include "esif.h"
#include "esif_data.h"


#ifdef ESIF_ATTR_OS_WINDOWS
#include "powrprof.h"
#include "win\dppe.h"
#endif

// Execute System Command. Encapsulate to avoid Linux warnings when using gcc -O9
static ESIF_INLINE void esif_ccb_system(const char *cmd)
{
	int rc = system(cmd);
	UNREFERENCED_PARAMETER(rc);
}


#define system(cmd) esif_ccb_system(cmd)

// Reboot
static ESIF_INLINE void esif_ccb_reboot()
{
#if defined(ESIF_ATTR_OS_WINDOWS)
	system("shutdown /r /t 0");
#else
	system("reboot");
#endif
}


#if defined(ESIF_ATTR_OS_WINDOWS)
//
// _THERMAL_EVENT and PowerReportThermalEvent have been defined in WinBlue WDK
//
#ifndef THERMAL_EVENT_VERSION

#define THERMAL_EVENT_VERSION 1
typedef struct _THERMAL_EVENT {
	ULONG   Version;
	ULONG   Size;
	ULONG   Type;
	ULONG   Temperature;
	ULONG   TripPointTemperature;
	LPWSTR  Initiator;
} THERMAL_EVENT, *PTHERMAL_EVENT;

#endif

typedef DWORD (WINAPI * PFNPOWERREPORTTHERMALEVENT)(
	PTHERMAL_EVENT Event);

#define THERMAL_EVENT_SHUTDOWN 0
#define THERMAL_EVENT_HIBERNATE 1
#define THERMAL_EVENT_UNSPECIFIED 0xffffffff

#endif


static ESIF_INLINE void esif_guid_to_ms_guid(esif_guid_t *guid)
{
#ifdef ESIF_ATTR_OS_WINDOWS
	u8 *ptr = (u8 *)guid;
	u8 b[ESIF_GUID_LEN] = {0};

	ESIF_TRACE_DEBUG("%s:\n", ESIF_FUNC);
	esif_ccb_memcpy(&b, ptr, ESIF_GUID_LEN);

	*(ptr + 0) = b[3];
	*(ptr + 1) = b[2];
	*(ptr + 2) = b[1];
	*(ptr + 3) = b[0];
	*(ptr + 4) = b[5];
	*(ptr + 5) = b[4];
	*(ptr + 6) = b[7];
	*(ptr + 7) = b[6];
#endif
}

// Enter S0 Shutdown
static ESIF_INLINE void esif_ccb_shutdown(
	UInt32 temperature,
	UInt32 tripPointTemperature
	)
{
#if defined(ESIF_ATTR_OS_WINDOWS)

	/*
	** Report Thermal Event Before Shutdown With This UNDOCUMENTED API
	** Only Available in Windows 8.1/Blue.
	*/

	HMODULE hModule = LoadLibrary(L"powrprof.dll");
	if (NULL != hModule) {
		PFNPOWERREPORTTHERMALEVENT pfnPowerReportThermalEvent = (PFNPOWERREPORTTHERMALEVENT)GetProcAddress(
				hModule,
				"PowerReportThermalEvent");

		if (NULL != pfnPowerReportThermalEvent) {
			THERMAL_EVENT t_event = {0};
			t_event.Version     = THERMAL_EVENT_VERSION;
			t_event.Size        = sizeof(THERMAL_EVENT);
			t_event.Type        = THERMAL_EVENT_SHUTDOWN;
			t_event.Temperature = temperature;
			t_event.TripPointTemperature = tripPointTemperature;
			t_event.Initiator   = L"Intel(R) Dynamic Platform Thermal Framework";

			/* Best effort we are shutting down anyway */
			pfnPowerReportThermalEvent(&t_event);
		}
		FreeLibrary(hModule);
	}
	system("shutdown /s /f /t 0");
#elif defined(ESIF_ATTR_OS_CHROME)
	system("shutdown -P now");
#elif defined(ESIF_ATTR_OS_ANDROID)
	system("reboot -p");
#else
	system("shutdown -h now");
#endif
}


// Enter S4 Hibernation
static ESIF_INLINE void esif_ccb_hibernate()
{
#if defined(ESIF_ATTR_OS_WINDOWS)
	SetSuspendState(1, 1, 0);
#elif defined(ESIF_ATTR_OS_CHROME)
	/* NA */
#else
	system("pm-hibernate");
#endif
}


// Enter S3 or CS
static ESIF_INLINE void esif_ccb_suspend()
{
#if defined(ESIF_ATTR_OS_WINDOWS)
	SetSuspendState(0, 1, 0);
#elif defined(ESIF_ATTR_OS_CHROME)
	system("powerd_dbus_suspend");
#elif defined(ESIF_ATTR_OS_ANDROID)
	system("input keyevent 26");
#else
	system("pm-suspend");
#endif
}

#ifdef ESIF_ATTR_OS_WINDOWS
#define esif_ccb_remove_power_setting(req_ptr) \
	esif_ccb_remove_power_setting_win(req_ptr)

#define system_clear_ctdp_names() \
	system_clear_ctdp_names_win()

#define system_set_ctdp_name(request, instance) \
	system_set_ctdp_name_win(request, instance)

#define system_get_ctdp_name(response, instance) \
	system_get_ctdp_name_win(response, instance)

#else
#define esif_ccb_remove_power_setting(req_ptr) \
	ESIF_E_ACTION_NOT_IMPLEMENTED

#define system_clear_ctdp_names() \
	ESIF_E_ACTION_NOT_IMPLEMENTED

#define system_set_ctdp_name(request, instance) \
	ESIF_E_ACTION_NOT_IMPLEMENTED

#define system_get_ctdp_name(response, instance) \
	ESIF_E_ACTION_NOT_IMPLEMENTED

#endif

#endif /* _ESIF_CCB_SYSTEM_H_ */

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
