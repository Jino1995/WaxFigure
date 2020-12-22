// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "BossMonster.h"
#include "AIController.h"
#include "Reaper.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Normal,
	SickleSkill1
};

UENUM(BlueprintType)
enum class EPaseState : uint8
{
	First,
	Second
};

/**
 * 
 */
UCLASS()
class OLDROCK_API AReaper : public ABossMonster
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AReaper();

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

	void EXSpell();

	void EXSummon();

	void EXDarkBall();

	void EXRune();

	bool SummonSkeletonMan(FVector Location);

	void SummonRune(FVector Location);

	/**
	*
	*/
	void EXApplyDamage();

	void ApplyNormalAttackDamage();
	void ApplySickleSkill1Damage();
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

	void CheckDistanceToPlayer();

	void ChangeToSecondPase();

	//FORCEINLINE EMonsterState GetCurrentState() { return CurrentState; }

	FORCEINLINE void SickleSkill1On() { bSickleSkill1 = true; }

	FORCEINLINE EPaseState GetCurrentPaseState() { return CurrentPaseState; }

	FORCEINLINE void OnSummonCoolTime() { bSummon = true; }

	FORCEINLINE void OnDarkBallCoolTime() { bDarkBall = true; }

	FORCEINLINE void OnRuneCoolTime() { bRune = true; }

private:
	/**
	* Monster Sound
	* NormalSound : Use to stopped NormalSoundCue
	* DeadSound : Use to stopped DeadSoundCue
	* NormalSoundCue : Normal(idle, damaged...) sound
	* DeadSoundCue : if monster is dead, play this sound
	*/

	//
private:
	/**
	*/
	UPROPERTY()
	UParticleSystemComponent* SummonPSComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle", Meta = (AllowPrivateAccess = true))
	UParticleSystem* SummonPS;

	UPROPERTY()
	UParticleSystemComponent* DarkBallPSComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle", Meta = (AllowPrivateAccess = true))
	UParticleSystem* DarkBallPS;

	UPROPERTY()
	UParticleSystemComponent* RunePSComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle", Meta = (AllowPrivateAccess = true))
	UParticleSystem* RunePS;
	//

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* HairPart1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* SicklePart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AReaperSpellBook> SpellBookAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Parts", Meta = (AllowPrivateAccess = true))
	class AReaperSpellBook* SpellBook;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Range", Meta = (AllowPrivateAccess = true))
	class UReaperAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMonsters", Meta = (AllowPrivateAccess = true))
	TSubclassOf<AMonster> SpawnMonsterAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpawnMonsters", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AReaperExRune> SpawnRuneAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Range", Meta = (AllowPrivateAccess = true))
	class UBoxComponent* AttackRange;

	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* Sensing;

	UPROPERTY()
	class APlayerCharacter* CombatTarget;

	UPROPERTY(VisibleAnywhere)
	class AAIController* AIController;

	//EMonsterState CurrentState;
	EAttackType CurrentAttackType;

	TSet<class APlayerCharacter*> DamagedEnemy;

	int32 CurNormalATKPaternNum;

	float DistanceToPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AReaperDarkBall> SpawnDarkBall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<class AReaperExRune> SpawnRune;

private:
	EPaseState CurrentPaseState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pase, Meta = (AllowPrivateAccess = true))
	float SecondPaseHPRate;

private:
	/**
	* Timers
	*/
	FTimerHandle SickleSkill1Timer;
	FTimerHandle SpellSkillTimer;

	FTimerHandle SummonCoolTimeTimer;
	FTimerHandle DarkBallCoolTimeTimer;
	FTimerHandle RuneCoolTimeTimer;
	//

	/**
	* Skill CoolTime boolean
	*/
	bool bSickleSkill1;
	//

	/**
	* Skill CoolTime
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoolTime", Meta = (AllowPrivateAccess = true))
	float SickleSkill1CoolTime;
	//

private:
	bool bSummon;

	bool bDarkBall;

	bool bRune;
};
