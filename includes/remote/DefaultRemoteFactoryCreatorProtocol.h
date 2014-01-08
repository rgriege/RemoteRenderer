#ifndef _DEFAULT_REMOTE_FACTORY_CREATOR_PROTOCOL_H
#define _DEFAULT_REMOTE_FACTORY_CREATOR_PROTOCOL_H

#include "OIS.h"
#include "remote/RemotePrereqs.h"

namespace OIS
{	
	class DefaultRemoteFactoryCreatorProtocol : public RemoteFactoryCreatorProtocol
	{
	public:
		virtual WindowDataRequest createFreeDeviceListRequest() const { WindowDataRequest request = { 0, 0 }; return request; }
		virtual WindowDataRequest createTotalDeviceCountRequest(Type iType) const { WindowDataRequest request = { 0, 0 }; return request; }
		virtual WindowDataRequest createFreeDeviceCountRequest(Type iType) const { WindowDataRequest request = { 0, 0 }; return request; }

		virtual DeviceList parseFreeDeviceListResponse(WindowDataResponse response) const { return DeviceList(); }
		virtual int parseTotalDeviceCountResponse(WindowDataResponse response) const { return 0; }
		virtual int parseFreeDeviceCountResponse(WindowDataResponse response) const { return 0; }
	};
}

#endif