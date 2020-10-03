#pragma once
#include "Client_01.h"
#include "Engine/GameInstance.h"
#include "VentusGameInstance.generated.h"

UCLASS()
class CLIENT_01_API UVentusGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UVentusGameInstance();

	virtual void BeginPlay() final;
};
