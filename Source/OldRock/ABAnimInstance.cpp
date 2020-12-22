// Fill out your copyright notice in the Description page of Project Settings.
#include "ABAnimInstance.h"
#include "PlayerCharacter.h"


UABAnimInstance::UABAnimInstance()
{
	CurrentSpeed = 0.f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage>AttackMontage1(TEXT("/Game/Annabel/Animations/Fire.Fire"));
	if (AttackMontage1.Succeeded()) {
		AttackMontage = AttackMontage1.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	Character = Cast<APlayerCharacter>(Pawn);
	if (IsValid(Pawn))
	{
		CurrentSpeed = Pawn->GetVelocity().Size();
		IsDead = Character->IsDead;
	}
}
bool UABAnimInstance::PlayAttackMontage()
{
	if (!Montage_IsPlaying(AttackMontage)) {
		Montage_Play(AttackMontage, 1.0f);
		return true;
	}
	return false;
	
}
bool UABAnimInstance::PlayHitMontage()
{
	Montage_Play(AttackMontage, 1.0f);
	return true;
	
}
void UABAnimInstance::JumpToHitMontageSection()
{
	Montage_JumpToSection(FName("Hit"), AttackMontage);
}
void UABAnimInstance::JumpToEvasionMontageSection()
{
	Montage_JumpToSection(FName("Evasion"), AttackMontage);
}
void UABAnimInstance::JumpToTurnMontageSection()
{
	Montage_JumpToSection(FName("Turn"), AttackMontage);
}
void UABAnimInstance::JumpToPickMontageSection()
{
	Montage_JumpToSection(FName("Pick"), AttackMontage);
}
void UABAnimInstance::JumpToPick2MontageSection()
{
	Montage_JumpToSection(FName("Pick2"), AttackMontage);
}
void UABAnimInstance::AnimNotify_AttackFire()
{
	OnAttackFire.Broadcast();
}
void UABAnimInstance::AnimNotify_SlideEnd()
{
	OnSlideEnd.Broadcast();
}