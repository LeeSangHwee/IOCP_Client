#pragma once

// ���� ���
#include "Client_01.h"
// ��Ʈ��ũ ���
#include "Networking.h"	
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "CCirQueue.h"
#include "Server_CustomData.h"
// ���� ���
#include "GameFramework/Actor.h"
#include "Server_Client.generated.h"

#define MAX_BUFFER 300000

UCLASS(Blueprintable)
class CLIENT_01_API AServer_Client : public AActor
{
	GENERATED_BODY()
	
//////////////// ���� �� �Ҹ��� //////////////////////
public:	
	AServer_Client();
	~AServer_Client();

///////////////////// ���� ///////////////////////////
public:
	// ������ ���� �� ����
	FSocket* Socket;

	// ������ ���� �� ���� �ּ�
	TSharedPtr<FInternetAddr> addr;

	// ���� ���� ũ��
	uint8 buf[MAX_BUFFER];

	// ��Ŷ
	CCirQueue g_Queue;

	// ĳ����
	class AClient_01Character* TestChar;
	class AClient_01Character* TestPawn[4];

	// ���� ���� �ο�
	int ConnectCount;

	// �ٸ� ���� ���� ����
	int SpawnCount;
	int TempSpawnCount;

	// ��Ʈ�ѷ� ����
	bool ControllerState;

///////////////////// �Լ� ///////////////////////////
public:
	// �̱������� ������ �޸𸮿� �Ҵ�
	static AServer_Client* GetSingleton()
	{
		static AServer_Client* instance;

		if (instance == NULL) instance = NewObject<AServer_Client>();
		return instance;
	}

	// Ŭ���� ���۰� ���ÿ� ȣ��
	virtual void BeginPlay() override;
	// Ŭ���� ���� �� �ݺ� ȣ��
	virtual void Tick(float Deltatime) override;

public:
	// ���� �ʱ�ȭ
	bool InitSocket();
	
	// ������ �����ϱ� ���� ȣ��
	bool Connect(FString pszIP, int nPort);

	// Custom Value
	bool SendData_Login();
	bool SendData_SpawnUser();
	bool SendData_PosAndRot(int _PlayerNumber, FVector vPos, FRotator rRot);
};
