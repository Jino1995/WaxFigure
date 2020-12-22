// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Monster.h"
#include "Water.generated.h"


UCLASS()
class OLDROCK_API AWater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWater();

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		class 	USphereComponent* CombatCollision;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
		class 	UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION()
		virtual void CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Fire_GetAIController(class AController* aa, class ACharacter* bb);
	void SetCollision();
	AAIController* a;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class AController* Controller;
	class ACharacter* PlayerCharacter;
	void Damage();
	AMonster* Monster;
	UPROPERTY()
		UAudioComponent* WaterSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
		class USoundBase* WaterSoundCue;
	class UParticleSystemComponent* WaterImpact;
	class UParticleSystemComponent* Water_G;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<UDamageType> DamageTypeClass;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireInDirection(const FVector & ShootDirection);

};
