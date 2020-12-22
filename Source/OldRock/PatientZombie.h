// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster.h"
#include "PatientZombie.generated.h"

/**
 * 
 */
UCLASS()
class OLDROCK_API APatientZombie : public AMonster
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APatientZombie();

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


	UFUNCTION()
	void OnSeePawn(APawn *OtherPawn);


	void MoveToTarget();

	bool CanMove();

	bool CanAttack();

	void Dead();

	FORCEINLINE bool IsCombat() { return bCombat; }

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Range", Meta = (AllowPrivateAccess = true))
	class UPatientZombieAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Range", Meta = (AllowPrivateAccess = true))
	class UBoxComponent* AttackRange;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* Sensing;

	UPROPERTY()
	class APlayerCharacter* CombatTarget;

	UPROPERTY(VisibleAnywhere)
	class AAIController* AIController;

	TSet<class APlayerCharacter*> DamagedEnemy;

	bool bCombat;
};
