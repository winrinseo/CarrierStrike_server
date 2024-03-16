// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include <unordered_map>

#define WIN32_LEAN_AND_MEAN
#include "../../ProudNet/include/ProudNetClient.h"
#include "../../Common/C2S_common.h"
#include "../../Common/C2S_proxy.h"

#include "../../Common/S2C_common.h"
#include "../../Common/S2C_stub.h"

#include "../../Common/C2C_common.h"
#include "../../Common/C2C_proxy.h"
#include "../../Common/C2C_stub.h"

#include "RemotePlayer.h"
#include "RemoteActor.h"

#include "CarrierStrikeGameInstance.generated.h"

extern LobbyC2S::Proxy lobbyProxy;
extern P2P::Proxy p2pProxy;

namespace Game {
	UCarrierStrikeGameInstance* instance = nullptr;
}

class LobbyS2Cstub : public LobbyS2C::Stub 
{
public:
	DECRMI_LobbyS2C_successedLogin;
	DECRMI_LobbyS2C_failedLogin;
	DECRMI_LobbyS2C_completeMatching;
	DECRMI_LobbyS2C_joinGame;

};

class GameC2Cstub : public P2P::Stub {
public:
	DECRMI_P2P_move;
	DECRMI_P2P_attack;
	DECRMI_P2P_attackStop;
	DECRMI_P2P_attackMissile;
	DECRMI_P2P_flare;
	DECRMI_P2P_destroyActor;
	DECRMI_P2P_respawnActor;
	DECRMI_P2P_getDamageActor;

	DECRMI_P2P_destroyerMove;

	DECRMI_P2P_ping;

	//상점
	DECRMI_P2P_fuelUpgrade;
	DECRMI_P2P_hpUpgrade;
	DECRMI_P2P_respawnTime;
	DECRMI_P2P_hyperDetect;
	DECRMI_P2P_hpFix;
	DECRMI_P2P_missileTime;
	DECRMI_P2P_destroyerPlus;

	//채팅
	DECRMI_P2P_allChat;
	DECRMI_P2P_teamChat;

	//킬
	DECRMI_P2P_kill;
	DECRMI_P2P_getPoint;

	DECRMI_P2P_endGame;
};

UCLASS()
class CARRIERSTRIKECPP_API UCarrierStrikeGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	//클라이언트 객체
	Proud::CNetClient* m_pNetClient;

	//블루프린트 노출 속성
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "userInfo")
	int id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "userInfo")
	FString userName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "userInfo")
	int mmr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "teamInfo")
	int initCoord;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "gameInfo")
	bool matching;

	/*한 게임 안에서 저장할 정보들, 게임이 끝나면 초기화됨*/

	//이전 p2p 그룹 번호 및 목록(게임)
	Proud::HostID recentGameNumber;
	std::vector<Proud::HostID> recentGameList;
	//유저 네임
	
	//이전 p2p 그룹 번호 및 목록(팀)
	//게임 내에서 나의 인덱스 홀 짝으로 팀 구분
	int myIdx;

	//내 팀원들의 HostID
	std::vector<Proud::HostID> recentTeamList;
	//팀 네임
	
	//hostId to gameListIdx
	std::unordered_map<int64, int> hostIdToGameListIdx;

	//actor 목록 페어
	//식별자 및 원격 플레이어
	std::unordered_map<int64, RemotePlayer*> player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "gameInfo")
	int kill;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "gameInfo")
	int death;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "gameInfo")
	int point;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "gameInfo")
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "gameInfo")
	float maxHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "gameInfo")
	float maxFuel;

	/*한 게임 정보 끝*/
	
public:
	virtual void Init();
	FTimerHandle TimerHandle;
	void TimerFunction();
};



