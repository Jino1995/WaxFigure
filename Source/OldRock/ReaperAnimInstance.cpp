// Fill out your copyright notice in the Description page of Project Settings.


#include "ReaperAnimInstance.h"
#include "Monster.h"

UReaperAnimInstance::UReaperAnimInstance()
{
	CurrentSpeed = 0.f;

}

void UReaperAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

int32 UReaperAnimInstance::PlayAttackMontage()
{
	Montage_Play(AttackMontage, 1.f);

	int32 SectionNum = rand() % 3 + 1;
	FName AttackSectionName = FName(*FString::Printf(TEXT("Attack%d"), SectionNum));

	Montage_JumpToSection(AttackSectionName, AttackMontage);

	return SectionNum;
}

void UReaperAnimInstance::PlaySickleSkill1Montage()
{
	Montage_Play(SickleSkill1Montage, 1.f);
}

void UReaperAnimInstance::PlayChangeToPaseMontage()
{
	Montage_Play(ChangeToPaseMontage, 1.f);
}

void UReaperAnimInstance::PlaySummonMontage()
{
	Montage_Play(SummonMontage, 1.f);
}

void UReaperAnimInstance::PlayDarkBallMontage()
{
	Montage_Play(DarkBallMontage, 1.f);
}

void UReaperAnimInstance::PlayRuneMontage()
{
	Montage_Play(RuneMontage, 1.f);
}

void UReaperAnimInstance::AnimNotify_AttackStart()
{
	AttackStart.Broadcast();
}

void UReaperAnimInstance::AnimNotify_AttackEnd()
{
	AttackEnd.Broadcast();
}

void UReaperAnimInstance::AnimNotify_AttackCheck()
{
	AttackCheck.Broadcast();
}

void UReaperAnimInstance::AnimNotify_BookOpenStart()
{
	BookOpenStart.Broadcast();
}

void UReaperAnimInstance::AnimNotify_EndPaseChanged()
{
	EndPaseChanged.Broadcast();
}

void UReaperAnimInstance::AnimNotify_StartSummon()
{
	StartSummon.Broadcast();
}

void UReaperAnimInstance::AnimNotify_EndSummon()
{
	EndSummon.Broadcast();
}

void UReaperAnimInstance::AnimNotify_ShotDarkBall()
{
	ShotDarkBall.Broadcast();
}

void UReaperAnimInstance::AnimNotify_EndDarkBall()
{
	EndDarkBall.Broadcast();
}

void UReaperAnimInstance::AnimNotify_StartRune()
{
	StartRune.Broadcast();
}

void UReaperAnimInstance::AnimNotify_EndRune()
{
	EndRune.Broadcast();
}