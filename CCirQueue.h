// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Server_CustomData.h"

#define QUEUE_SIZE 20000

/**
 * 
 */
class CLIENT_01_API CCirQueue
{
public:
	CCirQueue();
	~CCirQueue();

	void ClearQueue() { InitQueue(); }
	void InitQueue();
	void InitZeroQueue();

	int OnPutData(char *pData, short recvsize);
	void OnPopData(short popsize);

	F_tgPacketHeader* GetPacket();

protected:
	char *m_pQueue;
	int m_sFront;
	int m_sRear;
};
