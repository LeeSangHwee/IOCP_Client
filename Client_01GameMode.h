// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Client_01.h"
#include "GameFramework/GameModeBase.h"
#include "Client_01GameMode.generated.h"

UCLASS(minimalapi)
class AClient_01GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AClient_01GameMode();
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	virtual void Tick(float Deltatime) override;
};
