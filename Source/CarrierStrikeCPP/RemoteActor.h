// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RemoteActor.generated.h"

UCLASS()
class CARRIERSTRIKECPP_API ARemoteActor : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARemoteActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//원격 액터의 공격
	UFUNCTION(BlueprintImplementableEvent ,	category="remote")
	void RemoteAttack();

	//원격 액터의 공격 중지
	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void RemoteAttackStop();

	//원격 액터의 미사일 공격
	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void RemoteAttackMissile(int weapon, int target);

	//원격 액터의 플레어
	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void Flare();

	//원격 액터의 피격
	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void GetDamage(float remainHp);

	//원격 액터의 파괴
	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void Destroy();

	//구축함 이동
	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void RemoteDestroyerMove(FVector c);

	//핑
	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void Ping(int code , FVector c);


	//핑
	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void Win();

	UFUNCTION(BlueprintImplementableEvent, category = "remote")
	void Defeat();
};
