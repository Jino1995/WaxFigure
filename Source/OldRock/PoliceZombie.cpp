// Fill out your copyright notice in the Description page of Project Settings.


#include "PoliceZombie.h"
#include "TimerManager.h"
#include "AIController.h"
#include "PoliceZombieAnimInstance.h"
#include "Perception/PawnSensingComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

APoliceZombie::APoliceZombie()
{
	PrimaryActorTick.bCanEverTick = true;

	AttackRange = CreateDefaultSubobject<UBoxComponent>(TEXT("ATTACKRANGE"));
	AttackRange->SetupAttachment(GetRootComponent());

	Sensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PAWNSENSING"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("/Game/Monsters/Normal/Zombies/Police/Mesh/derrick.derrick"));
	if (SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fail to load"));
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimIns(TEXT("/Game/Monsters/Normal/Zombies/Police/AnimBP_PoliceZombie.AnimBP_PoliceZombie_C"));
	if (AnimIns.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimIns.Class);
	}

	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->SetWalkableFloorAngle(0.f);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentState = EMonsterState::Idle;
}

void APoliceZombie::BeginPlay()
{
	Super::BeginPlay();

	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &APoliceZombie::AttackRangeOnOverlapBegin);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &APoliceZombie::AttackRangeOnOverlapEnd);

	Sensing->bSeePawns = true;

	NormalSound = UGameplayStatics::SpawnSoundAttached(NormalSoundCue, GetMesh(), TEXT("Head"), FVector(ForceInit), EAttachLocation::SnapToTarget, false, 1.f, 0.8f);
}

void APoliceZombie::Tick(float DeltaTime)
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

void APoliceZombie::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UPoliceZombieAnimInstance>(GetMesh()->GetAnimInstance());

	AnimInstance->AttackEnd.AddLambda([this]() -> void {
		if (CurrentState != EMonsterState::Dead)
		{
			CurrentState = EMonsterState::Hunting;
			DamagedEnemy.Empty();
		}
	});

	AnimInstance->AttackCheck.AddLambda([this]() -> void {
		if (CurrentState != EMonsterState::Dead)
			EXApplyDamage();
	});

	Sensing->OnSeePawn.AddDynamic(this, &APoliceZombie::OnSeePawn);
}

float APoliceZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentState == EMonsterState::Dead)
	{
		return 0;
	}

	if (!CombatTarget)
	{
		CombatTarget = Cast<APlayerCharacter>(DamageCauser);
	}

	if ((CurrentHP -= DamageAmount) <= 0)
	{
		Dead();
	}
	
	return DamageAmount;
}

void APoliceZombie::AttackRangeOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &APoliceZombie::EXAttack, 0.1f, true);
		OverlapTarget->SetMonster(this);
	}
}

void APoliceZombie::AttackRangeOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}
}

void APoliceZombie::EXAttack()
{
	if (CanAttack())
	{
		CurrentState = EMonsterState::Attack;
		AnimInstance->PlayAttackMontage();
	}
}

void APoliceZombie::EXApplyDamage()
{
	/*
	FHitResult HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>(EObjectTypeQuery::ObjectTypeQuery3));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bResult = UKismetSystemLibrary::BoxTraceSingleForObjects(GetWorld(), GetMesh()->GetSocketLocation(FName("AttackStart")), GetMesh()->GetSocketLocation(FName("AttackEnd")), FVector(30.f, 10.f, 10.f), FRotator(0.f, 0.f, 0.f), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);
	if (bResult)
	{
		APlayerCharacter* Enemy = Cast<APlayerCharacter>(HitResult.GetActor());
		if (Enemy && DamageTypeClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Apply Damage"));
			UGameplayStatics::ApplyDamage(Enemy, MonsterDamage, AIController, this, DamageTypeClass);
		}
	}
	*/
	TArray<FHitResult> HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>(EObjectTypeQuery::ObjectTypeQuery3));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bResult = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), GetMesh()->GetSocketLocation(FName("AttackStart")), GetMesh()->GetSocketLocation(FName("AttackEnd")), FVector(30.f, 10.f, 10.f), FRotator(0.f, 0.f, 0.f), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f);

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

void APoliceZombie::OnSeePawn(APawn *OtherPawn)
{
	Sensing->bSeePawns = false;
	CombatTarget = Cast<APlayerCharacter>(OtherPawn);
	CurrentState = EMonsterState::Hunting;
}

void APoliceZombie::MoveToTarget()
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetAcceptanceRadius(0.0f);
	MoveRequest.SetGoalActor(CombatTarget);
	FNavPathSharedPtr NavPath;
	Cast<AAIController>(GetController())->MoveTo(MoveRequest, &NavPath);
}

bool APoliceZombie::CanMove()
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

bool APoliceZombie::CanAttack()
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

void APoliceZombie::Dead()
{
	CurrentState = EMonsterState::Dead;
	UGameplayStatics::PlaySound2D(GetWorld(), DeadSoundCue);

	NormalSound->Stop();

	if (GetWorldTimerManager().IsTimerActive(AttackTimer))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}