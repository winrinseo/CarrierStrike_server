// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include "CarrierStrikeGameInstance.h"

#include "CarrierStrikeCPPGameModeBase.generated.h"

#define MOTHERSHIP_SPAWN_AREA 4
#define	FIGHTER_INIT_HP 100
#define MOTHERSHIP_INIT_HP 500

/**
 * 
 */
UCLASS()
class CARRIERSTRIKECPP_API ACarrierStrikeCPPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	float m_lastSendTime = -1;
	//액터를 생성하고 그 주소를 반환한다.
	AActor* SpawnActor(FString path, FVector originCoord, FVector relativeCoord , FRotator rot, std::vector<FName> tags, FActorSpawnParameters param);

public:
	UFUNCTION(BlueprintCallable, Category = "Login")
	void LoginRequest(FString id , FString password);


	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void MatchRequest();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void PartyInvite(FString id);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void PartyOut();

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void LobbyChatWithEveryPlayer(FString id , FString chat);

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void LobbyChatWithMyParty(FString id, FString chat);
	
	//인게임에 필요한 변수들을 초기화한다.
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void StartGame();

	//게임이 종료된 후 정보를 정리 후 점수를 반영한다.
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void EndGame();

	//유저들의 이름을 받아온다.
	UFUNCTION(BlueprintCallable, Category = "InGame")
	FString GetUserNameFromHostID();

	//전투기 액터 스폰
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void SpawnFighter(int64 remoteID);

	//모함 액터 스폰
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void SpawnMotherShip(int64 remoteID, int i , int idx);

	//모든 액터 스폰 ( 게임 시작 시 최초 실행 ) 및 게임의 초기설정
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void SpawnFighterAll();

	UFUNCTION(BlueprintCallable, Category = "InGame")
	float GetRemoteHP(int64 remoteID);

	//지속적으로 p2p 그룹에 내 위치 정보 송신
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Move(const FVector & c, const FVector& v, const FRotator & r , const float & speed);

	//매 프레임마다 객체들을 실제로 변화시킨다 (보류)
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Update(int64 remoteID);

	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "InGame")
	void AttackStop();

	UFUNCTION(BlueprintCallable, Category = "InGame")
	void AttackMissile(int weapon , int targetID);

	//플레어
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Flare();

	//피격 시
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void GetDamage(float damage);

	//내 액터가 터졌을 때
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Destroy();

	//리스폰
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Respawn();


	//구축함의 이동
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void DestroyerMove(int idx ,const FVector& c);

	//핑
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Ping(int code , const FVector & c);


	//상점
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Store(int code);

	//구매
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void MinusPoint(int point);

	//전체 채팅
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void AllChat(FString userName , FString chat);

	//전체 채팅
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void TeamChat(FString userName, FString chat);

	//킬
	UFUNCTION(BlueprintCallable, Category = "InGame")
	void Kill(int userThatKilledMe);

	//게임 끝
	void EndGame(FString defeatedTeam);
};
