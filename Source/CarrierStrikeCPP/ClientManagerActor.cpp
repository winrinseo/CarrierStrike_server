// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientManagerActor.h"
#include "CarrierStrikeGameInstance.h"



AClientManagerActor::AClientManagerActor()
{
}

// Called when the game starts or when spawned
void AClientManagerActor::BeginPlay()
{
	Super::BeginPlay();
	
	UCarrierStrikeGameInstance* gameInstance = Cast<UCarrierStrikeGameInstance>(GetGameInstance());
	int id = gameInstance->id;
	int mmr = gameInstance->mmr;
	FString name = gameInstance->userName;
	UE_LOG(LogTemp, Warning, TEXT("My int value is: %s"), *FString::FromInt(id));
	UE_LOG(LogTemp, Warning, TEXT("My name value is: %s"), *name);
	UE_LOG(LogTemp, Warning, TEXT("My int value is: %s"), *FString::FromInt(mmr));

	if (gameInstance == Game::instance) {
		UE_LOG(LogTemp, Warning, TEXT("same address"));

	}

}

// Called every frame
void AClientManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

