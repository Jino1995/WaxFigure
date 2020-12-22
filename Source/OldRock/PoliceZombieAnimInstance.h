// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "PoliceZombieAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackCheckDelegate);

/**
 * 
 */
UCLASS()
class OLDROCK_API UPoliceZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPoliceZombieAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void PlayAttackMontage();

public:
	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_AttackCheck();

public:
	FOnAttackEndDelegate AttackEnd;
	FOnAttackCheckDelegate AttackCheck;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool bDead;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;
};
