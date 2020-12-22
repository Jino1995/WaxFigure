// Fill out your copyright notice in the Description page of Project Settings.


#include "ReaperExRune.h"
#include "PlayerCharacter.h"
#include "AIController.h"

// Sets default values
AReaperExRune::AReaperExRune()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DEFAULTROOT"));
	SetRootComponent(DefaultRootComponent);

	RunePS = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RUNE"));
	RunePS->SetupAttachment(GetRootComponent());

	ExplosionPSComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EXPLOSIONCOMP"));
	ExplosionPSComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AReaperExRune::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AReaperExRune::Dead, 10.0f);
	GetWorldTimerManager().SetTimer(CheckUserTimer, this, &AReaperExRune::CheckUserInCombatRange, 0.1f, true);
}

// Called every frame
void AReaperExRune::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AReaperExRune::Dead()
{
	if (ExplosionPSComp)
	{
		ExplosionPSComp->DestroyComponent();
	}
	Destroy();
}

void AReaperExRune::CheckUserInCombatRange()
{
	TArray<FHitResult> HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>(EObjectTypeQuery::ObjectTypeQuery3));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bResult;

	bResult = UKismetSystemLibrary::CapsuleTraceMultiForObjects(GetWorld(), GetActorLocation(), GetActorLocation(), 160.f, 10.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 1.f);
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
					ExplosionPSComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionPS, GetActorLocation());
					UGameplayStatics::ApplyDamage(Enemy, SkillDamage, SkillInstigator, SkillOwner, DamageTypeClass);
					GetWorldTimerManager().SetTimer(DestroyTimer, this, &AReaperExRune::Dead, 1.5f);
				}
			}
		}
	}
}