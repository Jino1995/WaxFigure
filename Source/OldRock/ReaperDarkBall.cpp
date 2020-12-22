// Fill out your copyright notice in the Description page of Project Settings.


#include "ReaperDarkBall.h"
#include "PlayerCharacter.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "AIController.h"

// Sets default values
AReaperDarkBall::AReaperDarkBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DEFAULTROOT"));
	SetRootComponent(DefaultRootComponent);

	DarkBall = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DARKBALL"));
	DarkBall->SetupAttachment(GetRootComponent());

	CollisionRange = CreateDefaultSubobject<USphereComponent>(TEXT("COLLISIONRANGE"));
	CollisionRange->SetupAttachment(GetRootComponent());
	CollisionRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(GetRootComponent());
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->MaxSpeed = 1000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
}

// Called when the game starts or when spawned
void AReaperDarkBall::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AReaperDarkBall::Dead, 3.0f);
	GetWorldTimerManager().SetTimer(CheckUserTimer, this, &AReaperDarkBall::CheckUserInCombatRange, 0.1f, true);
}

// Called every frame
void AReaperDarkBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AReaperDarkBall::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AReaperDarkBall::Dead()
{
	Destroy();
}

void AReaperDarkBall::CheckUserInCombatRange()
{
	TArray<FHitResult> HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>(EObjectTypeQuery::ObjectTypeQuery3));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bResult;

	bResult = UKismetSystemLibrary::CapsuleTraceMultiForObjects(GetWorld(), GetActorLocation(), GetActorLocation(), 100.f, 100.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 1.f);
	if (bResult)
	{
		for (auto Result : HitResult)
		{
			APlayerCharacter* Enemy = Cast<APlayerCharacter>(Result.GetActor());
			if (Enemy && DamageTypeClass)
			{
				if (!DamagedEnemy.Contains(Enemy))
				{
					DamagedEnemy.Emplace(Enemy);
					UGameplayStatics::PlaySound2D(GetWorld(), ExplosionSoundCue);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BoomPS, GetActorLocation());
					UGameplayStatics::ApplyDamage(Enemy, SkillDamage, SkillInstigator, SkillOwner, DamageTypeClass);
					Destroy();
				}
			}
		}
	}
}