// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Animation/AnimInstance.h"
#include "ReaperSpellBookAnimInstance.generated.h"

enum class EOpenState :uint8
{
	Closed,
	Open
};

/**
 * 
 */
UCLASS()
class OLDROCK_API UReaperSpellBookAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UReaperSpellBookAnimInstance();
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool bOpened;

};
