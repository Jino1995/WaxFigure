// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "Candle.generated.h"

UCLASS()
class OLDROCK_API ACandle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACandle();
	void FireSize(float size);
	void CandleSize(float size);
	void FireInitSize(float size);
	float InitSize;
	FVector Torchsize;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

private:
	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* Mesh2;

	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UParticleSystem* Fire;

	UPROPERTY(VisibleAnywhere, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UPointLightComponent* PointLight;
};
