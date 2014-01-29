#ifndef _DEFAULT_REMOTE_FACTORY_CREATOR_PROTOCOL_H
#define _DEFAULT_REMOTE_FACTORY_CREATOR_PROTOCOL_H

#include "OIS.h"
#include "RemoteOISPrereqs.h"

namespace RemoteOIS
{	
	class _RemoteOISExport DefaultFactoryCreatorProtocol : public FactoryCreatorProtocol
	{
	public:
		virtual WindowDataRequest createFreeDeviceListRequest() const { WindowDataRequest request = { 0, 0 }; return request; }
		virtual WindowDataRequest createTotalDeviceCountRequest(OIS::Type iType) const { WindowDataRequest request = { 0, 0 }; return request; }
		virtual WindowDataRequest createFreeDeviceCountRequest(OIS::Type iType) const { WindowDataRequest request = { 0, 0 }; return request; }

		virtual OIS::DeviceList parseFreeDeviceListResponse(WindowDataResponse response) const { return OIS::DeviceList(); }
		virtual int parseTotalDeviceCountResponse(WindowDataResponse response) const { return 0; }
		virtual int parseFreeDeviceCountResponse(WindowDataResponse response) const { return 0; }
	};
}

#endif