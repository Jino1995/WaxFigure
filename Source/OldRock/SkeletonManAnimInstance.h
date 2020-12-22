// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "SkeletonManAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDamagedEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDeadEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackCheckDelegate);

/**
 * 
 */
UCLASS()
class OLDROCK_API USkeletonManAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	USkeletonManAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void PlayAttackMontage();

	void PlayDamagedMontage();

	void PlayDeathMontage();

public:
	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_DamagedEnd();

	UFUNCTION()
	void AnimNotify_DeadEnd();

	UFUNCTION()
	void AnimNotify_AttackCheck();

public:
	FOnAttackEndDelegate AttackEnd;
	FOnDamagedEndDelegate DamagedEnd;
	FOnDeadEndDelegate DeadEnd;
	FOnAttackCheckDelegate AttackCheck;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool bDead;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DamagedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DeathMontage;
};
