// Fill out your copyright notice in the Description page of Project Settings.


#include "UglyZombieAnimInstance.h"
#include "Monster.h"

UUglyZombieAnimInstance::UUglyZombieAnimInstance()
{
	CurrentSpeed = 0.f;

}

void UUglyZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

void UUglyZombieAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UUglyZombieAnimInstance::PlayDamagedMontage()
{
	Montage_Play(DamagedMontage, 1.0f);

	int32 SectionNum = rand() % 2 + 1;
	FName DamagedSectionName = FName(*FString::Printf(TEXT("Damage%d"), SectionNum));
	
	Montage_JumpToSection(DamagedSectionName, DamagedMontage);
}

void UUglyZombieAnimInstance::AnimNotify_AttackEnd()
{
	AttackEnd.Broadcast();
}

void UUglyZombieAnimInstance::AnimNotify_DamagedEnd()
{
	DamagedEnd.Broadcast();
}

void UUglyZombieAnimInstance::AnimNotify_DeadEnd()
{
	DeadEnd.Broadcast();
}

void UUglyZombieAnimInstance::AnimNotify_AttackCheck()
{
	AttackCheck.Broadcast();
}