// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacter.h"
#include "ABAnimInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnFireDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSlideDelegate);
UCLASS()
class OLDROCK_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UABAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	bool PlayAttackMontage();

	bool PlayHitMontage();

	void JumpToHitMontageSection();

	void JumpToEvasionMontageSection();

	void JumpToTurnMontageSection();

	void JumpToPickMontageSection();
	void JumpToPick2MontageSection();
	UFUNCTION()
	void AnimNotify_AttackFire();
	UFUNCTION()
	void AnimNotify_SlideEnd();

	FOnFireDelegate OnAttackFire;
	FOnFireDelegate OnSlideEnd;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=State, Meta = (AllowPrivateAccess = true))
	float CurrentSpeed;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
		bool IsDead;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
		class APlayerCharacter* Character;
};
