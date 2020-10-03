#pragma once

// 메인 헤더
#include "Client_01.h"
// 네트워크 헤더
#include "Networking.h"	
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "CCirQueue.h"
#include "Server_CustomData.h"
// 엔진 헤더
#include "GameFramework/Actor.h"
#include "Server_Client.generated.h"

#define MAX_BUFFER 300000

UCLASS(Blueprintable)
class CLIENT_01_API AServer_Client : public AActor
{
	GENERATED_BODY()
	
//////////////// 생성 및 소멸자 //////////////////////
public:	
	AServer_Client();
	~AServer_Client();

///////////////////// 변수 ///////////////////////////
public:
	// 서버와 연결 할 소켓
	FSocket* Socket;

	// 서버와 연결 할 원격 주소
	TSharedPtr<FInternetAddr> addr;

	// 받을 버퍼 크기
	uint8 buf[MAX_BUFFER];

	// 패킷
	CCirQueue g_Queue;

	// 캐릭터
	class AClient_01Character* TestChar;
	class AClient_01Character* TestPawn[4];

	// 서버 접속 인원
	int ConnectCount;

	// 다른 유저 스폰 숫자
	int SpawnCount;
	int TempSpawnCount;

	// 컨트롤러 유무
	bool ControllerState;

///////////////////// 함수 ///////////////////////////
public:
	// 싱글턴으로 소켓을 메모리에 할당
	static AServer_Client* GetSingleton()
	{
		static AServer_Client* instance;

		if (instance == NULL) instance = NewObject<AServer_Client>();
		return instance;
	}

	// 클래스 시작과 동시에 호출
	virtual void BeginPlay() override;
	// 클래스 시작 후 반복 호출
	virtual void Tick(float Deltatime) override;

public:
	// 윈속 초기화
	bool InitSocket();
	
	// 서버와 연결하기 위해 호출
	bool Connect(FString pszIP, int nPort);

	// Custom Value
	bool SendData_Login();
	bool SendData_SpawnUser();
	bool SendData_PosAndRot(int _PlayerNumber, FVector vPos, FRotator rRot);
};
