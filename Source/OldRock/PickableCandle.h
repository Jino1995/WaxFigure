// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "PickableCandle.generated.h"

UCLASS()
class OLDROCK_API APickableCandle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickableCandle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
		UStaticMeshComponent* Mesh2;
};
