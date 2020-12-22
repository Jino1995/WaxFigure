// Fill out your copyright notice in the Description page of Project Settings.


#include "Water.h"
#include "Engine/Classes/Components/SphereComponent.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Monster.h"
#include "UglyZombie.h"
#include "Kismet/KismetMathLibrary.h"
// Sets default values
AWater::AWater()
{
	InitialLifeSpan = 100.0f;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CombatCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CombatCollision->InitSphereRadius(15.0f);
	RootComponent = CombatCollision;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CombatCollision);
	ProjectileMovementComponent->InitialSpeed = 700.0f;
	ProjectileMovementComponent->MaxSpeed = 700.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	
	Water_G = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MovementParticles"));
	Water_G->SetupAttachment(RootComponent, NAME_None);
	Water_G->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); 
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/Annabel/particles/P_ky_stabShot1.P_ky_stabShot1"));
	if (ParticleAsset.Succeeded())
	{
		Water_G->SetTemplate(ParticleAsset.Object);

	}
	Water_G->SetRelativeScale3D(FVector(0.3f, 0.045f, 0.045f));

	WaterImpact = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MovementParticles2"));
	WaterImpact->SetupAttachment(RootComponent, NAME_None);
	WaterImpact->bAutoActivate = false;
	WaterImpact->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset2(TEXT("/Game/AdvancedMagicFX12/particles/P_ky_hit_water.P_ky_hit_water"));
	if (ParticleAsset2.Succeeded())
	{
		WaterImpact->SetTemplate(ParticleAsset2.Object);

	}
	
	
}

// Called when the game starts or when spawned
void AWater::BeginPlay()
{
	Super::BeginPlay();
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	//CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWater::CombatCollisionOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWater::CombatCollisionOnOverlapEnd);
	//CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	WaterSound = UGameplayStatics::SpawnSoundAttached(WaterSoundCue, RootComponent, NAME_None, FVector::ZeroVector, EAttachLocation::SnapToTarget, false, 1.f, 0.8f);

	WaterSound->Deactivate();
}

// Called every frame
void AWater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWater::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
void AWater::CombatCollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
	
		Monster = Cast<AMonster>(OtherActor);
		if (Monster)
		{
			if (DamageTypeClass)
			{
				//if (Monster->GetCurrentState() != EMonsterState::Dead) {
					WaterSound->Play();
					WaterImpact->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
					WaterImpact->Activate();
					UGameplayStatics::ApplyDamage(Monster, 20, Controller, PlayerCharacter, DamageTypeClass);
					Water_G->Deactivate();
					UE_LOG(LogTemp, Error, TEXT("damage"));
					CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				//}
			}
		}
	}

}
void AWater::Damage() {
	//UGameplayStatics::ApplyDamage(Player, 20, a, this, DamageTypeClass);
}

void AWater::CombatCollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
void AWater::Fire_GetAIController(class AController* aa, class ACharacter* bb) {
	Controller = aa;
	PlayerCharacter = bb;
}
void AWater::SetCollision() {
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}