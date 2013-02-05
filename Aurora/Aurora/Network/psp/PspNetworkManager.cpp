#include <Aurora/Network/psp/PspNetworkManager.h>
#include <Aurora/Graphics/RenderManager.h>

#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>

namespace Aurora
{
	namespace Network
	{
		PspNetworkManager::PspNetworkManager()
		{
			_connectionActive = false;
		}

		bool PspNetworkManager::Init()
		{
			if(!_connectionActive)
			{
				_connectionActive = InitPSPNetwork();

				if(_connectionActive)
				{
					_connectionActive = ShowNetworkDialog();
				}
			}
		}

		bool PspNetworkManager::InitPSPNetwork()
		{
			int result;

			result = sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);

			if(result < 0)
				return false;

			result = sceUtilityLoadNetModule(PSP_NET_MODULE_INET);

			if(result < 0)
				return false;

			result = sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEURI);

			if(result < 0)
				return false;

			result = sceUtilityLoadNetModule(PSP_NET_MODULE_PARSEHTTP);

			if(result < 0)
				return false;

			result = sceUtilityLoadNetModule(PSP_NET_MODULE_HTTP);

			if(result < 0)
				return false;

			result = sceNetInit(128*1024, 42, 0, 42, 0);

			if(result < 0)
				return false;

			result = sceNetInetInit();

			if(result < 0)
				return false;

			result = sceNetApctlInit(0x10000, 48);

			if(result < 0)
				return false;

			result = sceNetResolverInit();

			if(result < 0)
				return false;

			return true;
		}

		bool PspNetworkManager::ShowNetworkDialog()
		{
			bool done = true;
			int result = -1;

			memset(&networkData, 0, sizeof(pspUtilityNetconfData));
			networkData.base.size = sizeof(networkData);
			sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &networkData.base.language);
			sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &networkData.base.buttonSwap);
			networkData.base.graphicsThread = 17;
			networkData.base.accessThread = 19;
			networkData.base.fontThread = 18;
			networkData.base.soundThread = 16;
			networkData.action = PSP_NETCONF_ACTION_CONNECTAP;

			struct pspUtilityNetconfAdhoc adhocparam;
			memset(&adhocparam, 0, sizeof(adhocparam));
			networkData.adhocparam = &adhocparam;

			sceUtilityNetconfInitStart(&networkData);

			while(done)
			{
				Graphics::RenderManager::Instance()->StartFrame();
				sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
				sceGuFinish();
				sceGuSync(0,0);
				//Graphics::RenderManager::Instance()->ClearScreen();


				switch(sceUtilityNetconfGetStatus())
				{
					case PSP_UTILITY_DIALOG_NONE:
					{
						result = networkData.base.result;
						done = false;
					}
					break;

					case PSP_UTILITY_DIALOG_VISIBLE:
						sceUtilityNetconfUpdate(1);
						break;

					case PSP_UTILITY_DIALOG_QUIT:
						sceUtilityNetconfShutdownStart();
					break;

					case PSP_UTILITY_DIALOG_FINISHED:
					break;

					default:
					break;
				}

				sceDisplayWaitVblankStart();
				sceGuSwapBuffers();
			}

			if(result == 0)
				return true;

			return false;
		}
	}
}

