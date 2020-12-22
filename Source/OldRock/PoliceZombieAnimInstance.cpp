// Fill out your copyright notice in the Description page of Project Settings.


#include "PoliceZombieAnimInstance.h"
#include "Monster.h"

UPoliceZombieAnimInstance::UPoliceZombieAnimInstance()
{
	CurrentSpeed = 0.f;
}

void UPoliceZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bDead)
	{
		return;
	}

	auto Pawn = Cast<AMonster>(TryGetPawnOwner());
	if (::IsValid(Pawn))
	{
		CurrentSpeed = Pawn->GetVelocity().Size();

		bDead = (Pawn->GetCurrentState() == EMonsterState::Dead) ? true : false;
	}
}

void UPoliceZombieAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UPoliceZombieAnimInstance::AnimNotify_AttackEnd()
{
	AttackEnd.Broadcast();
}

void UPoliceZombieAnimInstance::AnimNotify_AttackCheck()
{
	AttackCheck.Broadcast();
}