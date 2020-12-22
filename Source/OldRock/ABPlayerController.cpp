// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.h"

AABPlayerController::AABPlayerController()
{
	bPauseMenu = false;
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Error, TEXT("312"));
	if (PauseMenuAsset1)
	{
		PauseMenu = CreateWidget<UPauseMenuWidget>(this, PauseMenuAsset1);
		PauseMenu->AddToViewport();
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AABPlayerController::VisiblePauseMenu()
{
	if (bPauseMenu)
	{
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		bShowMouseCursor = false;
		SetPause(false);
		bPauseMenu = false;
	}
	else
	{
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
		bShowMouseCursor = true;
		SetPause(true);
		bPauseMenu = true;
	}
}