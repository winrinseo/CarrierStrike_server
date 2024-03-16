// Fill out your copyright notice in the Description page of Project Settings.


#include "RemoteActor.h"

// Sets default values
ARemoteActor::ARemoteActor()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARemoteActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARemoteActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARemoteActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

