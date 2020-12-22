// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABAnimInstance.h"
#include "AIController.h"
#include "Water.h"
#include "WaterGun.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class OLDROCK_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CheckCandle();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void MoveForward(float NewAxisValue);
	virtual void PostInitializeComponents() override;
	void MoveRight(float NewAxisValue);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Candle")
		bool OnHand = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Candle")
	float candleHP = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Candle")
	float fireSize = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Candle")
	float InvincibleTime = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Candle")
	float evasionSelfDamage = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Candle")
		float evasionCoolTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float IsDead;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class AAIController* AIController;
	float maxcandle;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	void InvincibleEnd();
	void Fire();
	void Fire_Launch();
	void evasion();
	void evasionCoolDown();
	void evasionCool();
	bool canEvasion = true;
	bool bevasion=false; 
		bool Invincible = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackMontage, Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMTG;

	class UABAnimInstance* AnimInstance;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AWater> ProjectileClass;

	FTimerHandle InvincibleTimer;
	FTimerHandle evasionTimer;
	UPROPERTY()
		class AMonster* Monster;
	void SetMonster(AMonster* NewTarget);
	UFUNCTION(BlueprintCallable, Category = "Candle")
		float GetCurrentHP() { return candleHP; }
	UFUNCTION(BlueprintCallable, Category = "Candle")
		float GetMaxHP() { return maxcandle; }
	UFUNCTION(BlueprintCallable, Category = "Candle")
		float GetCurrentCooltime() { return currentCoolTime; }
	UFUNCTION(BlueprintCallable, Category = "Candle")
		float GetMaxCooltime() { return evasionCoolTime*50; }
	float currentCoolTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
		class 	UBoxComponent* ECollision;

	UFUNCTION()
		virtual void ECollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void ECollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void Interaction();
	float objectNum=0;

	TArray<AActor*> objectArr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction, Meta = (AllowPrivateAccess = true))
	TArray<FString> objectNameArr;
	TArray<FString> ActivedObjectNameArr;
	TArray<AActor*> PickableObject;
	bool candlePicked=false;
	bool gunPicked=false;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = true))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = true))
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class ACandle> BP_Candle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", Meta = (AllowPrivateAccess = true))
		TSubclassOf<class AWaterGun> BP_WaterGun;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = true))
	class ACandle* Candle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = true))
	class AWaterGun* WaterGun1;
	FVector DirectionToMove = FVector::ZeroVector;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI, Meta = (AllowPrivateAccess = true))
		TSubclassOf<class UPauseMenuWidget> PauseMenuAsset1;*/
	void VisiblePauseMenu();
};
