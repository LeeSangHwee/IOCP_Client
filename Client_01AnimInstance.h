// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Client_01.h"
#include "Animation/AnimInstance.h"
#include "Client_01AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_01_API UClient_01AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UClient_01AnimInstance();

	//모션 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool Is_Walk;
};
