// Fill out your copyright notice in the Description page of Project Settings.


#include "CarrierStrikeGameInstance.h"

#define WIN32_LEAN_AND_MEAN
#include "../../Common/C2S_common.cpp"
#include "../../Common/C2S_proxy.cpp"

#include "../../Common/S2C_common.cpp"
#include "../../Common/S2C_stub.cpp"

#include "../../Common/C2C_common.cpp"
#include "../../Common/C2C_proxy.cpp"
#include "../../Common/C2C_stub.cpp"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

//로비 RMI 정의
LobbyC2S::Proxy lobbyProxy;
LobbyS2Cstub lobbyStub;

//게임 RMI 정의
P2P::Proxy p2pProxy;
GameC2Cstub p2pStub;


void UCarrierStrikeGameInstance::Init() {
	Super::Init();
	m_pNetClient = Proud::CNetClient::Create();
	UE_LOG(LogTemp,Warning,TEXT("game instance start"));

	// 서버로 커넥션이 맺어졌는지 여부를 저장하고 있습니다.
	bool isConnected = false;

	// 서버로의 커넥션 시도가 완료되면 호출되어집니다.
	m_pNetClient->OnJoinServerComplete =
		[&](Proud::ErrorInfo* info, const Proud::ByteArray& replyFromServer)
	{
		if (info->m_errorType == Proud::ErrorType::Ok)
		{
			// 서버로의 커넥션에 성공하였습니다.
			printf("Succeed to connect server. Allocated hostID=%d\n", m_pNetClient->GetLocalHostID());
			isConnected = true;
		}
		else
		{
			// 서버로의 커넥션에 실패하였습니다.
			std::cout << "Failed to connect to server.\n";
		}
	};
	// 서버와의 커넥션이 끊겼으면 호출되어집니다.
	m_pNetClient->OnLeaveServer = [&](Proud::ErrorInfo* errorInfo) {

		// 메인 루프가 break되도록 합니다.
		isConnected = false;
	};

	// p2p 그룹으로 묶였을 때 호출됩니다.
	m_pNetClient->OnP2PMemberJoin = [this](Proud::HostID memberHostID, Proud::HostID groupHostID, int count, const Proud::ByteArray& arr) {
		recentGameNumber = groupHostID;
	};
	
	//p2p 그룹에서 유저가 떠났을 때 호출됩니다.


	// 클라이언트에 생성한 프록시 객체와 스텁 객체를 부착시킵니다.
	m_pNetClient->AttachProxy(&lobbyProxy);
	m_pNetClient->AttachProxy(&p2pProxy);

	m_pNetClient->AttachStub(&lobbyStub);
	m_pNetClient->AttachStub(&p2pStub);

	Proud::CNetConnectionParam cp;
	cp.m_serverIP = _PNT("192.168.30.116");
	cp.m_serverPort = 4444;

	m_pNetClient->Connect(cp);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCarrierStrikeGameInstance::TimerFunction, 0.01f, true);

	//다른 액터에서도 접근이 쉽도록 instance 저장
	Game::instance = Cast<UCarrierStrikeGameInstance>(GetWorld()->GetGameInstance());
}

void UCarrierStrikeGameInstance::TimerFunction() {
	m_pNetClient->FrameMove();
}

DEFRMI_LobbyS2C_successedLogin(LobbyS2Cstub) {
	UE_LOG(LogTemp,Warning,TEXT("Login success"));
	if (Game::instance != nullptr) {
		//정보 저장
		Game::instance->id = id;
		Game::instance->userName = FString(userName.c_str());
		Game::instance->mmr = mmr;
		Game::instance->matching = false;

		//레벨 이동
		//UGameplayStatics::OpenLevel(Game::instance->GetWorld(),"lobby");


		//test code
		lobbyProxy.requestMatching(Proud::HostID_Server, Proud::RmiContext::ReliableSend);
	}
	return true;
}

DEFRMI_LobbyS2C_failedLogin(LobbyS2Cstub) {
	UE_LOG(LogTemp, Warning, TEXT("Login fail"));
	//UGameplayStatics::OpenLevel(Game::instance->GetWorld(), "SampleMap");
	return true;
}



DEFRMI_LobbyS2C_joinGame(LobbyS2Cstub) {
	//UE_LOG(LogTemp, Warning, TEXT("Join Game"));

	if (Game::instance->player[remoteID] == nullptr)
		Game::instance->player[remoteID] = new RemotePlayer;
	Game::instance->player[remoteID]->playerName;
	return true;
}

DEFRMI_P2P_move(GameC2Cstub) {
	//pitch : 상하각 , yaw : 좌우각 , poll : 회전각
	//UE_LOG(LogTemp, Warning, TEXT("remote : %d / pitch : %f , yaw : %f , roll : %f"),(int)remote,rx,ry,rz);
	double t = Game::instance->m_pNetClient->GetRecentUnreliablePingSec(remote);
	//UE_LOG(LogTemp, Warning, TEXT("remote %d ping is %f"), (int)remote, ping);
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	if (Game::instance->player[(int64)remote] == nullptr)
		return true;
	
	RemotePlayer* remotePlayer = Game::instance->player[(int64)remote];
	if (remotePlayer->actor == nullptr)
		return true;
	remotePlayer->actor->SetActorLocation(FVector(x, y, z));
	remotePlayer->actor->SetActorRotation(FRotator(rx, ry, rz));

	return true;

	//추측항법의 흔적
		////현재 위치 (position follower의 시작지점)
		//FVector here = remotePlayer->actor->GetActorLocation();
		//remotePlayer->positionFollower.SetFollower(Vector3(here.X,here.Y,here.Z), Vector3(vx, vy, vz));
		////예측된 좌표 P = (t * v) + P0
		//Vector3 P = (t * Vector3(vx, vy, vz)) + Vector3(x, y, z);
		//remotePlayer->positionFollower.SetTarget(P , Vector3(vx,vy,vz));

		////각속도 계산
		//FRotator rotationDelta = FRotator(rx, ry, rz) - remotePlayer->prevRotation;
		//float pitchAngularVelocity = rotationDelta.Pitch / remotePlayer->GetActor()->GetWorld()->DeltaTimeSeconds;
		//float yawAngularVelocity = rotationDelta.Yaw / remotePlayer->GetActor()->GetWorld()->DeltaTimeSeconds;
		//float rollAngularVelocity = rotationDelta.Roll / remotePlayer->GetActor()->GetWorld()->DeltaTimeSeconds;

		//
		////현재 각도 (angle follower의 시작지점)
		//FRotator presentRot = remotePlayer->actor->GetActorRotation();

		//remotePlayer->pitchFollwer.SetFollowerAngle(presentRot.Pitch);
		//remotePlayer->pitchFollwer.SetTargetAngle(rx);
		//remotePlayer->pitchFollwer.SetFollowerAngleVelocity(pitchAngularVelocity);

		//remotePlayer->yawFollwer.SetFollowerAngle(presentRot.Yaw);
		//remotePlayer->yawFollwer.SetTargetAngle(ry);
		//remotePlayer->yawFollwer.SetFollowerAngleVelocity(yawAngularVelocity);

		//remotePlayer->rollFollwer.SetFollowerAngle(presentRot.Roll);
		//remotePlayer->rollFollwer.SetTargetAngle(rz);
		//remotePlayer->rollFollwer.SetFollowerAngleVelocity(rollAngularVelocity);

		//remotePlayer->prevRotation = FRotator(rx, ry, rz);
		//return true;
}


DEFRMI_P2P_attack(GameC2Cstub) {
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	ARemoteActor* remotePlayer = dynamic_cast<ARemoteActor*>(Game::instance->player[(int64)remote]->actor);
	
	remotePlayer->RemoteAttack();

	return true;
}

DEFRMI_P2P_attackStop(GameC2Cstub) {
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	ARemoteActor* remotePlayer = dynamic_cast<ARemoteActor*>(Game::instance->player[(int64)remote]->actor);

	remotePlayer->RemoteAttackStop();

	return true;
}

DEFRMI_P2P_attackMissile(GameC2Cstub) {
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	ARemoteActor* remotePlayer = dynamic_cast<ARemoteActor*>(Game::instance->player[(int64)remote]->actor);

	remotePlayer->RemoteAttackMissile(weapon,targetID);

	return true;
}

DEFRMI_P2P_flare(GameC2Cstub) {
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	ARemoteActor* remotePlayer = static_cast<ARemoteActor*>(Game::instance->player[(int64)remote]->actor);

	remotePlayer->Flare();

	return true;
}

DEFRMI_P2P_getDamageActor(GameC2Cstub) {
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	ARemoteActor* remotePlayer = static_cast<ARemoteActor*>(Game::instance->player[(int64)remote]->actor);

	//remotePlayer->Flare();

	return true;
}


DEFRMI_P2P_destroyActor(GameC2Cstub) {
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	UE_LOG(LogTemp, Warning, TEXT("destroy Actor : %d"),remote);
	ARemoteActor* remotePlayer = static_cast<ARemoteActor*>(Game::instance->player[(int64)remote]->actor);
	//파괴하고
	remotePlayer->Destroy();

	//값을 null로
	Game::instance->player[remote] = nullptr;
	return true;
}


DEFRMI_P2P_destroyerMove(GameC2Cstub) {
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	ARemoteActor* remotePlayer = static_cast<ARemoteActor*>(Game::instance->player[(int64)remote]->myDestroyer[idx]);

	remotePlayer->RemoteDestroyerMove( FVector(x,y,20.0));
	return true;
}

DEFRMI_P2P_ping(GameC2Cstub) {
	if (remote == Game::instance->m_pNetClient->GetLocalHostID())
		return true;
	ARemoteActor* remotePlayer = static_cast<ARemoteActor*>(Game::instance->player[(int64)remote]->actor);

	remotePlayer->Ping(code , FVector(x,y,z));
	return true;
}

//상점 기능
DEFRMI_P2P_fuelUpgrade(GameC2Cstub) {
	Game::instance->maxFuel += 50;
	return true;
}

DEFRMI_P2P_hpUpgrade(GameC2Cstub) {
	//다음 리스폰부터 적용
	Game::instance->maxHP += 50;
	return true;
}

DEFRMI_P2P_respawnTime(GameC2Cstub) {
	return true;
}

DEFRMI_P2P_hyperDetect(GameC2Cstub) {
	return true;
}

DEFRMI_P2P_hpFix(GameC2Cstub) {
	return true;
}

DEFRMI_P2P_missileTime(GameC2Cstub) {
	return true;
}

DEFRMI_P2P_destroyerPlus(GameC2Cstub) {
	return true;
}

//채팅

DEFRMI_P2P_allChat(GameC2Cstub) {
	return true;
}

DEFRMI_P2P_teamChat(GameC2Cstub) {
	return true;
}

//킬
DEFRMI_P2P_kill(GameC2Cstub) {
	++Game::instance->kill;
	
	//항공모함에게 포인트를 보낸다.
	HostID carrier = (HostID)Game::instance->recentTeamList[0];
	int point = 100;
	p2pProxy.getPoint(carrier, RmiContext::ReliableSend,point);
	return true;
}

DEFRMI_P2P_getPoint(GameC2Cstub) {
	Game::instance->point += point;
	return true;
}



