// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ReaperDarkBall.generated.h"

UCLASS()
class OLDROCK_API AReaperDarkBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AReaperDarkBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void FireInDirection(const FVector& ShootDirection);

	void Dead();

	FORCEINLINE void SetSkillOwner(class APlayerCharacter* OwnerCharacter) { SkillOwner = OwnerCharacter; }
	FORCEINLINE void SetInstigator(class AAIController* Inst) { SkillInstigator = Inst; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	class USoundBase* ExplosionSoundCue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, Meta = (AllowPrivateAccess = true))
	USceneComponent* DefaultRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UParticleSystemComponent* DarkBall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UParticleSystem* BoomPS;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	USphereComponent* CollisionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", Meta = (AllowPrivateAccess = true))
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY()
	class AAIController* SkillInstigator;

	UPROPERTY()
	APlayerCharacter* SkillOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage, Meta = (AllowPrivateAccess = true))
	float SkillDamage;

	TSet<class APlayerCharacter*> DamagedEnemy;

private:
	FTimerHandle DestroyTimer;
	FTimerHandle CheckUserTimer;

private:
	void CheckUserInCombatRange();

};
