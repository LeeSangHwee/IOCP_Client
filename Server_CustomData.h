#pragma once
#include "Client_01.h"
#include "Server_CustomData.generated.h"

////////////////////////////// Packet ID //////////////////////////////
#define PKT_TEST_CONNECT 	0xa0000001 // TEST 패킷
#define PKT_TEST_SPAWN 		0xa0000002 // TEST 패킷
#define PKT_TEST_POS 		0xa0000003 // TEST 패킷

////////////////////////////// Packet Data //////////////////////////////
// Vector & Rotation
struct F_gVector
{
	float Pos_x;
	float Pos_y;
	float Pos_z;
};

struct F_gRotator
{
	float Rot_Yaw;
	float Rot_Pitch;
	float Rot_Roll;
};

// Base Packet
USTRUCT(BlueprintType)
struct F_tgPacketHeader
{
	GENERATED_USTRUCT_BODY()

	UINT32 PktID;
	UINT16 PktSize;

	F_tgPacketHeader() {};
	~F_tgPacketHeader() {};
};

USTRUCT(BlueprintType)
struct F_gUserConnect : public F_tgPacketHeader
{
	GENERATED_USTRUCT_BODY()

	UINT16 ConnectCount;

	F_gUserConnect() {};
	~F_gUserConnect() {};
};

USTRUCT(BlueprintType)
struct F_gUserSpawn : public F_tgPacketHeader
{
	GENERATED_USTRUCT_BODY()

	UINT16 SpawnCount;

	F_gUserSpawn() {};
	~F_gUserSpawn() {};
};

USTRUCT(BlueprintType)
struct F_gUserPos : public F_tgPacketHeader
{
	GENERATED_USTRUCT_BODY()

	int m_iPlayerNumber;
	F_gVector m_UserPos;
	F_gRotator m_UserRot;

	F_gUserPos() {};
	~F_gUserPos() {};
};