// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ReaperAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackStartDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnBookOpenStartDelegate);
DECLARE_MULTICAST_DELEGATE(FOnEndPaseChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnStartSummonDelegate);
DECLARE_MULTICAST_DELEGATE(FOnEndSummonDelegate);
DECLARE_MULTICAST_DELEGATE(FOnShotDarkBallDelegate);
DECLARE_MULTICAST_DELEGATE(FOnEndDarkBallDelegate);
DECLARE_MULTICAST_DELEGATE(FOnStartRuneDelegate);
DECLARE_MULTICAST_DELEGATE(FOnEndRuneDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSkillStartDelegate);

/**
 * 
 */
UCLASS()
class OLDROCK_API UReaperAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UReaperAnimInstance();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	int32 PlayAttackMontage();

	void PlaySickleSkill1Montage();

	void PlayChangeToPaseMontage();

	void PlaySummonMontage();

	void PlayDarkBallMontage();

	void PlayRuneMontage();

public:
	UFUNCTION()
	void AnimNotify_AttackStart();

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_AttackCheck();

	UFUNCTION()
	void AnimNotify_BookOpenStart();

	UFUNCTION()
	void AnimNotify_EndPaseChanged();

	UFUNCTION()
	void AnimNotify_StartSummon();

	UFUNCTION()
	void AnimNotify_EndSummon();

	UFUNCTION()
	void AnimNotify_ShotDarkBall();

	UFUNCTION()
	void AnimNotify_EndDarkBall();

	UFUNCTION()
	void AnimNotify_StartRune();

	UFUNCTION()
	void AnimNotify_EndRune();

public:
	FOnAttackStartDelegate AttackStart;
	FOnAttackEndDelegate AttackEnd;
	FOnAttackCheckDelegate AttackCheck;
	FOnBookOpenStartDelegate BookOpenStart;
	FOnEndPaseChangedDelegate EndPaseChanged;
	FOnStartSummonDelegate StartSummon;
	FOnEndSummonDelegate EndSummon;
	FOnShotDarkBallDelegate ShotDarkBall;
	FOnEndDarkBallDelegate EndDarkBall;
	FOnStartRuneDelegate StartRune;
	FOnEndRuneDelegate EndRune;
	FOnSkillStartDelegate SkillStart;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool bDead;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* SickleSkill1Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ChangeToPaseMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	UAnimMontage* SummonMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DarkBallMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	UAnimMontage* RuneMontage;
};
