// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"

UENUM(BlueprintType)
enum class EMonsterState :uint8
{
	Patrol,
	Idle,
	Hunting,
	AttackStart,
	Attack,
	Damaged,
	Dead,
	ChangingPase,
	Skill
};

UCLASS()
class OLDROCK_API AMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EMonsterState GetCurrentState() { return CurrentState; }

protected:
	/**
	* An event that occurs when the player overlaped this range (this range is "AttackRange")
	*/
	UFUNCTION()
	virtual void AttackRangeOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {};

	UFUNCTION()
	virtual void AttackRangeOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {};
	//

protected:
	/** 
	* Monster' Stat
	* This values are changed at blueprint, You need to change this values at "BP_(Monster name)" 
	* MaxHP : Monster's Max HP
	* CurrentHP : Monster's Current HP
	* MonsterDamage : Monster's Damage
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", Meta = (AllowPrivateAccess = true))
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", Meta = (AllowPrivateAccess = true))
	float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", Meta = (AllowPrivateAccess = true))
	float MonsterDamage;
	//

	FTimerHandle AttackTimer;
	FTimerHandle DeadTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<UDamageType> DamageTypeClass;

	EMonsterState CurrentState;
};
