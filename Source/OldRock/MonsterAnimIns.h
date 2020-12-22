// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimIns.generated.h"

/**
 * 
 */
UCLASS()
class OLDROCK_API UMonsterAnimIns : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UMonsterAnimIns();

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=State, Meta = (AllowPrivateAccess = true))
	float CurrentSpeed;

};
