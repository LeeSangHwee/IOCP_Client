// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Client_01GameMode.h"
#include "TestPlayerController.h"

AClient_01GameMode::AClient_01GameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AClient_01GameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AClient_01GameMode::StartPlay()
{
	Super::StartPlay();
}

void AClient_01GameMode::Tick(float Deltatime)
{
	Super::Tick(Deltatime);
}
