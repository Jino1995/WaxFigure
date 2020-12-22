// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "ReaperSpellBook.generated.h"

UCLASS()
class OLDROCK_API AReaperSpellBook : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReaperSpellBook();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	FORCEINLINE bool GetIsOpened() { return bOpening; }
	FORCEINLINE void PlayOpenBook() { bOpening = true; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, Meta = (AllowPrivateAccess = true))
	USceneComponent* DefaultRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, Meta = (AllowPrivateAccess = true))
	bool bOpening = false;
};
