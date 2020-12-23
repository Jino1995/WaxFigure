# Wax Figure
![20201222_172522](https://user-images.githubusercontent.com/74805177/102866565-cebb1c80-447a-11eb-8391-fc4d30796f54.png)

이 소스 코드는 ue4 기반 퍼즐 어드벤처 장르의 게임을 구현하는 데 사용되었으며 여기에서는 몬스터 코드에 대해 설명합니다.

## Overview
![MonsterStruct](https://user-images.githubusercontent.com/74805177/102843301-402fa680-444c-11eb-9628-e7a3b772a67b.png)

위 그림은 몬스터를 만드는 데 사용 된 클래스들의 구조입니다.

## 일반 몬스터
#### 일반 몬스터는 모두 유사하게 구현되어 있으므로 일반 몬스터 중 하나 인 Police Zombie의 코드로 설명하겠습니다.
### 1. 플레이어 감지
<img src="https://user-images.githubusercontent.com/74805177/102860269-d37ad300-4470-11eb-9089-4bd38a44ea11.gif" width="50%" height="30%"> 

```c++
void APoliceZombie::OnSeePawn(APawn *OtherPawn)
{
	Sensing->bSeePawns = false;
	CombatTarget = Cast<APlayerCharacter>(OtherPawn);
	CurrentState = EMonsterState::Hunting;
}
```
위 코드는 플레이어가 몬스터의 시야 범위(PawnSensingComponent 사용)에 들어왔을 때 호출이 되는 메서드로 PawnSensingComponent인 Sensing의 bSeePawns 설정을 false로 바꿈으로써, OnSeePawn 메서드는 한 번만 호출이 되도록 한다. CombatTarget 변수의 값을 시야 범위로 들어온 플레이어로 값을 할당하여 몬스터의 공격 목표가 플레이어가 되도록 설정을 하고, 플레이어의 현재 상태를 나타내는 변수인 CurrentState를 EMonsterState::Idle에서 EMonsterState::Hunting으로 바꿔준다.

### 2. 플레이어에게 접근
<img src="https://user-images.githubusercontent.com/74805177/102861346-dbd40d80-4472-11eb-9243-16b57f1f9558.gif" width="50%" height="30%">

```c++
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
```
플레이어가 몬스터의 시야 범위로 들어와 몬스터가 플레이어를 인식하게 되었을 경우 OnSeePawn()에서 CurrentState 값이 EMonsterHunting으로 바뀌게 되는데 이로 인해 매 틱마다 호출이 되는 메서드인 Tick()에서 CanMove()의 실행결과가 true로 반환이 되고 2번째 if문 또한 OnSeePawn()에서 CombatTarget이 플레이어로 설정이 되었으므로 항상 true가 나오게 된다.

이렇게 두 if문을 만족하게 되면 FindLookAtRotation() 을 통해 현재 몬스터의 정면을 기준으로 타겟 플레이어의 위치 Rotation을 알아내고 알아낸 값을 기반으로 몬스터의 Rotation 값을 변경하여 매 틱마다 몬스터가 항상 플레이어를 바라보도록 설정하였다.


### 3. 공격
<img src="https://user-images.githubusercontent.com/74805177/102860519-60259100-4471-11eb-80a3-8090346528f3.gif" width="50%" height="30%">

```c++
void APoliceZombie::AttackRangeOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto OverlapTarget = Cast<APlayerCharacter>(OtherActor);
	if (OverlapTarget)
	{
		GetWorldTimerManager().SetTimer(AttackTimer, this, &APoliceZombie::EXAttack, 0.1f, true);
		OverlapTarget->SetMonster(this);
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

void APoliceZombie::PostInitializeComponents()
{
	...
	
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

	...
}

void APoliceZombie::EXApplyDamage()
{
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
```
 AttackRangeOnOverlapBegin()는 몬스터의 공격 실행 범위인 사각형(BoxComponent)에 어떤 물체가 Overlap 되었을 경우에 실행이 된다. 만약 Overlap된 물체(OverlapTarget)가 PlayerCharacter일 경우에는 타이머를 설정하여 0.1초마다 공격을 실행하는 EXAttack()을 호출하도록 하였다. EXAttack()이 호출되었을 경우 CanAttack()을 호출하여 현재 몬스터가 공격이 가능한 상태인지를 확인을 한 후 공격이 가능한 상태라면 AnimInstance의 AttackMontage를 실행을 시킨다. 
 
실행 중인 AttackMontage가 AttackCheck 노티파이를 발생시키면 PostInitializeComponents에서 등록한 람다식이 실행하게 되고, 몬스터가 죽은 상태가 아니라면 EXApplyDamage() 메서드가 실행이 되고, BoxTraceMultiForObject()를 통해 사각형의 정해진 범위 내에 있는 물체를 탐지하고 탐지를 한 결과를 bResult에 저장을 하게 되는데 범위 내에 플레이어가 아닌 다른 물체는 무시하고, 플레이어가 감지되었다면 몬스터는 ApplyDamage()를 통해 플레이어에게 데미지를 주게 된다. 
 
여기서 플레이어가 중복이 되어 인식이 될 수 있음으로 데미지가 적용이 된 플레이어는 TArray에 넣어 이 TArray에 있는 플레이어는 데미지를 입지 않도록 하였다. 몬스터의 AttackMontage가 끝이 났을 경우 AttackEnd 노티파이 발생 실행을 하도록 등록된 람다식이 호출이 되며, 몬스터의 상태를 다시 Hunting으로 변경을 하고, 데미지를 입은 플레이어가 담겨있는 TArray 변수인 DamagedEnemy를 비워 다음에 공격하였을 경우 해당 플레이어에게 데미지 적용이 가능하도록 하였다.
 
 
 ### 4. 데미지/죽음
 <img src="https://user-images.githubusercontent.com/74805177/102861210-a29b9d80-4472-11eb-9fef-f5c81e0c8106.gif" width="50%" height="30%">

```c++
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
```
TakeDamage()는 몬스터가 플레이어의 ApplyDamage()의 호출로 인해 데미지를 입었을 경우 호출이 되는 메서드로 몬스터가 죽은 상태(EMonsterState::Dead)일 경우에는 아무런 반응이 없게 하고, 몬스터가 플레이어를 인식하지 못한 상황에서 플레이어에게 데미지를 받았을 경우 CombatTarget을 공격자로 설정을 하여 몬스터가 데미지를 가한 플레이어를 인식하도록 하였다. 

몬스터의 체력을 받은 데미지 만큼 감소시키고 이때 몬스터의 체력이 0이하일 경우, Dead() 메서드를 호출하게 된다. 

이때 몬스터는 죽었을 때의 소리를 발생시키며, Attack Timer가 실행 중이라면 Attack 타이머를 중단 시키고 CapsuleComponent의 콜리전은 끄고, Mesh의 콜리전은 On 함으로써, 몬스터의 시체가 플레이어의 경로를 방해하도록 설정을 하였다.



## 보스 몬스터
#### 보스 몬스터의 코드는(감지, 추적, 일반 공격, 데미지/죽음) 일반 몬스터와 유사하게 구현되어 있으므로 여기서는 일반 몬스터와 차별화가 되는 보스몬스터의 스킬 공격에 대한 설명하겠습니다. 
### 1. 대시 공격
<img src="https://user-images.githubusercontent.com/74805177/102864507-949c4b80-4477-11eb-8ced-2314ff43523e.gif" width="50%" height="30%">

```c++
void AReaper::Tick(float DeltaTime)
{
	...
	
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
```
보스 몬스터는 일반 몬스터와는 달리 Tick()에서 추가적으로 특수한 공격을 실행하는 코드가 들어간다. 

플레이어와 보스 몬스터의 거리가 500 이상일 경우 대시 공격을 실행하도록 하였으며, 대시 공격을 실행한 후에는 이 공격을 연속으로 사용할 수 없도록 쿨타임을 적용을 하였는데 타이머를 설정을 하여 사용자가 블루프린트에서 설정한 쿨타임이 지나면 다시 대시 공격을 할 수 있도록 boolean 변수를 true로 만들어주는 메서드를 실행하도록 하였다.


### 2. 2페이즈
<img src="https://user-images.githubusercontent.com/74805177/102864805-1ee4af80-4478-11eb-9df0-e1c9ed4e19a5.gif" width="50%" height="30%">

```c++
float AReaper::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	...
	
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
	
	...
}

void AReaper::ChangeToSecondPase()
{
	CurrentState = EMonsterState::ChangingPase;
	Cast<AAIController>(GetController())->StopMovement();
	DamagedEnemy.Empty();
	SpellBook->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Weapon_l"));
	AnimInstance->PlayChangeToPaseMontage();
}

void AReaper::PostInitializeComponents()
{
	...
	
	AnimInstance->EndPaseChanged.AddLambda([this]() -> void {
		CurrentState = EMonsterState::Hunting;
		GetWorldTimerManager().SetTimer(SpellSkillTimer, this, &AReaper::EXSpell, 3.f, true);
	});
	
	...
}
```
보스 몬스터는 낫을 휘두르는 물리 공격만을 하는 1페이즈와, 마법 공격을 병행하며 공격을 하는 2페이즈로 나뉘게 된다.

1페이즈 상태에서 공격을 받았을 때 일정 이하의 체력 퍼센트가 남아있게 된다면 2페이즈로 돌입을 하게 되며, 이때 ChangeToSecondPase() 메서드를 통해 허리에 있던 책의 위치가 왼손 쪽으로 변경이 되며, 책을 펼치는 AnimMontage가 실행이 된다. 

페이즈로 변경을 하는 AnimMontage가 끝나 EndPaseChanged 노티파이가 발생을 하면 PostInitializeComponents()에서 등록한 람다식이 실행이 되어 같이 보스 몬스터의 상태를 다시 Hunting으로 변경을 한 후, 타이머를 동작 시켜 일정한 시간마다 스킬을 사용하는 메서드가 실행이 되도록 하였다.


### 3. 소환 스킬
<img src="https://user-images.githubusercontent.com/74805177/102865488-2a84a600-4479-11eb-9d09-652025295bfc.gif" width="50%" height="30%">

```c++
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
			
			...
		}
	}
}

void AReaper::PostInitializeComponents()
{
	...
	
	AnimInstance->StartSummon.AddLambda([this]() -> void {
		EXSummon();
	});

	AnimInstance->EndSummon.AddLambda([this]() -> void {
		SummonPSComp->DestroyComponent();
		CurrentState = EMonsterState::Hunting;
		GetWorldTimerManager().SetTimer(SummonCoolTimeTimer, this, &AReaper::OnSummonCoolTime, 30.f, false);
	});
	
	...
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
```
타이머가 동작하여 스킬을 사용하기 위해 EXSpell()이 실행이 되고 랜덤 난수에서 1이 나오면 해골 몬스터를 소환하는 스킬이 발동이 된다. 

우선 쿨타임을 위한 boolean 변수를 false로 만든 후, 보스 몬스터의 바닥에 붉은색의 마법 진 파티클을 생성하고, 소환 AnimMontage가 실행이 된다. 소환을 시작하는 AnimMontage가 실행하는 도중 특정 구간에서 StartSummon 노티파이가 발생을 하게 되는데 이때 PostInitializeComponents()에 등록한 람다식이 실행이 되어 실질적으로 몬스터를 소환하는 코드가 담긴 EXSummon() 메서드가 실행이 된다.

EXSummon()은 실질적으로 몬스터를 소환하는 코드로 보스 몬스터가 존재하는 네비게이션 시스템 안에서 GetRandomPointNavigableRadius()를 통해 보스 몬스터를 중심으로 특정 구간 내에 랜덤으로 위치 값을 얻어내고, 이렇게 얻어낸 위치 값으로 SummonSkeletonMan() 메서드를 호출하여 해골을 소환하여 성공 시 true를 실패 시 false를 반환하게 된다. 성공적으로 소환을 하여 true를 반환할 경우 소환하고자 하는 몬스터의 수를 담은 변수인 SummonNum 값이 하나씩 감소가 되며 이 변수의 값이 0이 되면 소환이 끝이 나게 된다.


### 4. 다크볼 스킬
<img src="https://user-images.githubusercontent.com/74805177/102865560-4b4cfb80-4479-11eb-826e-186af9bcffa3.gif" width="50%" height="30%">

```c++
void AReaper::EXSpell()
{
	...
	
	if (CurrentState != EMonsterState::Skill && CurrentState != EMonsterState::Attack)
	{
		Cast<AAIController>(GetController())->StopMovement();
		int32 SelectNum = rand() % 3 + 1;

		switch (SelectNum)
		{
			...
		
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
			
			...
		}
	}
}

void AReaper::PostInitializeComponents()
{
	...

	AnimInstance->ShotDarkBall.AddLambda([this]() -> void {
		CurrentState = EMonsterState::Skill;
		EXDarkBall();
	});

	AnimInstance->EndDarkBall.AddLambda([this]() -> void {
		DarkBallPSComp->DestroyComponent();
		CurrentState = EMonsterState::Hunting;
		GetWorldTimerManager().SetTimer(DarkBallCoolTimeTimer, this, &AReaper::OnDarkBallCoolTime, 3.f, false);
	});
	
	...
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
```
만약 랜덤 난수가 2가 나왔을 경우 다크볼 스킬을 사용을 하게 된다. 

다른 기술들과 마찬가지로 쿨타임 boolean 변수를 false로 만들고, 보스 몬스터의 바닥에 마법진 파티클을 생성 후, 다크볼을 날리는 AnimMontage를 실행을 한다. 다크볼을 날리는 AnimMontage를 실행하는 도중 다크볼을 날려야 하는 지점에 들어서게 되면 ShotDarkBall 노티파이가 발생을 하게 되는데, 이때 PostInitializeComponents()에 등록된 람다식을 실행하여, 다크볼을 날리는 EXDarkBall()을 호출을 한다. 

EXDarkBall()은 다크볼을 생성하여 몬스터의 전방으로 발사를 하는 코드로 다크볼이 생성되는 위치를 보스 몬스터의 전방에 위치하는 DarkBallHand 소켓으로 지정을 한 후, ProjectileMovement를 가진 DarkBall 객체를 소환을 하고, FireDirection()의 호출로 발사 방향을 지정을 하여 발사를 한다.


### 5. 폭발룬 스킬
<img src="https://user-images.githubusercontent.com/74805177/102865604-599b1780-4479-11eb-9e9b-9cfde5e7f003.gif" width="50%" height="30%">

```c++
void AReaper::EXSpell()
{
	if (CurrentState != EMonsterState::Skill && CurrentState != EMonsterState::Attack)
	{
		Cast<AAIController>(GetController())->StopMovement();
		int32 SelectNum = rand() % 3 + 1;

		switch (SelectNum)
		{
			...
			
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

void AReaper::PostInitializeComponents()
{
	...
	
	AnimInstance->StartRune.AddLambda([this]() -> void {
		EXRune();
	});

	AnimInstance->EndRune.AddLambda([this]() -> void {
		RunePSComp->DestroyComponent();
		CurrentState = EMonsterState::Hunting;
		GetWorldTimerManager().SetTimer(RuneCoolTimeTimer, this, &AReaper::OnRuneCoolTime, 10.f, false);
	});
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
```
만약 랜덤 난수가 3이 나왔을 경우 폭발 룬 스킬을 사용을 하게 된다. 

다른 기술들과 마찬가지로 쿨타임 boolean 변수를 false로 만들고, 보스 몬스터의 바닥에 마법진 파티클을 생성 후, 폭발 룬을 소환하는 AnimMontage를 실행을 한다. 폭발 룬을 소환하는 AnimMontage를 실행하는 도중 폭발 룬을 소환해야 하는 지점에 들어서게 되면 StartRune 노티파이가 발생을 하게 되는데, 이때 PostInitializeComponents()에 등록된 람다식을 실행하여, 폭발 룬을 소환하는 EXRune()을 호출한다. 

ExRune()는 폭발 룬을 소환하는 메서드로 해골을 소환하는 코드와 동일하게 네비게이션 시스템 내에서 보스 몬스터 주위로 특정 범위 내에 폭발 룬을 소환을 한다.
