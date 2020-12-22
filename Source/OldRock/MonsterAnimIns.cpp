// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAnimIns.h"

UMonsterAnimIns::UMonsterAnimIns()
{
	CurrentSpeed = 0.f;
}

void UMonsterAnimIns::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		CurrentSpeed = Pawn->GetVelocity().Size();
	}
}