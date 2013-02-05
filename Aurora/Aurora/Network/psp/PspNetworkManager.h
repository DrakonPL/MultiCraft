#ifndef PspNetworkManager_H
#define PspNetworkManager_H

#include <Aurora/Network/NetworkManager.h>

#include <malloc.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <psputility.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <string.h>

namespace Aurora
{
	namespace Network
	{
		class  PspNetworkManager : public NetworkManager
		{

		private:

			//dialog params
			pspUtilityMsgDialogParams dialog;
			pspUtilityNetconfData networkData;
			//void ConfigureDialog(pspUtilityMsgDialogParams *dialog, size_t dialog_size);

			bool InitPSPNetwork();
			bool ShowNetworkDialog();

		public:

			PspNetworkManager();

			bool Init();
		};
	}
}

#endif
