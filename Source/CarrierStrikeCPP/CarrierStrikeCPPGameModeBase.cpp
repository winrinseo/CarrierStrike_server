// Copyright Epic Games, Inc. All Rights Reserved.


#include "CarrierStrikeCPPGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include <algorithm>

float coord[MOTHERSHIP_SPAWN_AREA][3] = {
	{-1092040.0, -1025030.0, 20.0 },
	//{1415670.0, -1486080.0, 20.0 },
	{-647805.0, -997143.875, 20.0},
	{-1450390.0, 282540.0, 20.0 },
	{269060.0, 240810.0, 20.0 }
};

float relativeCoord[4][3] = {
	{5230.0 , 1680.0 , 1990.0 },
	{-3820.0 ,-2150.0 ,1990.0},
	//{4450.0 , -470.0 , 1990.0 },
	{-4530.0 , 2050.0 , 1990.0 },
	{-6310.0 , -3410.0 , 1990.0 }
};


float shipRelativeCoord[6][3] = {
   { 6610.0, -19120.0, 0.0 },
   { -10660.0 , -19120.0, 0.0 },
   { 6610.0, 16280.0, 0.0 },
   { -10660.0, 16280.0, 0.0 },
   { 33090.0, 0.0, 0.0 },
   { -25180.0, 0.0, 0.0 },
};

float carrierTurret[4][3] = {
	{-10390.0   ,-4810.0  ,2330.0},
	{-8960.0 ,3700.0, 2280.0},
	{6880.0 ,3830.0 , 2610.0},
	{6920.0 ,-4550.0, 2550.0}
};
float carrierRim[3] = {-3100 ,-3730.0 ,2220.0 };

float destTurret[3] = {-940.0  ,-70.0, 3420.0 };

float destRim[3] = { -1710.0 ,-110.0  ,3360.0 };

float camRelativeCoord[3] = {0,0,160000.0};

AActor* ACarrierStrikeCPPGameModeBase::SpawnActor(FString path, FVector originCoord = FVector(0, 0, 0), FVector rCoord = FVector(0,0,0), FRotator rot = FRotator(0,0,0), std::vector<FName> tags = std::vector<FName>(), FActorSpawnParameters param = FActorSpawnParameters()) {
	FVector c = originCoord + rCoord;
	FTransform transform(rot , c);
	if (UBlueprint* BPClass = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *path))) {
		TSubclassOf<class AActor> blockBP = (UClass*)BPClass->GeneratedClass;
		AActor* spawnedActor = GetWorld()->SpawnActor<AActor>((UClass*)blockBP, c , rot, param);

		//태그 부여
		for (auto & tag : tags) 
			spawnedActor->Tags.AddUnique(tag);
		return spawnedActor;
	}
	return nullptr;
}

void ACarrierStrikeCPPGameModeBase::LoginRequest(FString id, FString password) {
	std::string sId(TCHAR_TO_UTF8(*id));
	std::string sPassword(TCHAR_TO_UTF8(*password));
	UE_LOG(LogTemp, Warning, TEXT("id is : %s"), *id);
	UE_LOG(LogTemp, Warning, TEXT("pass is : %s"), *password);
	lobbyProxy.requestLogin(Proud::HostID_Server, Proud::RmiContext::SecureReliableSend, sId, sPassword);
}

void ACarrierStrikeCPPGameModeBase::MatchRequest() {
	UE_LOG(LogTemp, Warning, TEXT("match"));
	//서버로부터 매칭 큐에 들어갔다는 응답이 온다면 매칭 중 화면띄움
	lobbyProxy.requestMatching(Proud::HostID_Server, Proud::RmiContext::ReliableSend);
	Game::instance->matching = true;
}

void ACarrierStrikeCPPGameModeBase::PartyInvite(FString id) {

}

void ACarrierStrikeCPPGameModeBase::PartyOut() {

}

void ACarrierStrikeCPPGameModeBase::LobbyChatWithEveryPlayer(FString id, FString chat) {

}

void ACarrierStrikeCPPGameModeBase::LobbyChatWithMyParty(FString id, FString chat) {

}

void ACarrierStrikeCPPGameModeBase::StartGame() {
	UCarrierStrikeGameInstance* instance = Game::instance;
	
	instance->player.clear();
	instance->kill = 0;
	instance->death = 0;
	instance->point = 0;

	HostIDArray p2pGroup;
	Game::instance->m_pNetClient->GetGroupMembers(Game::instance->recentGameNumber,p2pGroup);
	for (auto remoteID : p2pGroup) {
		
		Game::instance->recentGameList.push_back(remoteID);
		//remotePlayer를 생성한다.
		//사용하진 않겠지만 나 자신도 여기 등록해둔다.
		if (Game::instance->player[remoteID] == nullptr)
			Game::instance->player[remoteID] = new RemotePlayer;
		int idx = (Game::instance->recentGameList.size() - 1);
		Game::instance->player[remoteID]->team = idx % 2 == 0 ? TEXT("BLUE") : TEXT("RED");
	}
	sort(Game::instance->recentGameList.begin(), Game::instance->recentGameList.end());

	for (auto a : Game::instance->recentGameList) {
		UE_LOG(LogTemp, Warning, TEXT("remoteID : %d"), (int)a);
	}
}

void ACarrierStrikeCPPGameModeBase::EndGame() {
	//최근 진행한 게임의 정보를 제거한다.
	UCarrierStrikeGameInstance* instance = Game::instance;

	instance->recentGameNumber = HostID_None;
	instance->recentGameList.clear();
	for (auto p : instance->player) 
		delete p.second;

	instance->player.clear();

	instance->recentTeamList.clear();
	instance->hostIdToGameListIdx.clear();
}

FString ACarrierStrikeCPPGameModeBase::GetUserNameFromHostID() {
	return "";
}

//HOSTID
void ACarrierStrikeCPPGameModeBase::SpawnFighter(int64 remoteID) {
	if (Game::instance->player[remoteID] == nullptr)
		Game::instance->player[remoteID] = new RemotePlayer;
	UE_LOG(LogTemp, Warning, TEXT("spawn"));

	//모함의 idx 번호를 구해온다.
	int carrierIdx = (Game::instance->hostIdToGameListIdx[remoteID] % 2);
	UE_LOG(LogTemp, Warning, TEXT("carrierIDX : %d"), carrierIdx);
	//스폰될 상대 좌표의 idx를 구한다.
	int fighterCoordIdx = (Game::instance->hostIdToGameListIdx[remoteID] / 2);

	/*FVector mothershipCoord = Game::instance->player[Game::instance->recentGameList[carrierIdx]]->actor->GetActorLocation();
	FRotator mothershipRot = Game::instance->player[Game::instance->recentGameList[carrierIdx]]->actor->GetActorRotation();*/
	//식별자 정의
	FString TagString = FString::Printf(TEXT("%d"), remoteID);
	int t = (Game::instance->hostIdToGameListIdx[remoteID] % 2);
	FName team = ((Game::instance->hostIdToGameListIdx[remoteID] % 2) == 0) ? TEXT("BLUE") : TEXT("RED");
	FName unique = FName(*TagString);
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Game::instance->player[remoteID]->HP = FIGHTER_INIT_HP;
	if ((Proud::HostID)remoteID == Game::instance->m_pNetClient->GetLocalHostID()) {
		//생성해야 할 액터의 remoteID가 나라면 내가 조종할 수 있는 액터를 생성한다.
		AActor* spawnedActor = SpawnActor("Blueprint'/Game/BlueprintClasses/F-18_Blueprint/BP_F-18Jet_Mine.BP_F-18Jet_Mine'", FVector(coord[t][0], coord[t][1], coord[t][2]), FVector(relativeCoord[fighterCoordIdx][0], relativeCoord[fighterCoordIdx][1], relativeCoord[fighterCoordIdx][2]), FRotator(0,0,0), {unique,team,"","","FLIGHT"}, param);
		Game::instance->player[remoteID]->actor = spawnedActor;
		AActor* parent = Game::instance->player[Game::instance->recentGameList[carrierIdx]]->actor;
		//spawnedActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
		UE_LOG(LogTemp, Warning, TEXT("address : %f , %f , %f"), Game::instance->player[Game::instance->recentGameList[carrierIdx]]->actor->GetActorLocation().X , Game::instance->player[Game::instance->recentGameList[carrierIdx]]->actor->GetActorLocation().Y , Game::instance->player[Game::instance->recentGameList[carrierIdx]]->actor->GetActorLocation().Z);
	}
	else{
		//일반적인 원격 액터 생성
		AActor* spawnedActor = SpawnActor("Blueprint'/Game/BlueprintClasses/F-18_Blueprint/BP_F-18Jet_Other.BP_F-18Jet_Other'", FVector(coord[t][0], coord[t][1], coord[t][2]),  FVector(relativeCoord[fighterCoordIdx][0], relativeCoord[fighterCoordIdx][1], relativeCoord[fighterCoordIdx][2]), FRotator(0,0,0), { unique,team ,"SONAR","HOMING","FLIGHT"}, param);
		Game::instance->player[remoteID]->actor = spawnedActor;
		AActor* parent = Game::instance->player[Game::instance->recentGameList[carrierIdx]]->actor;
		//spawnedActor->AttachToActor(parent, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

//HOSTID와 스폰할 위치 인덱스
void ACarrierStrikeCPPGameModeBase::SpawnMotherShip(int64 remoteID, int i,int idx) {
	UE_LOG(LogTemp, Warning, TEXT("spawn"));
	FActorSpawnParameters param;
	param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//식별자 정의
	FString TagString = FString::Printf(TEXT("%d"), remoteID);
	FName team = (idx == 0) ? TEXT("BLUE") : TEXT("RED");
	FName teamship = (idx == 0) ? TEXT("BLUESHIP") : TEXT("REDSHIP");
	FName unique = FName(*TagString);
	UE_LOG(LogTemp, Warning, TEXT("i : %d , idx : %d"),i,idx);

	//if ((Proud::HostID)remoteID == Game::instance->m_pNetClient->GetLocalHostID()) {
	//	//생성해야 할 액터의 remoteID가 나라면 
	//	AActor* spawnedActor = SpawnActor("Blueprint'/Game/BlueprintClasses/Carrier_BluePrint/BP_West_Carrier_CVN76_Mine.BP_West_Carrier_CVN76_Mine'", FVector(coord[i][0], coord[i][1], coord[i][2]), FVector(0,0,0), FRotator(0,0,0), {unique,team,teamship }, param);
	//	Game::instance->player[remoteID]->actor = spawnedActor;
	//	UE_LOG(LogTemp, Warning, TEXT("x : %f , y : %f, z : %f"), coord[i][0], coord[i][1], coord[i][2]);
	//	//카메라 스폰
	//	AActor* camActor = SpawnActor("Blueprint'/Game/BlueprintClasses/Carrier_BluePrint/RTSCamera.RTSCamera'" , FVector(0,0,0) , FVector(camRelativeCoord[0], camRelativeCoord[1], camRelativeCoord[2]), FRotator(0, 0, 0), { unique, team }, param);
	//	camActor->AttachToActor(spawnedActor, FAttachmentTransformRules::KeepRelativeTransform);

	//	//CIWS 스폰
	//	/*for (int j = 0; j < 2; j++) {
	//		AActor* turret = SpawnActor("Blueprint'/Game/BlueprintClasses/CIWS_Turret/BP_Turret_Invisible.BP_Turret_Invisible'", FVector(0, 0, 0), FVector(carrierTurret[j][0], carrierTurret[j][1], carrierTurret[j][2]), FRotator(0, 0, 0), { unique, team }, param);
	//		turret->AttachToActor(spawnedActor, FAttachmentTransformRules::KeepRelativeTransform);
	//	}*/
	//	//rim 스폰
	//	//AActor* rim = SpawnActor("Blueprint'/Game/BlueprintClasses/CIWS_Turret/BP_RIM_invisible.BP_RIM_invisible'", FVector(0, 0, 0), FVector(camRelativeCoord[0], camRelativeCoord[1], camRelativeCoord[2]), FRotator(0, 0, 0), { unique, team }, param);
	//	//rim->AttachToActor(spawnedActor, FAttachmentTransformRules::KeepRelativeTransform);

	//	//Mine 호위 구축함 스폰
	//	for (int j = 1; j < 6; j+=3) {
	//		FString TagString2 = FString::Printf(TEXT("%d"), j);
	//		FName unique2 = FName(*TagString2);
	//		AActor* shipActor = SpawnActor("Blueprint'/Game/BlueprintClasses/Carrier_BluePrint/BP_West_Ship_DDG102_Mine.BP_West_Ship_DDG102_Mine'", FVector(coord[i][0], coord[i][1], coord[i][2]), FVector(shipRelativeCoord[j][0], shipRelativeCoord[j][1], shipRelativeCoord[j][2]), FRotator(0,0,0), { unique, team , unique2 }, param);
	//		shipActor->AttachToActor(spawnedActor, FAttachmentTransformRules::KeepWorldTransform);
	//		Game::instance->player[remoteID]->myDestroyer.push_back(shipActor);

	//		//turret
	//		/*AActor* turret = SpawnActor("Blueprint'/Game/BlueprintClasses/CIWS_Turret/BP_Turret_Invisible.BP_Turret_Invisible'", FVector(0, 0, 0), FVector(destTurret[0], destTurret[1], destTurret[2]), FRotator(0, 0, 0), { unique, team }, param);
	//		turret->AttachToActor(shipActor, FAttachmentTransformRules::KeepRelativeTransform);*/

	//		//AActor* dRim = SpawnActor("Blueprint'/Game/BlueprintClasses/CIWS_Turret/BP_RIM_invisible.BP_RIM_invisible'", FVector(0, 0, 0), FVector(camRelativeCoord[0], camRelativeCoord[1], camRelativeCoord[2]), FRotator(0, 0, 0), { unique, team }, param);
	//		//dRim->AttachToActor(shipActor, FAttachmentTransformRules::KeepRelativeTransform);
	//	}
	//}
	if(1) {
		//일반적인 원격 액터 생성
		AActor* spawnedActor = SpawnActor("Blueprint'/Game/BlueprintClasses/Carrier_BluePrint/BP_West_Carrier_CVN76_Other.BP_West_Carrier_CVN76_Other'", FVector(coord[i][0], coord[i][1], coord[i][2]), FVector(0,0,0), FRotator(0,0,0), { unique,team,teamship }, param);
		Game::instance->player[remoteID]->actor = spawnedActor;


		//CIWS 스폰
		/*for (int j = 0; j < 2; j++) {
			AActor* turret = SpawnActor("Blueprint'/Game/BlueprintClasses/CIWS_Turret/BP_Turret_Invisible.BP_Turret_Invisible'", FVector(0, 0, 0), FVector(carrierTurret[j][0], carrierTurret[j][1], carrierTurret[j][2]), FRotator(0, 0, 0), { unique, team }, param);
			turret->AttachToActor(spawnedActor, FAttachmentTransformRules::KeepRelativeTransform);
		}*/
		//rim 스폰
		//AActor* rim = SpawnActor("Blueprint'/Game/BlueprintClasses/CIWS_Turret/BP_RIM_invisible.BP_RIM_invisible'", FVector(0, 0, 0), FVector(camRelativeCoord[0], camRelativeCoord[1], camRelativeCoord[2]), FRotator(0, 0, 0), { unique, team }, param);
		//rim->AttachToActor(spawnedActor, FAttachmentTransformRules::KeepRelativeTransform);

		//Other 호위 구축함 스폰
		//for (int j = 1; j < 6; j+=3) {
		//	FString TagString2 = FString::Printf(TEXT("%d"), j);
		//	FName unique2 = FName(*TagString2);
		//	AActor* shipActor = SpawnActor("Blueprint'/Game/BlueprintClasses/Carrier_BluePrint/BP_West_Ship_DDG102_Other.BP_West_Ship_DDG102_Other'", FVector(coord[i][0], coord[i][1], coord[i][2]), FVector(shipRelativeCoord[j][0], shipRelativeCoord[j][1], shipRelativeCoord[j][2]), FRotator(), { unique, team , unique2 }, param);
		//	shipActor->AttachToActor(spawnedActor, FAttachmentTransformRules::KeepWorldTransform);
		//	Game::instance->player[remoteID]->myDestroyer.push_back(shipActor);


		//	//turret
		//	/*AActor* turret = SpawnActor("Blueprint'/Game/BlueprintClasses/CIWS_Turret/BP_Turret_Invisible.BP_Turret_Invisible'", FVector(0, 0, 0), FVector(destTurret[0], destTurret[1], destTurret[2]), FRotator(0, 0, 0), { unique, team }, param);
		//	turret->AttachToActor(shipActor, FAttachmentTransformRules::KeepRelativeTransform);*/
		//	//rim
		//	//AActor* dRim = SpawnActor("Blueprint'/Game/BlueprintClasses/CIWS_Turret/BP_RIM_invisible.BP_RIM_invisible'", FVector(0, 0, 0), FVector(camRelativeCoord[0], camRelativeCoord[1], camRelativeCoord[2]), FRotator(0, 0, 0), { unique, team }, param);
		//	//dRim->AttachToActor(shipActor, FAttachmentTransformRules::KeepRelativeTransform);
		//}
	}
}

void ACarrierStrikeCPPGameModeBase::SpawnFighterAll() {
	UE_LOG(LogTemp, Warning, TEXT("spawn all"));

	std::vector<Proud::HostID>& gameList = Game::instance->recentGameList;

	for (int i = 0; i < gameList.size(); i++) {
		if (gameList[i] == Game::instance->m_pNetClient->GetLocalHostID()) {
			//일단 팀구분을 위해 인덱스를 미리 구한다.
			Game::instance->myIdx = i;
			if (i > 1) Game::instance->maxHP = FIGHTER_INIT_HP;
			else Game::instance->maxHP = MOTHERSHIP_INIT_HP;
		}
		//hostID와 gameList idx를 매핑한다.
		Game::instance->hostIdToGameListIdx[gameList[i]] = i;
	}

	//나의 팀 리스트 설정
	for (int i = 0; i < gameList.size(); i++) {
		if ((gameList[i] % 2) == (Game::instance->myIdx % 2)) {
			Game::instance->recentTeamList.push_back(gameList[i]);
		}
		Game::instance->player[gameList[i]]->team = ((i % 2) == 0) ? TEXT("BLUE") : TEXT("RED");
	}
	/*
		모함의 위치를 랜덤하게 정하기 위한 비트마스크,
		MOTHERSHIP_SPAWN_AREA 개의 스폰 위치 중에서 하나씩 랜덤하게 뽑는다.
		이미 뽑은 숫자는 스킵한다.
	*/
	int area = 1 << (MOTHERSHIP_SPAWN_AREA + 1);

	for (int i = 0; i < 2; i++) {
		SpawnMotherShip((int64)gameList[i], i, i);
	}

	for (int i = 0; i < gameList.size(); i++) {

		//if ((i <= 1)) {
		//	int random = MOTHERSHIP_SPAWN_AREA + 1;

		//	//새로운 숫자를 뽑을 때 까지 난수를 생성한다.
		//	while (area & (1 << random)) {
		//		srand((uint64_t)time(NULL));
		//		random = rand() % MOTHERSHIP_SPAWN_AREA;
		//	}
		//	area |= (1 << random);
		//	SpawnMotherShip((int64)gameList[i], i , i);
		//}
		if(1) {
			//전투기는 각 팀 항공 모함의 상대 좌표에 스폰된다.
			//hostID를 사용해 gameList의 위치 인덱스를 얻어온다.
			SpawnFighter((int64)gameList[i]);
		}
	}
}

float ACarrierStrikeCPPGameModeBase::GetRemoteHP(int64 remoteID) {
	return Game::instance->player[remoteID]->HP;
}

void ACarrierStrikeCPPGameModeBase::Move(const FVector& c, const FVector& v,  const FRotator& r, const float& speed) {
	//if (m_lastSendTime < 0 || Game::instance->GetWorld()->TimeSeconds - m_lastSendTime > 0.3) {
		//Game p2p 그룹에게 내 위치를 전송한다.
		FVector vv = v * speed * 0.3f;
		p2pProxy.move(Game::instance->recentGameNumber, Proud::RmiContext::UnreliableSend, c.X, c.Y, c.Z, vv.X, vv.Y, vv.Z, r.Pitch, r.Yaw, r.Roll);
		
	//}
}


void ACarrierStrikeCPPGameModeBase::Update(int64 remoteID) {
	if ((HostID)remoteID == Game::instance->m_pNetClient->GetLocalHostID())
		return;

	RemotePlayer* remotePlayer = Game::instance->player[remoteID];

	remotePlayer->Update();
}

void ACarrierStrikeCPPGameModeBase::Attack() {
	p2pProxy.attack(Game::instance->recentGameNumber , RmiContext::ReliableSend);
}

void ACarrierStrikeCPPGameModeBase::AttackStop() {
	p2pProxy.attackStop(Game::instance->recentGameNumber, RmiContext::ReliableSend);
}

void ACarrierStrikeCPPGameModeBase::AttackMissile(int weapon, int targetID) {
	p2pProxy.attackMissile(Game::instance->recentGameNumber, RmiContext::ReliableSend, weapon ,targetID);
}

void ACarrierStrikeCPPGameModeBase::Flare() {
	p2pProxy.flare(Game::instance->recentGameNumber, RmiContext::ReliableSend);
}

void ACarrierStrikeCPPGameModeBase::GetDamage(float damage) {
	
	if ((Game::instance->HP) -= damage <= 0) {
		//액터 삭제, 파괴되었다는 통보를 함으로써 원격에 있는 액터들도 메모리를 해제할 수 있도록 한다.
		p2pProxy.destroyActor(Game::instance->recentGameNumber, RmiContext::ReliableSend);
	}
	else {
		//남은 HP를 보내준다.
		p2pProxy.getDamageActor(Game::instance->recentGameNumber, RmiContext::ReliableSend , Game::instance->HP);
	}

}

void ACarrierStrikeCPPGameModeBase::Destroy() {
	//액터 삭제, 파괴되었다는 통보를 함으로써 원격에 있는 액터들도 메모리를 해제할 수 있도록 한다.
	p2pProxy.destroyActor(Game::instance->recentGameNumber, RmiContext::ReliableSend);
}


void ACarrierStrikeCPPGameModeBase::Respawn() {
	p2pProxy.respawnActor(Game::instance->recentGameNumber, RmiContext::ReliableSend);
}


void ACarrierStrikeCPPGameModeBase::DestroyerMove(int idx,const FVector& c) {
	p2pProxy.destroyerMove(Game::instance->recentGameNumber, RmiContext::ReliableSend,idx , c.X , c.Y);
}

void ACarrierStrikeCPPGameModeBase::Ping(int code , const FVector& c) {
	//아군에게만 전송
	p2pProxy.ping(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), RmiContext::ReliableSend, code, c.X , c.Y , c.Z);
}

void ACarrierStrikeCPPGameModeBase::Store(int code) {
	//해당 유저가 무엇을 구매했는지에 따라 다른 RMI를 호출함 / 짝수면 블루팀 , 홀수면 레드팀
	int teamCode = (Game::instance->myIdx % 2);

	switch (code) {
	case 0:
		//p2pProxy.ping(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), code, c.X, c.Y, c.Z);
		break;
	case 1:
		//p2pProxy.ping(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), code, c.X, c.Y, c.Z);
		break;
	case 2:
		//p2pProxy.ping(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), code, c.X, c.Y, c.Z);
		break;
	case 3:
		//p2pProxy.ping(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), code, c.X, c.Y, c.Z);
		break;
	case 4:
		//p2pProxy.ping(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), code, c.X, c.Y, c.Z);
		break;
	case 5:
		//p2pProxy.ping(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), code, c.X, c.Y, c.Z);
		break;
	case 6:
		//p2pProxy.ping(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), code, c.X, c.Y, c.Z);
		break;
	}
}

void ACarrierStrikeCPPGameModeBase::MinusPoint(int point) {
	Game::instance->point -= point;
}

//채팅
void ACarrierStrikeCPPGameModeBase::AllChat(FString userName , FString chat) {
	std::string name(TCHAR_TO_UTF8(*userName));
	std::string chatting(TCHAR_TO_UTF8(*chat));
	p2pProxy.allChat(Game::instance->recentGameNumber, RmiContext::ReliableSend, name, chatting);
}

void ACarrierStrikeCPPGameModeBase::TeamChat(FString userName, FString chat) {
	std::string name(TCHAR_TO_UTF8(*userName));
	std::string chatting(TCHAR_TO_UTF8(*chat));
	p2pProxy.teamChat(&Game::instance->recentTeamList[0], Game::instance->recentTeamList.size(), RmiContext::ReliableSend, name, chatting);
}

void ACarrierStrikeCPPGameModeBase::Kill(int userThatKilledMe){
	p2pProxy.kill((HostID)userThatKilledMe, RmiContext::ReliableSend);
}


void ACarrierStrikeCPPGameModeBase::EndGame(FString defeatedTeam) {
	std::string dteam(TCHAR_TO_UTF8(*defeatedTeam));
	p2pProxy.endGame(Game::instance->recentGameNumber, RmiContext::ReliableSend , dteam);
}

//modebase 객체가 필요한 RMI는 여기서 정의

DEFRMI_LobbyS2C_completeMatching(LobbyS2Cstub) {
	UE_LOG(LogTemp, Warning, TEXT("matching complete"));
	Game::instance->matching = false;
	ACarrierStrikeCPPGameModeBase* modebase = dynamic_cast<ACarrierStrikeCPPGameModeBase*>(Game::instance->GetWorld()->GetAuthGameMode());
	//게임 레벨로 이동
	modebase->StartGame();
	UGameplayStatics::OpenLevel(Game::instance->GetWorld(), "SampleMap");
	return true;
}

DEFRMI_P2P_respawnActor(GameC2Cstub) {
	UE_LOG(LogTemp, Warning, TEXT("respawn %d"), remote);
	//메모리 해제
	Game::instance->player[remote] = nullptr;

	ACarrierStrikeCPPGameModeBase* modebase = dynamic_cast<ACarrierStrikeCPPGameModeBase*>(Game::instance->GetWorld()->GetAuthGameMode());
	modebase->SpawnFighter(remote);
	return true;
}

DEFRMI_P2P_endGame(GameC2Cstub) {
	ACarrierStrikeCPPGameModeBase* modebase = dynamic_cast<ACarrierStrikeCPPGameModeBase*>(Game::instance->GetWorld()->GetAuthGameMode());
	//종료를 위한 객체를 생성한다.
	ARemoteActor* player = static_cast<ARemoteActor*>(modebase->SpawnActor(TEXT("Blueprint'/Game/BlueprintClasses/EndActor.EndActor'")));
	std::string myTeam = (Game::instance->myIdx % 2 == 0) ? "BLUE" : "RED";
	if (defeatedTeam == myTeam)
		player->Defeat();
	else
		player->Win();
		return true;
}
