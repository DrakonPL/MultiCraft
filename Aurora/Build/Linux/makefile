DEBUGFLAGS = -g #-DNDEBUG
CC       = g++
CFLAGS   = -Wall -DAURORA_UNIX_MAC -DUSE_FILE32API -DPTE_CLEANUP_CXX -fpermissive -I./../../ -I./../../libs/ -I./../../libs/sfml/include -I./../../libs/libzip/ -I./../../libs/tinyxml/ -I./../../libs/glew/include/GL -I./../../libs/cAudio/Headers -I./../../libs/cAudio/include $(DEBUGFLAGS)
LDFLAGS  = -L./../../libs/cAudio/lib/linux
EXECPATH = ./

EXEC = aurora
OBJS  = ../../Aurora/Math/MathLib.o \
../../Aurora/Math/MathUtils.o \
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
../../Aurora/Graphics/opengl/LegacyOpenGLRenderManager.o \
../../Aurora/Graphics/opengl/VertexBuffer.o \
../../Aurora/Graphics/models/ModelMd5.o \
../../Aurora/Graphics/models/ModelObj2.o \
../../Aurora/Graphics/models/ModelMesh.o \
../../Aurora/Graphics/models/MeshMaterial.o \
../../Aurora/Graphics/stb/fontstash.o \
../../Aurora/Graphics/stb/fontstash_renderer.o \
../../Aurora/Graphics/stb/stb_truetype.o \
../../Aurora/Graphics/TextureManager.o \
../../Aurora/Graphics/Camera.o \
../../Aurora/Graphics/Image.o \
../../Aurora/Graphics/SkyBox.o \
../../Aurora/Graphics/Sprite.o \
../../Aurora/Graphics/Sprite3D.o \
../../Aurora/Graphics/TrueTypeFont.o \
../../Aurora/Graphics/UnicodeFont.o \
../../Aurora/Graphics/RenderTexture.o \
../../Aurora/Audio/AudioManager.o \
../../Aurora/Audio/sfml/Sound.o \
../../Aurora/Audio/sfml/sfmlAudioManager.o \
../../Aurora/Network/Unix/SocketHelper.o \
../../Aurora/Network/pc/PcNetworkManager.o \
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
../../Aurora/System/pc/Clock.o \
../../Aurora/System/pc/PcSystemManager.o \
../../Aurora/Utils/md5.o \
../../Aurora/Utils/AES.o \
../../Aurora/Utils/Compression.o \
../../Aurora/Utils/Crypto.o \
../../Aurora/Utils/Logger.o \
../../Aurora/Utils/GameLoader.o \
../../Aurora/Utils/GameManager.o \
../../Aurora/Utils/pc/WinGameLoader.o \
../../Aurora/Threads/Thread.o \
../../Aurora/Threads/Mutex.o \
../../Aurora/Threads/Lock.o \
../../Aurora/Threads/Sleep.o \
../../Aurora/Threads/unix/ThreadImpl.o \
../../Aurora/Threads/unix/MutexImpl.o \
../../Aurora/Threads/unix/SleepImpl.o \
../../libs/libzip/ioapi.o \
../../libs/libzip/unzip.o \
../../libs/libzip/zip.o \
../../libs/tinyxml/tinyxml2.o \
../../Tests/ThreadTest.o \
../../Tests/SpriteTest.o \
../../Tests/SimpleTest.o \
../../Tests/UnicodeTest.o \
../../Tests/SkyBoxTest.o \
../../Starter.o

all: $(EXEC)

aurora: $(OBJS)
	$(CC) $(LDFLAGS) -o $(EXECPATH)/$@ $(OBJS) -lsfml-graphics -lsfml-window -lsfml-system -lz -lsfml-audio -lGLEW -lGLU  -lGL -lopenal

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	@rm -rf *.o
