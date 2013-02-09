TARGET = Aurora

OBJS = ../../Aurora/Math/MathLib.o \
../../Aurora/Math/PSP/MathUtils.o \
../../Aurora/Math/Vector2.o \
../../Aurora/Math/Vector3.o \
../../Aurora/Math/Vector4.o \
../../Aurora/Math/Matrix3.o \
../../Aurora/Math/Matrix4.o \
../../Aurora/Math/Quaternion.o \
../../Aurora/Math/BoundingBox.o \
../../Aurora/Math/BoundingSphere.o \
../../Aurora/Math/Plane.o \
../../Aurora/Math/Ray.o \
../../Aurora/Math/Frustum.o \
../../Aurora/Math/Polygon.o \
../../Aurora/Graphics/RenderManager.o \
../../Aurora/Graphics/gu/vram.o \
../../Aurora/Graphics/gu/GuRenderManager.o \
../../Aurora/Graphics/gu/VertexBuffer.o \
../../Aurora/Graphics/models/ModelMd5.o \
../../Aurora/Graphics/models/ModelObj2.o \
../../Aurora/Graphics/models/ModelMesh.o \
../../Aurora/Graphics/models/MeshMaterial.o \
../../Aurora/Graphics/TextureManager.o \
../../Aurora/Graphics/Camera.o \
../../Aurora/Graphics/Image.o \
../../Aurora/Graphics/Sprite.o \
../../Aurora/Graphics/Sprite3D.o \
../../Aurora/Graphics/TrueTypeFont.o \
../../Aurora/Graphics/RenderTexture.o \
../../Aurora/Audio/AudioManager.o \
../../Aurora/Audio/psp/Sound.o \
../../Aurora/Audio/psp/PspAudioManager.o \
../../Aurora/Audio/psp/Audio.o \
../../Aurora/Audio/psp/AudioMPEG.o \
../../Aurora/Network/Unix/SocketHelper.o \
../../Aurora/Network/psp/PspNetworkManager.o \
../../Aurora/Network/NetworkManager.o \
../../Aurora/Network/Http.o \
../../Aurora/Network/IPAddress.o \
../../Aurora/Network/Packet.o \
../../Aurora/Network/SelectorBase.o \
../../Aurora/Network/SocketTCP.o \
../../Aurora/Network/SocketUDP.o \
../../Aurora/Network/NetworkInputControllerClient.o \
../../Aurora/Network/NetworkInputControllerServer.o \
../../Aurora/System/DiskFile.o \
../../Aurora/System/File.o \
../../Aurora/System/FileManager.o \
../../Aurora/System/VFSFile.o \
../../Aurora/System/VFSPack.o \
../../Aurora/System/ZipFile.o \
../../Aurora/System/ZipPack.o \
../../Aurora/System/Timer.o \
../../Aurora/System/SystemManager.o \
../../Aurora/System/psp/Clock.o \
../../Aurora/System/psp/PspSystemManager.o \
../../Aurora/Utils/md5.o \
../../Aurora/Utils/AES.o \
../../Aurora/Utils/Compression.o \
../../Aurora/Utils/Crypto.o \
../../Aurora/Utils/Logger.o \
../../Aurora/Utils/GameLoader.o \
../../Aurora/Utils/GameManager.o \
../../Aurora/Utils/psp/PspCommon.o \
../../Aurora/Utils/psp/PspGameLoader.o \
../../Aurora/Threads/Thread.o \
../../Aurora/Threads/Mutex.o \
../../Aurora/Threads/Lock.o \
../../Aurora/Threads/Sleep.o \
../../Aurora/Threads/psp/ThreadImpl.o \
../../Aurora/Threads/psp/MutexImpl.o \
../../Aurora/Threads/psp/SleepImpl.o \
../../libs/libzip/ioapi.o \
../../libs/libzip/unzip.o \
../../libs/libzip/zip.o \
../../libs/tinyxml/tinyxml2.o \
../../Tests/ThreadTest.o \
../../Tests/SpriteTest.o \
../../Tests/SimpleTest.o \
../../Starter.o

INCDIR = $(PSPPATH)/include
INCDIR += ./../../ ./../../libs/ ./../../libs/libzip/ ./../../libs/tinyxml/ ./../../libs/tinyxml/ ./../../libs/pthread/psp
CFLAGS = -O2 -Wall -mpreferred-stack-boundary=4 -DSANE -DAURORA_PSP -DPTE_CLEANUP_CXX -DUSE_FILE32API #-enable-newlib-hw-fp -ffast-math
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -Wno-deprecated
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PSP_FW_VERSION=371
#PSP_LARGE_MEMORY = 1

LIBDIR += ./../../libs/pthread/psp
LDFLAGS =
LIBS  =  -lc -lstdc++ -lpng -lz -lpspaudio -lpspaudiocodec -lpspgum -lpspgu  -lpsprtc -lpspctrl -lm -lpspvfpu -lpspsdk -lpsppower -lpspnet -lpthread-psp


EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Aurora
#PSP_EBOOT_ICON = ICON0.png
#PSP_EBOOT_ICON1 = ICON1.PMF
#PSP_EBOOT_PIC1 = PIC1.png

PSPSDK=$(shell c:/pspsdk/bin/psp-config.exe -p)
include $(PSPSDK)/lib/build.mak
