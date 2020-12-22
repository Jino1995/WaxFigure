// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Monster.h"
#include "AIController.h"
#include "UglyZombie.generated.h"

/**
 * 
 */
UCLASS()
class OLDROCK_API AUglyZombie : public AMonster
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	AUglyZombie();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

	/**
	* An event that occurs when the player overlaped this range (this range is "AttackRange")
	*/
	virtual void AttackRangeOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void AttackRangeOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	//
public:
	// for play attack AnimMontage
	void EXAttack();

	// for play damaged AnimMontage
	void EXApplyDamage();

	/**
	* Play patrol Reconnaissance with specified coordinates
	* this coordinates are "TArray<FVector> PatrolPoint"
	* if "bool bHasPatrol" is false, do not execute 
	*/
	void EXPatrol();
	//

	/** 
	* Execute when the player is detected in PawnSensingComponent's field of view
	*/
	UFUNCTION()
	void OnSeePawn(APawn *OtherPawn);

	void MoveToTarget();

	bool CanMove();
	
	bool CanAttack();

	void Dead();

	// if UglyZombie is dead, change to particle
	void ChangeBodyPSToDeadPS();

	//FORCEINLINE EMonsterState GetCurrentState() { return CurrentState; }

private:
	/**
	* Monster Sound
	* NormalSound : Use to stopped NormalSoundCue 
	* DeadSound : Use to stopped DeadSoundCue 
	* NormalSoundCue : Normal(idle, damaged...) sound
	* DeadSoundCue : if monster is dead, play this sound
	*/
	UPROPERTY()
	UAudioComponent* NormalSound;

	UPROPERTY()
	UAudioComponent* DeadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	class USoundBase* NormalSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", Meta = (AllowPrivateAccess = true))
	class USoundBase* DeadSoundCue;
	//
private:
	/**
	* Particle
	* BodyFirePSComp : for change to Deadfire Particle
	* BodyFire : Ugly Zombie's basic particle
	* DeadFire : Ugly Zombie's particle after death
	*/
	UPROPERTY()
	UParticleSystemComponent* BodyFirePSComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle", Meta = (AllowPrivateAccess = true))
	UParticleSystem* BodyFire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle", Meta = (AllowPrivateAccess = true))
	UParticleSystem* DeadFire;
	//
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Range", Meta = (AllowPrivateAccess = true))
	class UUglyZombieAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Range", Meta = (AllowPrivateAccess = true))
	class UBoxComponent* AttackRange;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* Sensing;

	UPROPERTY()
	class APlayerCharacter* CombatTarget;

	UPROPERTY(VisibleAnywhere)
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (AllowPrivateAccess = true))
	TArray<FVector> PatrolPoint;

	int32 CurrentPatrolPoint;

	FPathFollowingRequestResult PatrolResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (AllowPrivateAccess = true))
	bool bHasPatrol;

	TSet<class APlayerCharacter*> DamagedEnemy;

private:
	/**
	* Timers
	*/
	FTimerHandle PatrolTimer;
	//
};