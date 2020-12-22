// Fill out your copyright notice in the Description page of Project Settings.


#include "UglyZombie.h"
#include "TimerManager.h"
#include "AIController.h"
#include "UglyZombieAnimInstance.h"
#include "Perception/PawnSensingComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

AUglyZombie::AUglyZombie()
{
	PrimaryActorTick.bCanEverTick = true;

	AttackRange = CreateDefaultSubobject<UBoxComponent>(TEXT("ATTACKRANGE"));
	AttackRange->SetupAttachment(GetRootComponent());

	Sensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PAWNSENSING"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("/Game/Monsters/Normal/Zombies/Ugly/Mesh/Ch10_nonPBR.Ch10_nonPBR"));
	if (SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fail to load"));
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimIns(TEXT("/Game/Monsters/Normal/Zombies/Ugly/AnimBP_UglyZombie.AnimBP_UglyZombie_C"));
	if (AnimIns.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimIns.Class);
	}

	CurrentPatrolPoint = 0;

	GetCharacterMovement()->MaxWalkSpeed = 100.f;
	GetCharacterMovement()->SetWalkableFloorAngle(0.f);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentState = EMonsterState::Idle;
}

void AUglyZombie::BeginPlay()
{
	Super::BeginPlay();

	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &AUglyZombie::AttackRangeOnOverlapBegin);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &AUglyZombie::AttackRangeOnOverlapEnd);

	if (bHasPatrol)
	{
		CurrentState = EMonsterState::Patrol;
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AUglyZombie::EXPatrol, 0.01, true);
	}

	Sensing->bSeePawns = true;

	NormalSound = UGameplayStatics::SpawnSoundAttached(NormalSoundCue, GetMesh(), TEXT("Head"), FVector(ForceInit), EAttachLocation::SnapToTarget, false, 1.f, 0.8f);
	BodyFirePSComp = UGameplayStatics::SpawnEmitterAttached(BodyFire, GetMesh(), TEXT("BodySocket"), FVector(ForceInit), FRotator(ForceInit),EAttachLocation::SnapToTarget);
}

void AUglyZombie::Tick(float DeltaTime)
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

void AUglyZombie::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UUglyZombieAnimInstance>(GetMesh()->GetAnimInstance());

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
			NormalSound->Play();
			CurrentState = EMonsterState::Hunting;
		}
	});

	AnimInstance->DeadEnd.AddLambda([this]() -> void {
		GetWorldTimerManager().SetTimer(DeadTimer, this, &AUglyZombie::ChangeBodyPSToDeadPS, 1.f, false);
	});

	AnimInstance->AttackCheck.AddLambda([this]() -> void {
		if (CurrentState != EMonsterState::Dead)
			EXApplyDamage();
	});

	Sensing->OnSeePawn.AddDynamic(this, &AUglyZombie::OnSeePawn);

}

float AUglyZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentState == EMonsterState::Dead)
	{
		return 0;
	}

	if (!CombatTarget)
	{
		if (GetWorldTimerManager().IsTimerActive(PatrolTimer))
		{
			GetWorldTimerManager().ClearTimer(PatrolTimer);
		}
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

void AUglyZombie::EXPatrol()
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetAcceptanceRadius(0.0f);
	FNavPathSharedPtr NavPath;

	if (PatrolResult)
	{
		if (PatrolResult.Code == EPathFollowingRequestResult::RequestSuccessful)
		{
			PatrolResult = Cast<AAIController>(GetController())->MoveTo(MoveRequest, &NavPath);
		}
		else if (PatrolResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			CurrentPatrolPoint = (CurrentPatrolPoint + 1) % PatrolPoint.Num();
			MoveRequest.SetGoalLocation(PatrolPoint[CurrentPatrolPoint]);
			PatrolResult = Cast<AAIController>(GetController())->MoveTo(MoveRequest, &NavPath);
		}
	}
	else
	{
		MoveRequest.SetGoalLocation(PatrolPoint[CurrentPatrolPoint]);
		PatrolResult = Cast<AAIController>(GetController())->MoveTo(MoveRequest, &NavPath);
	}
}

void AUglyZombie::AttackRangeOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AUglyZombie::EXAttack, 0.1f, true);
		OverlapTarget->SetMonster(this);
	}
}

void AUglyZombie::AttackRangeOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}
}

void AUglyZombie::EXAttack()
{
	if (CanAttack())
	{
		CurrentState = EMonsterState::Attack;
		AnimInstance->PlayAttackMontage();
	}
}

void AUglyZombie::EXApplyDamage()
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

void AUglyZombie::OnSeePawn(APawn *OtherPawn)
{
	if (GetWorldTimerManager().IsTimerActive(PatrolTimer))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
	}

	Sensing->bSeePawns = false;
	CombatTarget = Cast<APlayerCharacter>(OtherPawn);
	CurrentState = EMonsterState::Hunting;
}

void AUglyZombie::MoveToTarget()
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetAcceptanceRadius(0.0f);
	MoveRequest.SetGoalActor(CombatTarget);
	FNavPathSharedPtr NavPath;
	Cast<AAIController>(GetController())->MoveTo(MoveRequest, &NavPath);
}

bool AUglyZombie::CanMove()
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

bool AUglyZombie::CanAttack()
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

void AUglyZombie::Dead()
{
	UGameplayStatics::PlaySound2D(GetWorld(), DeadSoundCue);

	NormalSound->Stop();

	if (GetWorldTimerManager().IsTimerActive(PatrolTimer))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
	}
	else if (GetWorldTimerManager().IsTimerActive(AttackTimer))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	CurrentState = EMonsterState::Dead;
}

void AUglyZombie::ChangeBodyPSToDeadPS()
{
	BodyFirePSComp->DestroyComponent();
	UGameplayStatics::SpawnEmitterAttached(DeadFire, GetMesh(), TEXT("BodySocket"), FVector(ForceInit), FRotator(ForceInit), EAttachLocation::SnapToTarget);
}