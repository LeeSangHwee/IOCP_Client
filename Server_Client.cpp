#include "Server_Client.h"
// ĳ���� ���
#include "Client_01Character.h"
#include "Client_01AnimInstance.h"

// ������
AServer_Client::AServer_Client()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize
	Socket = NULL;

	for (int i = 0; i < 4; i++) { TestPawn[i] = nullptr; }

	// ���� ���� �� ����
	ConnectCount = 0;
	SpawnCount = 0;
	TempSpawnCount = 0;
	ControllerState = false;
}

// �Ҹ���
AServer_Client::~AServer_Client()
{
	delete Socket;
	Socket = nullptr;
}

// ����
void AServer_Client::BeginPlay()
{
	Super::BeginPlay();

	InitSocket();
	Connect(TEXT("192.168.123.100"), 9000);
}

// �ݺ�
void AServer_Client::Tick(float Deltatime)
{
	Super::Tick(Deltatime);
	TestChar = Cast<AClient_01Character>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	int32 BytesRead = sizeof(buf);
	F_tgPacketHeader* pHeader = NULL;
	uint32 Size;

	//������ �����鼭 �����.
	while (Socket->HasPendingData(Size)) //tcp ���� ������ ó��
	{
		int32 Read = 0;
		if (Socket->Recv(buf, BytesRead, Read)) pHeader = (F_tgPacketHeader*)&buf;

		g_Queue.OnPutData((char*)buf, Read);
		pHeader = g_Queue.GetPacket();

		while (pHeader != NULL)
		{
			// ���� �α��� ����
			if (pHeader->PktID == PKT_TEST_CONNECT)
			{
				ConnectCount = ((F_gUserConnect*)pHeader)->ConnectCount;
				printf("ConnectCount : %d", ConnectCount);
				
				if (ConnectCount <= 4)
				{
					if (ControllerState == false)
					{
						FActorSpawnParameters spawnParams;
						TestPawn[ConnectCount - 1] = GetWorld()->SpawnActor<AClient_01Character>(AClient_01Character::StaticClass(), FVector(-1000, -500 + (200 * (ConnectCount - 1)), 250), FRotator(0, 0, 0), spawnParams);
						TestPawn[ConnectCount - 1]->iPlayerNumber = ConnectCount; // iPlayerNumber : 1~4

						// ��Ʈ�ѷ��� ���ϴ� Pawn ����
						GetWorld()->GetFirstPlayerController()->Possess((APawn*)TestPawn[ConnectCount - 1]);
						TestPawn[ConnectCount - 1]->myPlayer = true;
						ControllerState = true;

						// ���Ϳ� ���� ����
						TestPawn[ConnectCount - 1]->SocketClass = this;
						print("Create my Character");
					}
					else print("Controller state is true");

					// Spawn Other User
					print("Send Spawn Data");
					SendData_SpawnUser();
				}
				else print("Connect count is over");
			}
			// ���� ����
			else if (pHeader->PktID == PKT_TEST_SPAWN)
			{
				SpawnCount = ((F_gUserSpawn*)pHeader)->SpawnCount;
				printf("SpawnCount : %d", SpawnCount);

				if (SpawnCount <= 4)
				{
					for (int i = 0; i < SpawnCount; i++)
					{
						if (TestPawn[i] == nullptr)
						{
							FActorSpawnParameters spawnParams;
							TestPawn[i] = GetWorld()->SpawnActor<AClient_01Character>(AClient_01Character::StaticClass(), FVector(-1000, -500 + (200 * i), 250), FRotator(0, 0, 0), spawnParams);
							TestPawn[i]->iPlayerNumber = i + 1; // iPlayerNumber : 1~4
							printf("Create another User : Num %d", TestPawn[i]->iPlayerNumber);
						}
						else printf("Values exist in the array of the corresponding Player Number %d", TestPawn[i]->iPlayerNumber);
					}
				}
				else print("Spawn count is over");
			}
			// ĳ���� �̵�
			else if (pHeader->PktID == PKT_TEST_POS)
			{
				FVector F_ActorLocation;
				FRotator F_ActorRotation;

				if ((TestPawn[((F_gUserPos*)pHeader)->m_iPlayerNumber - 1] != nullptr) && (TestPawn[((F_gUserPos*)pHeader)->m_iPlayerNumber - 1] == false))
				{
					printf("Set Pos Player Num : %d", ((F_gUserPos*)pHeader)->m_iPlayerNumber);

					F_ActorLocation.X = ((F_gUserPos*)pHeader)->m_UserPos.Pos_x;
					F_ActorLocation.Y = ((F_gUserPos*)pHeader)->m_UserPos.Pos_y;
					F_ActorLocation.Z = ((F_gUserPos*)pHeader)->m_UserPos.Pos_z;

					F_ActorRotation.Yaw = ((F_gUserPos*)pHeader)->m_UserRot.Rot_Yaw;
					F_ActorRotation.Pitch = ((F_gUserPos*)pHeader)->m_UserRot.Rot_Pitch;
					F_ActorRotation.Roll = ((F_gUserPos*)pHeader)->m_UserRot.Rot_Roll;

					// Set Postion & Set Rotation
					TestPawn[((F_gUserPos*)pHeader)->m_iPlayerNumber - 1]->SetActorLocationAndRotation(F_ActorLocation, F_ActorRotation);
				}
				else print("This Pawn is NULL");
			}
			else print("PKT NULL");

			g_Queue.OnPopData(pHeader->PktSize);
			pHeader = g_Queue.GetPacket();
		}
	}
}

// �ʱ�ȭ
bool AServer_Client::InitSocket()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
	return true;
}

// ���� ����
bool AServer_Client::Connect(FString pszIP, int nPort)
{
	FString address = pszIP;
	int32 port = nPort;
	FIPv4Address ip;
	FIPv4Address::Parse(address, ip);

	addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(port);

	bool connected = Socket->Connect(*addr);
	if (!connected)
	{
		printc("ERROR : Connect Fail", FColor::Red);
		return false;
	}
	else SendData_Login();

	return true;
}

// ���� �α���
bool AServer_Client::SendData_Login()
{
	int32 BytesSent = 0;
	F_gUserConnect TestDataLogin;

	TestDataLogin.PktID = PKT_TEST_CONNECT;
	TestDataLogin.ConnectCount = ConnectCount;
	TestDataLogin.PktSize = sizeof(TestDataLogin);

	Socket->SendTo((uint8*)&TestDataLogin, sizeof(TestDataLogin), BytesSent, *addr);

	return true;
}

// ���� ����
bool AServer_Client::SendData_SpawnUser()
{
	int32 BytesSent = 0;
	F_gUserSpawn TestDataSpawn;

	TestDataSpawn.PktID = PKT_TEST_SPAWN;
	TestDataSpawn.SpawnCount = SpawnCount;
	TestDataSpawn.PktSize = sizeof(TestDataSpawn);

	Socket->SendTo((uint8*)&TestDataSpawn, sizeof(TestDataSpawn), BytesSent, *addr);

	return true;
}

// ĳ���� ��ġ �� ȸ�� �� ����
bool AServer_Client::SendData_PosAndRot(int _PlayerNumber, FVector vPos, FRotator rRot)
{
	int32 BytesSent = 0;
	F_gUserPos TestDataPos;

	TestDataPos.PktID = PKT_TEST_POS;
	TestDataPos.m_iPlayerNumber = _PlayerNumber;
	TestDataPos.m_UserPos.Pos_x = vPos.X;
	TestDataPos.m_UserPos.Pos_y = vPos.Y;
	TestDataPos.m_UserPos.Pos_z = vPos.Z;
	TestDataPos.m_UserRot.Rot_Yaw = rRot.Yaw;
	TestDataPos.m_UserRot.Rot_Pitch = rRot.Pitch;
	TestDataPos.m_UserRot.Rot_Roll = rRot.Roll;
	TestDataPos.PktSize = sizeof(TestDataPos);

	Socket->SendTo((uint8*)&TestDataPos, sizeof(TestDataPos), BytesSent, *addr);

	return true;
}
