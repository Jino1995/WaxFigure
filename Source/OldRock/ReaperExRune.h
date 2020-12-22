// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "ReaperExRune.generated.h"

UCLASS()
class OLDROCK_API AReaperExRune : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReaperExRune();

	void Dead();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	class USoundBase* ExplosionSoundCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, Meta = (AllowPrivateAccess = true))
	USceneComponent* DefaultRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UParticleSystemComponent* RunePS;

	UParticleSystemComponent* ExplosionPSComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UParticleSystem* ExplosionPS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY()
	class AAIController* SkillInstigator;

	UPROPERTY()
	class APlayerCharacter* SkillOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	float SkillDamage;

	TSet<class APlayerCharacter*> DamagedEnemy;

private:
	FTimerHandle DestroyTimer;
	FTimerHandle CheckUserTimer;

private:
	void CheckUserInCombatRange();
};
