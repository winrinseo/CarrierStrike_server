// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "CarrierStrikeGameInstance.h"

#define WIN32_LEAN_AND_MEAN
#include "../../ProudNet/include/ProudNetClient.h"

using namespace Proud;
/**
 *
 */
class CARRIERSTRIKECPP_API RemotePlayer
{
private:
public:
	FString playerName;
	FString team;

	//위치 , 속도 , 가속도 (가속도가 클 수록 송신량을 크게 올린다.)
	// P = (V * T) + P0
	CPositionFollower positionFollower;

	//바라보는 방향
	CAngleFollower pitchFollwer;
	CAngleFollower yawFollwer;
	CAngleFollower rollFollwer;
	FRotator prevRotation;

	float HP;

	AActor* actor;

	//구축함 정보 (항모일 경우만)
	std::vector<AActor*> myDestroyer;
public:
	RemotePlayer();
	~RemotePlayer();
	void DelActor();

	AActor* GetActor();

	//실질적인 위치를 업데이트한다.
	void Update();
};
