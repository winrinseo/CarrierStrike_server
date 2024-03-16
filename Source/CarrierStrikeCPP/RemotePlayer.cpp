// Fill out your copyright notice in the Description page of Project Settings.


#include "RemotePlayer.h"

RemotePlayer::RemotePlayer()
{
	actor = nullptr;
	prevRotation = FRotator(0, 0, 0);
}

RemotePlayer::~RemotePlayer()
{
	if (actor != nullptr) {
		delete actor;
	}
	for (AActor* d : myDestroyer)
		delete d;
	myDestroyer.clear();
}


void RemotePlayer::DelActor() {
	if (actor != nullptr) {
		delete actor;
		actor = nullptr;
	}
}

AActor* RemotePlayer::GetActor() {
	return actor;
}

void RemotePlayer::Update() {
	if (actor == nullptr)
		return;
	
	positionFollower.FrameMove(actor->GetWorld()->DeltaTimeSeconds);
	pitchFollwer.FrameMove(actor->GetWorld()->DeltaTimeSeconds);
	yawFollwer.FrameMove(actor->GetWorld()->DeltaTimeSeconds);
	rollFollwer.FrameMove(actor->GetWorld()->DeltaTimeSeconds);

	//곡선형으로 좌표를 구함
	Vector3 getPosition = positionFollower.GetSplineFollowerPosition();
	FVector position = FVector(getPosition.x, getPosition.y, getPosition.z);
	//FVector position = positionFollower.GetFollowerPosition();
	
	FRotator rotation = FRotator(
		(float)pitchFollwer.GetFollowerAngle(),
		(float)yawFollwer.GetFollowerAngle(),
		(float)rollFollwer.GetFollowerAngle()
	);
	UE_LOG(LogTemp, Warning, TEXT("predict , pitch : %f , yaw : %f , roll : %f"), (float)pitchFollwer.GetFollowerAngle(),
		(float)yawFollwer.GetFollowerAngle(),
		(float)rollFollwer.GetFollowerAngle());

	actor->SetActorLocation(position);
	actor->SetActorRotation(rotation);
}