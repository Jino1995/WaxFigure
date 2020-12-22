// Fill out your copyright notice in the Description page of Project Settings.


#include "Reaper.h"
#include "ReaperAnimInstance.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "ReaperSpellBook.h"
#include "SkeletonMan.h"
#include "NavigationSystem.h"
#include "ReaperDarkBall.h"
#include "ReaperExRune.h"

AReaper::AReaper()
{
	PrimaryActorTick.bCanEverTick = true;

	AttackRange = CreateDefaultSubobject<UBoxComponent>(TEXT("ATTACKRANGE"));
	AttackRange->SetupAttachment(GetRootComponent());

	Sensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PAWNSENSING"));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("/Game/Monsters/Boss/Stage1/TheReaper/Meshes/Characters/Combines/SK_ReaperB.SK_ReaperB"));
	if (SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fail to load"));
	}

	HairPart1 = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HAIRPARTS1"));
	HairPart1->SetupAttachment(GetMesh());
	HairPart1->SetMasterPoseComponent(GetMesh());

	SicklePart = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SICKLEPARTS"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Sickle(TEXT("/Game/Monsters/Boss/Stage1/TheReaper/Meshes/Characters/Separates/Acessories/SK_Scythe.SK_Scythe"));
	if (SK_Sickle.Succeeded())
	{
		SicklePart->SetSkeletalMesh(SK_Sickle.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fail to load"));
	}

	SicklePart->SetupAttachment(GetMesh());
	SicklePart->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("WEAPON_R"));

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimIns(TEXT("/Game/Monsters/Boss/Stage1/TheReaper/AnimBP_Reaper.AnimBP_Reaper_C"));
	if (AnimIns.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimIns.Class);
	}

	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->SetWalkableFloorAngle(0.f);

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CurrentState = EMonsterState::Idle;

	bSickleSkill1 = true;

	CurrentPaseState = EPaseState::First;

	bSummon = true;
	bDarkBall = true;
	bRune = true;
}

void AReaper::BeginPlay()
{
	Super::BeginPlay();
	
	Sensing->bSeePawns = true;
	
	AttackRange->OnComponentBeginOverlap.AddDynamic(this, &AReaper::AttackRangeOnOverlapBegin);
	AttackRange->OnComponentEndOverlap.AddDynamic(this, &AReaper::AttackRangeOnOverlapEnd);

	SpellBook = GetWorld()->SpawnActor<AReaperSpellBook>(SpellBookAsset, FVector::ZeroVector, FRotator::ZeroRotator);
	SpellBook->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("BOOK"));

}

void AReaper::Tick(float DeltaTime)
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

		if (CurrentState != EMonsterState::AttackStart)
		{
			MoveToTarget();
		}
	}

	if (CombatTarget && CanAttack())
	{
		if (bSickleSkill1 && DistanceToPlayer <= 500)
		{
			CurrentState = EMonsterState::AttackStart;
			Cast<AAIController>(GetController())->StopMovement();

			CurrentAttackType = EAttackType::SickleSkill1;
			AnimInstance->PlaySickleSkill1Montage();
			bSickleSkill1 = false;
			GetWorldTimerManager().SetTimer(SickleSkill1Timer, this, &AReaper::SickleSkill1On, SickleSkill1CoolTime, false);
		}
	}
}

void AReaper::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AnimInstance = Cast<UReaperAnimInstance>(GetMesh()->GetAnimInstance());

	AnimInstance->AttackStart.AddLambda([this]() -> void {
		CurrentState = EMonsterState::Attack;
	});

	AnimInstance->AttackEnd.AddLambda([this]() -> void {
		if (CurrentState != EMonsterState::Dead)
		{
			CurrentState = EMonsterState::Hunting;
			DamagedEnemy.Empty();
		}
	});

	AnimInstance->AttackCheck.AddLambda([this]() -> void {
		EXApplyDamage();
	});
	
	AnimInstance->BookOpenStart.AddLambda([this]() -> void {
		SpellBook->PlayOpenBook();
	});

	AnimInstance->EndPaseChanged.AddLambda([this]() -> void {
		CurrentState = EMonsterState::Hunting;
		GetWorldTimerManager().SetTimer(SpellSkillTimer, this, &AReaper::EXSpell, 3.f, true);
	});

	AnimInstance->StartSummon.AddLambda([this]() -> void {
		EXSummon();
	});

	AnimInstance->EndSummon.AddLambda([this]() -> void {
		SummonPSComp->DestroyComponent();
		CurrentState = EMonsterState::Hunting;
		GetWorldTimerManager().SetTimer(SummonCoolTimeTimer, this, &AReaper::OnSummonCoolTime, 30.f, false);
	});

	AnimInstance->ShotDarkBall.AddLambda([this]() -> void {
		CurrentState = EMonsterState::Skill;
		EXDarkBall();
	});

	AnimInstance->EndDarkBall.AddLambda([this]() -> void {
		DarkBallPSComp->DestroyComponent();
		CurrentState = EMonsterState::Hunting;
		GetWorldTimerManager().SetTimer(DarkBallCoolTimeTimer, this, &AReaper::OnDarkBallCoolTime, 3.f, false);
	});

	AnimInstance->StartRune.AddLambda([this]() -> void {
		EXRune();
	});

	AnimInstance->EndRune.AddLambda([this]() -> void {
		RunePSComp->DestroyComponent();
		CurrentState = EMonsterState::Hunting;
		GetWorldTimerManager().SetTimer(RuneCoolTimeTimer, this, &AReaper::OnRuneCoolTime, 10.f, false);
	});

	Sensing->OnSeePawn.AddDynamic(this, &AReaper::OnSeePawn);
}

float AReaper::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentState == EMonsterState::ChangingPase || CurrentState == EMonsterState::Skill)
	{
		return 0.f;
	}

	if (!CombatTarget)
	{
		Sensing->bSeePawns = false;
		CombatTarget = Cast<APlayerCharacter>(DamageCauser);
	}

	DamagedEnemy.Empty();

	if ((CurrentHP -= DamageAmount) > 0)
	{
		switch (CurrentPaseState)
		{
		case EPaseState::First:
			if (MaxHP * (SecondPaseHPRate / 100) >= CurrentHP)
			{
				ChangeToSecondPase();
				CurrentPaseState = EPaseState::Second;
			}
			break;
		}
	}
	else
	{
		Dead();
	}

	return DamageAmount;
}

void AReaper::AttackRangeOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AReaper::EXAttack, 0.1f, true);
		OverlapTarget->SetMonster(this);
	}
}

void AReaper::AttackRangeOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}
}

void AReaper::EXAttack()
{
	if (CanAttack())
	{
		CurrentState = EMonsterState::AttackStart;
		Cast<AAIController>(GetController())->StopMovement();

		int32 SelectAttackNum = rand() % 1 + 1;

		switch (SelectAttackNum)
		{
		case 1:
			CurrentAttackType = EAttackType::Normal;
			CurNormalATKPaternNum = AnimInstance->PlayAttackMontage();
			break;
		}
	}
}

void AReaper::EXSpell()
{
	if (CurrentState != EMonsterState::Skill && CurrentState != EMonsterState::Attack)
	{
		Cast<AAIController>(GetController())->StopMovement();
		int32 SelectNum = rand() % 3 + 1;

		switch (SelectNum)
		{
		case 1:
			if (bSummon)
			{
				bSummon = false;
				CurrentState = EMonsterState::Skill;
				SummonPSComp = UGameplayStatics::SpawnEmitterAttached(SummonPS, GetMesh(), TEXT("Root"), FVector(ForceInit), FRotator(ForceInit), EAttachLocation::SnapToTarget);
				AnimInstance->PlaySummonMontage();
			}
			else
			{
				EXSpell();
			}
			break;
		case 2:
			if (bDarkBall)
			{
				bDarkBall = false;
				CurrentState = EMonsterState::AttackStart;
				DarkBallPSComp = UGameplayStatics::SpawnEmitterAttached(DarkBallPS, GetMesh(), TEXT("DarkBallRun"), FVector(ForceInit), FRotator(ForceInit), EAttachLocation::SnapToTarget);
				DarkBallPSComp->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
				AnimInstance->PlayDarkBallMontage();
			}
			else
			{
				EXSpell();
			}
			break;
		case 3:
			if (bRune)
			{
				bRune = false;
				CurrentState = EMonsterState::Skill;
				RunePSComp = UGameplayStatics::SpawnEmitterAttached(RunePS, GetMesh(), TEXT("Root"), FVector(ForceInit), FRotator(ForceInit), EAttachLocation::SnapToTarget);
				AnimInstance->PlayRuneMontage();
			}
			else
			{
				EXSpell();
			}
			break;
		}
	}
}

void AReaper::EXSummon()
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	int SummonNum = 5;

	if (NavSystem != nullptr)
	{
		while(SummonNum >= 1)
		{
			FNavLocation SpawnLocation;
			NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 400.f, SpawnLocation);
			if (SummonSkeletonMan(SpawnLocation))
			{
				SummonNum--;
			}
		}
	}
}

void AReaper::EXDarkBall()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	FVector SpawnLocation = GetMesh()->GetSocketLocation(FName("DarkBallHand"));
	
	AReaperDarkBall* Projectile = GetWorld()->SpawnActor<AReaperDarkBall>(SpawnDarkBall, SpawnLocation, GetActorRotation(), SpawnParams);
	if (Projectile)
	{
		FVector LaunchDirection = GetActorRotation().Vector();
		Projectile->FireInDirection(LaunchDirection);
	}
}

void AReaper::EXRune()
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	if (NavSystem != nullptr)
	{
		for (int i = 0; i < 3; i++)
		{
			FNavLocation SpawnLocation;
			NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 500.f, SpawnLocation);
			SummonRune(SpawnLocation);
		}
	}
}

bool AReaper::SummonSkeletonMan(FVector Location)
{
	auto Skeleton = GetWorld()->SpawnActor<ASkeletonMan>(SpawnMonsterAsset, Location, FRotator::ZeroRotator);
	if (Skeleton)
	{
		Skeleton->SpawnDefaultController();
		return true;
	}
	else
	{
		return false;
	}
}

void AReaper::SummonRune(FVector Location)
{
	auto Rune = GetWorld()->SpawnActor<AReaperExRune>(SpawnRuneAsset, Location, FRotator::ZeroRotator);
	if (!Rune)
	{
		SummonRune(Location);
	}
}

void AReaper::EXApplyDamage()
{
	switch (CurrentAttackType)
	{
	case EAttackType::Normal:
		ApplyNormalAttackDamage();
		break;
	case EAttackType::SickleSkill1:
		ApplySickleSkill1Damage();
		break;
	}
}

void AReaper::ApplyNormalAttackDamage()
{
	TArray<FHitResult> HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>(EObjectTypeQuery::ObjectTypeQuery3));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bResult;

	switch (CurNormalATKPaternNum)
	{
	case 1:
		bResult = UKismetSystemLibrary::CapsuleTraceMultiForObjects(GetWorld(), GetActorLocation() + GetActorForwardVector() * 50.f, GetActorLocation() + GetActorForwardVector() * 100.f, 160.f, 10.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 1.f);
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
						UGameplayStatics::ApplyDamage(Enemy, MonsterDamage, AIController, this, DamageTypeClass);
					}
				}
			}
		}
		break;
	case 2:
		bResult = UKismetSystemLibrary::CapsuleTraceMultiForObjects(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 200.f, 50.f, 10.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 1.f);
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
						UGameplayStatics::ApplyDamage(Enemy, MonsterDamage, AIController, this, DamageTypeClass);
					}
				}
			}
		}
		break;
	case 3:
		bResult = UKismetSystemLibrary::CapsuleTraceMultiForObjects(GetWorld(), GetActorLocation() + GetActorForwardVector() * 50.f, GetActorLocation() + GetActorForwardVector() * 100.f, 160.f, 10.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 1.f);
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
						UGameplayStatics::ApplyDamage(Enemy, MonsterDamage, AIController, this, DamageTypeClass);
					}
				}
			}
		}
		break;
	}
}

void AReaper::ApplySickleSkill1Damage()
{
	TArray<FHitResult> HitResult;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>(EObjectTypeQuery::ObjectTypeQuery3));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	bool bResult;
	bResult = UKismetSystemLibrary::CapsuleTraceMultiForObjects(GetWorld(), GetActorLocation() + GetActorForwardVector() * 400.f, GetActorLocation() + GetActorForwardVector() * 50.f, 160.f, 10.f, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true, FLinearColor::Red, FLinearColor::Green, 1.f);
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
					UE_LOG(LogTemp, Error, TEXT("Apply Skill Damage"));
					UGameplayStatics::ApplyDamage(Enemy, 30, AIController, this, DamageTypeClass);
				}
			}
		}
	}
}

void AReaper::OnSeePawn(APawn *OtherPawn)
{
	UE_LOG(LogTemp, Error, TEXT("Reaper OnSeePawn"));
	Sensing->bSeePawns = false;
	CombatTarget = Cast<APlayerCharacter>(OtherPawn);
	CurrentState = EMonsterState::Hunting;
}

void AReaper::MoveToTarget()
{
	FAIMoveRequest MoveRequest;
	MoveRequest.SetAcceptanceRadius(0.0f);
	MoveRequest.SetGoalActor(CombatTarget);
	FNavPathSharedPtr NavPath;
	Cast<AAIController>(GetController())->MoveTo(MoveRequest, &NavPath);
}

bool AReaper::CanMove()
{
	if (CurrentState == EMonsterState::Hunting || CurrentState == EMonsterState::AttackStart)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool AReaper::CanAttack()
{
	if (CurrentState != EMonsterState::Attack && CurrentState != EMonsterState::Dead && CurrentState != EMonsterState::Damaged && CurrentState != EMonsterState::AttackStart && CurrentState != EMonsterState::ChangingPase && CurrentState != EMonsterState::Skill)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AReaper::Dead()
{
	if (GetWorldTimerManager().IsTimerActive(AttackTimer))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
	}
	GetWorldTimerManager().ClearTimer(SpellSkillTimer);

	AttackRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	CurrentState = EMonsterState::Dead;
}

void AReaper::CheckDistanceToPlayer()
{
	DistanceToPlayer = GetDistanceTo(CombatTarget);
}

void AReaper::ChangeToSecondPase()
{
	CurrentState = EMonsterState::ChangingPase;
	Cast<AAIController>(GetController())->StopMovement();
	DamagedEnemy.Empty();
	SpellBook->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Weapon_l"));
	AnimInstance->PlayChangeToPaseMontage();
}