// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OLDROCK_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AABPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void VisiblePauseMenu();

private:
	/** Pause Menu */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class UPauseMenuWidget> PauseMenuAsset1;

	UPROPERTY()
	UPauseMenuWidget* PauseMenu;
	//

	bool bPauseMenu;

};
