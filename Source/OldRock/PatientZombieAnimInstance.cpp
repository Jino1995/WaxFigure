// Fill out your copyright notice in the Description page of Project Settings.


#include "PatientZombieAnimInstance.h"
#include "Monster.h"
#include "PatientZombie.h"

UPatientZombieAnimInstance::UPatientZombieAnimInstance()
{
	CurrentSpeed = 0.f;

}

void UPatientZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bDead)
	{
		return;
	}

	auto Pawn = Cast<APatientZombie>(TryGetPawnOwner());
	if (::IsValid(Pawn))
	{
		CurrentSpeed = Pawn->GetVelocity().Size();

		bDead = (Pawn->GetCurrentState() == EMonsterState::Dead) ? true : false;
		bCombat = Pawn->IsCombat();
	}
}

void UPatientZombieAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);
}

void UPatientZombieAnimInstance::PlayDamagedMontage()
{
	Montage_Play(DamagedMontage, 1.0f);

	int32 SectionNum = rand() % 2 + 1;
	FName DamagedSectionName = FName(*FString::Printf(TEXT("Damage%d"), SectionNum));

	Montage_JumpToSection(DamagedSectionName, DamagedMontage);
}

void UPatientZombieAnimInstance::AnimNotify_AttackEnd()
{
	AttackEnd.Broadcast();
}

void UPatientZombieAnimInstance::AnimNotify_DamagedEnd()
{
	DamagedEnd.Broadcast();
}

void UPatientZombieAnimInstance::AnimNotify_DeadEnd()
{
	DeadEnd.Broadcast();
}

void UPatientZombieAnimInstance::AnimNotify_AttackCheck()
{
	AttackCheck.Broadcast();
}