pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=C_ --send=S_
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=S_ --send=C_

IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../GameServer"
XCOPY /Y Enum.pb.cc "../../../GameServer"
XCOPY /Y Struct.pb.h "../../../GameServer"
XCOPY /Y Struct.pb.cc "../../../GameServer"
XCOPY /Y Protocol.pb.h "../../../GameServer"
XCOPY /Y Protocol.pb.cc "../../../GameServer"
XCOPY /Y ClientPacketHandler.h "../../../GameServer"


XCOPY /Y Enum.pb.h "../../../../P1/Source/P1/Network"
XCOPY /Y Enum.pb.cc "../../../../P1/Source/P1/Network"
XCOPY /Y Struct.pb.h "../../../../P1/Source/P1/Network"
XCOPY /Y Struct.pb.cc "../../../../P1/Source/P1/Network"
XCOPY /Y Protocol.pb.h "../../../../P1/Source/P1/Network"
XCOPY /Y Protocol.pb.cc "../../../../P1/Source/P1/Network"
XCOPY /Y ServerPacketHandler.h "../../../../P1/Source/P1/Network"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE