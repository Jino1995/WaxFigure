// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonManAnimInstance.h"
#include "Monster.h"

USkeletonManAnimInstance::USkeletonManAnimInstance()
{
	CurrentSpeed = 0.f;

}

void USkeletonManAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

void USkeletonManAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.0f);

	int32 SectionNum = rand() % 2 + 1;
	FName AttackSectionName = FName(*FString::Printf(TEXT("Attack%d"), SectionNum));

	Montage_JumpToSection(AttackSectionName, AttackMontage);
}

void USkeletonManAnimInstance::PlayDamagedMontage()
{
	Montage_Play(DamagedMontage, 1.0f);

	int32 SectionNum = rand() % 2 + 1;
	FName DamagedSectionName = FName(*FString::Printf(TEXT("Damage%d"), SectionNum));

	Montage_JumpToSection(DamagedSectionName, DamagedMontage);
}

void USkeletonManAnimInstance::PlayDeathMontage()
{
	Montage_Play(DeathMontage, 1.0f);

	int32 SectionNum = rand() % 4 + 1;
	FName DeathSectionName = FName(*FString::Printf(TEXT("Death%d"), SectionNum));

	Montage_JumpToSection(DeathSectionName, DeathMontage);
}

void USkeletonManAnimInstance::AnimNotify_AttackEnd()
{
	AttackEnd.Broadcast();
}

void USkeletonManAnimInstance::AnimNotify_DamagedEnd()
{
	DamagedEnd.Broadcast();
}

void USkeletonManAnimInstance::AnimNotify_DeadEnd()
{
	DeadEnd.Broadcast();
}

void USkeletonManAnimInstance::AnimNotify_AttackCheck()
{
	AttackCheck.Broadcast();
}
