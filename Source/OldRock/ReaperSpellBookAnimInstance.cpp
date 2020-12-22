// Fill out your copyright notice in the Description page of Project Settings.


#include "ReaperSpellBookAnimInstance.h"
#include "ReaperSpellBook.h"

UReaperSpellBookAnimInstance::UReaperSpellBookAnimInstance()
{
	bOpened = false;
}

void UReaperSpellBookAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	auto Pawn = Cast<AReaperSpellBook>(GetOwningActor());
	if(::IsValid(Pawn))
	{
		if (!bOpened && Pawn->GetIsOpened())
		{
			bOpened = true;
		}
	}
}