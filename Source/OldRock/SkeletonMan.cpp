// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonMan.h"
#include "TimerManager.h"
#include "AIController.h"
#include "SkeletonManAnimInstance.h"
#include "Perception/PawnSensingComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

ASkeletonMan::ASkeletonMan()
{
	PrimaryActorTick.bCanEverTick = true;

	AttackRange = CreateDefaultSubobject<UBoxComponent>(TEXT("ATTACKRANGE"));
	AttackRange->SetupAttachment(GetRootComponent());

	Sensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PAWNSENSING"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("/Game/Monsters/Normal/SekeltonLight/Meshes/Lod/SkeletonLight_rig_FULL.SkeletonLight_rig_FULL"));
	if (SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fail to load"));
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimIns(TEXT("/Game/Monsters/Normal/SekeltonLight/AnimBP_SkeletonMan.AnimBP_SkeletonMan_C"));
	if (AnimIns.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimIns.Class);
	}

	GetCharacterMovement()->MaxWalkSpeed = 50.f;
	GetCharacterMovement()->SetWalkableFloorAngle(0.f);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentState = EMonsterState::Idle;
}

void ASkeletonMan::BeginPlay()
{
	Super::BeginPlay();

	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &ASkeletonMan::AttackRangeOnOverlapBegin);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &ASkeletonMan::AttackRangeOnOverlapEnd);

	Sensing->bSeePawns = true;
}

void ASkeletonMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMove())
	{
		if (CombatTarget)
		{
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation());
			FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
			FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotationYaw, DeltaTime, 5.0f);
			SetActorRotation(InterpRotation);
		}

		MoveToTarget();
	}
}

void ASkeletonMan::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<USkeletonManAnimInstance>(GetMesh()->GetAnimInstance());

	AnimInstance->AttackEnd.AddLambda([this]() -> void {
		if (CurrentState != EMonsterState::Dead)
		{
			CurrentState = EMonsterState::Hunting;
			DamagedEnemy.Empty();
		}
	});

	AnimInstance->DamagedEnd.AddLambda([this]() -> void {
		if (CurrentState != EMonsterState::Dead)
		{
			CurrentState = EMonsterState::Hunting;
		}
	});

	AnimInstance->DeadEnd.AddLambda([this]() -> void {
		GetMesh()->bPauseAnims = true;
		GetMesh()->bNoSkeletonUpdate = true;
		GetWorldTimerManager().SetTimer(DeadTimer, this, &ASkeletonMan::RemoveMonster, 2.f, false);
	});

	AnimInstance->AttackCheck.AddLambda([this]() -> void {
		if (CurrentState != EMonsterState::Dead)
			EXApplyDamage();
	});

	Sensing->OnSeePawn.AddDynamic(this, &ASkeletonMan::OnSeePawn);

}

float ASkeletonMan::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Error, TEXT("Take Damage(Police)"));
	if (!CombatTarget)
	{
		Sensing->bSeePawns = false;
		CombatTarget = Cast<APlayerCharacter>(DamageCauser);
	}

	Cast<AAIController>(GetController())->StopMovement();
	DamagedEnemy.Empty();

	if ((CurrentHP -= DamageAmount) > 0)
	{
		AnimInstance->PlayDamagedMontage();
		CurrentState = EMonsterState::Damaged;
	}
	else
	{
		Dead();
	}

	return DamageAmount;
}

void ASkeletonMan::AttackRangeOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &ASkeletonMan::EXAttack, 0.1f, true);
		OverlapTarget->SetMonster(this);
	}
}

void ASkeletonMan::AttackRangeOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}
}

void ASkeletonMan::EXAttack()
{
	if (CanAttack())
	{
		CurrentState = EMonsterState::Attack;
		AnimInstance->PlayAttackMontage();
	}
}

void ASkeletonMan::EXApplyDamage()
{
	TArray<FHitResult> HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>(EObjectTypeQuery::ObjectTypeQuery3));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bResult = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f, FVector(20.f, 20.f, 10.f), FRotator(0.f, 0.f, 0.f), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);

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
					UE_LOG(LogTemp, Error, TEXT("Apply Damage"));
					UGameplayStatics::ApplyDamage(Enemy, MonsterDamage, AIController, this, DamageTypeClass);
				}
			}
		}
	}
}

void ASkeletonMan::OnSeePawn(APawn *OtherPawn)
{
	Sensing->bSeePawns = false;
	CombatTarget = Cast<APlayerCharacter>(OtherPawn);
	CurrentState = EMonsterState::Hunting;
}

void ASkeletonMan::MoveToTarget()
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetAcceptanceRadius(0.0f);
	MoveRequest.SetGoalActor(CombatTarget);
	FNavPathSharedPtr NavPath;
	Cast<AAIController>(GetController())->MoveTo(MoveRequest, &NavPath);
}

bool ASkeletonMan::CanMove()
{
	if (CurrentState == EMonsterState::Hunting || CurrentState == EMonsterState::Attack)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ASkeletonMan::CanAttack()
{
	if (CurrentState != EMonsterState::Attack && CurrentState != EMonsterState::Dead && CurrentState != EMonsterState::Damaged)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ASkeletonMan::Dead()
{
	UGameplayStatics::PlaySound2D(GetWorld(), DeadSoundCue);

	if (GetWorldTimerManager().IsTimerActive(AttackTimer))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	CurrentState = EMonsterState::Dead;
	AnimInstance->PlayDeathMontage();
}

void ASkeletonMan::RemoveMonster()
{
	Destroy();
}