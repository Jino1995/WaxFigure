// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Candle.h"
#include "Water.h"
#include "TimerManager.h"
#include "AIController.h"
#include "ABAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Monster.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ECollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	ECollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Root"));
	//ECollision->InitSphereRadius(100.0f);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mesh(TEXT("/Game/Annabel/Annabel.Annabel"));
	if(SK_Mesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mesh.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> ANIM_AnimInstance(TEXT("/Game/Annabel/Animations/CharacterAnimBP.CharacterAnimBP_C")); 
	if (ANIM_AnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ANIM_AnimInstance.Class);
	}
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	SpringArm->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 450.0f;
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.f, 0.f));
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = false;
	//SpringArm->bDoCollisionTest = true;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);


	Tags.Add("Player");
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	AIController = Cast<AAIController>(GetController());

	WaterGun1 = GetWorld()->SpawnActor<AWaterGun>(BP_WaterGun, FVector::ZeroVector, FRotator::ZeroRotator);
	WaterGun1->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Gun_H"));



	Candle = GetWorld()->SpawnActor<ACandle>(BP_Candle, FVector::ZeroVector, FRotator::ZeroRotator);
	Candle->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Candle_H"));

	if (!OnHand) {
		WaterGun1->SetActorHiddenInGame(true);
		Candle->SetActorHiddenInGame(true);
	}
	else if (OnHand) {
		candlePicked = true;
		gunPicked = true;
	}

	maxcandle = candleHP;
	Candle->FireInitSize(fireSize);
	currentCoolTime = evasionCoolTime * 50;
	IsDead = false;

	ECollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ECollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ECollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ECollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ECollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	ECollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::ECollisionOnOverlapBegin);
	ECollision->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::ECollisionOnOverlapEnd);
	ECollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DirectionToMove.SizeSquared() > 0.0f)
	{
		if (!AnimInstance->IsAnyMontagePlaying()) {
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
	}
	//candleHP -= 0.05f;
	if(!bevasion) CheckCandle();
	if (evasionCoolTime*50 < currentCoolTime) {
		evasionCool();
	}
}
void APlayerCharacter::CheckCandle() {
	if (candleHP>0.0f) {
		Candle->FireSize(fireSize*((candleHP/maxcandle * 0.75)+0.25));
		Candle->CandleSize(fireSize*((candleHP / maxcandle * 0.7) + 0.3));
		//UE_LOG(LogTemp, Warning, TEXT("%f"), candleHP);
	}
	else {
		//die 처리
		IsDead = true;
	}
}
void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == AnimInstance)return;

	AnimInstance->OnAttackFire.AddLambda([this]()->void {
		Fire_Launch();
	});
	AnimInstance->OnSlideEnd.AddLambda([this]()->void {
		/*GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->GetMaxSpeed() / 1.5;
		GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->GetMaxAcceleration() / 5;*/
		bevasion = false;
	});
}
// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APlayerCharacter::Fire);
	PlayerInputComponent->BindAction(TEXT("evasion"), IE_Pressed, this, &APlayerCharacter::evasion);
	PlayerInputComponent->BindAction(TEXT("Interaction"), IE_Pressed, this, &APlayerCharacter::Interaction);
	PlayerInputComponent->BindAction(TEXT("PauseMenu"), IE_Pressed, this, &APlayerCharacter::VisiblePauseMenu);
}
float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
	if (!bevasion&&!Invincible) {
		Invincible = true;
		if (AnimInstance->PlayHitMontage()) {
			AnimInstance->JumpToHitMontageSection();
			DirectionToMove.Y = 0.0f;
			DirectionToMove.X = 0.0f;
			GetWorldTimerManager().SetTimer(InvincibleTimer, this, &APlayerCharacter::InvincibleEnd, InvincibleTime, false);
			UE_LOG(LogTemp, Warning, TEXT("InvincibleStart"));
		}
		candleHP -= DamageAmount;
		
		UE_LOG(LogTemp, Warning, TEXT("Take Damage - Player HP : %f"), candleHP);
	}
	return DamageAmount;
}
void APlayerCharacter::InvincibleEnd(){
	Invincible = false;
	GetWorldTimerManager().ClearTimer(InvincibleTimer);
	UE_LOG(LogTemp, Warning, TEXT("InvincibleEnd"));
}
void APlayerCharacter::MoveForward(float NewAxisValue)
{
	//AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), NewAxisValue);
	if (!AnimInstance->IsAnyMontagePlaying()) {
		DirectionToMove.X = NewAxisValue;
	}
}

void APlayerCharacter::MoveRight(float NewAxisValue)
{
	//AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), NewAxisValue);
	if (!AnimInstance->IsAnyMontagePlaying()) {
		DirectionToMove.Y = NewAxisValue;
	}
}
void APlayerCharacter::Fire() {
	if (gunPicked) {
		if (AnimInstance->PlayAttackMontage()) {
			DirectionToMove.Y = 0.0f;
			DirectionToMove.X = 0.0f;
		}
	}
	
}
void APlayerCharacter::Fire_Launch() {
	const FRotator rotation = GetController()->GetControlRotation();
	FVector ALocation = GetActorLocation();
	FVector MuzzleLocation(ALocation.X, ALocation.Y, ALocation.Z);
	FRotator MuzzleRotation1 = GetActorRotation();
	UWorld* World = GetWorld();
	if (World) {
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		AWater* Projectile = World->SpawnActor<AWater>(ProjectileClass, MuzzleLocation + GetActorForwardVector() * 90.0f, MuzzleRotation1, SpawnParams);
		if (Projectile)
		{
			Projectile->Fire_GetAIController(GetController(), this);
			Projectile->SetCollision();
			FVector LaunchDirection = MuzzleRotation1.Vector();
			Projectile->FireInDirection(LaunchDirection);

		}
	}
}
void APlayerCharacter::evasion() {
	//FVector a = this->GetActorForwardVector();
	if (canEvasion&&candlePicked) {
		

		/*GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->GetMaxSpeed()*1.5;
		GetCharacterMovement()->MaxAcceleration = GetCharacterMovement()->GetMaxAcceleration() * 5;*/
		if (AnimInstance->PlayAttackMontage()) {
			AnimInstance->JumpToEvasionMontageSection();
			DirectionToMove.Y = 0.0f;
			DirectionToMove.X = 0.0f;
			//DirectionToMove.Set(a.X,a.Y,a.Z);
			if (candleHP - evasionSelfDamage > 0) candleHP -= evasionSelfDamage;
			UE_LOG(LogTemp, Warning, TEXT("Shield - Player HP : %f"), candleHP);
			bevasion = true;

			Candle->FireSize(fireSize * 2);
			canEvasion = false;
			currentCoolTime = 0;
			GetWorldTimerManager().SetTimer(evasionTimer, this, &APlayerCharacter::evasionCoolDown, 0.02f, true);
		}
		
	}
	
}
void APlayerCharacter::evasionCoolDown() {
	currentCoolTime++;
}
void APlayerCharacter::evasionCool() {
	GetWorldTimerManager().ClearTimer(evasionTimer);
	canEvasion = true;
}
void APlayerCharacter::SetMonster(AMonster* NewTarget)
{
	Monster = NewTarget;
}

void APlayerCharacter::ECollisionOnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		UE_LOG(LogTemp, Warning, TEXT("OverlapBegin : %s "), *OtherActor->GetName());
		for (int32 Index = 0; Index != objectNameArr.Num(); ++Index) {
			if (OtherActor->GetName().Contains(objectNameArr[Index]) && (ActivedObjectNameArr.Find(OtherActor->GetName()) == INDEX_NONE)  ) {
				if (OtherActor->GetName().Contains(TEXT("WaterGun"))|| OtherActor->GetName().Contains(TEXT("PickableCandle"))) {
					PickableObject.AddUnique(OtherActor);
				}
				UE_LOG(LogTemp, Warning, TEXT("objectAdded : %s num : %d"), *OtherActor->GetName(), objectArr.Num());
				objectArr.AddUnique(OtherActor);
				break;
			}
		}

		
	}
	
}
void APlayerCharacter::ECollisionOnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		/*if (OtherActor->GetName().Contains(TEXT("Candle_BP"))) {
			objectNameArr.Find(OtherActor->GetName());
			objectNum--;
			if (objectNum < 0) objectNum = 0;
			UE_LOG(LogTemp, Warning, TEXT("objectNum : %f"), objectNum);
		}*/
		
		for (int32 Index = 0; Index != objectNameArr.Num(); ++Index) {
			if (OtherActor->GetName().Contains(objectNameArr[Index])  ) {
				if (OtherActor->GetName().Contains(objectNameArr[Index])) {
					UE_LOG(LogTemp, Warning, TEXT("objectRemoved : %s num : %d"), *OtherActor->GetName(), objectArr.Num());
					if (OtherActor->GetName().Contains(TEXT("WaterGun")) || OtherActor->GetName().Contains(TEXT("PickableCandle")))
						PickableObject.Remove(OtherActor);
					objectArr.Remove(OtherActor);
					break;
				}
			}
		}
	}
}

void APlayerCharacter::Interaction() {
	if (!AnimInstance->IsAnyMontagePlaying()) {
		if (objectArr.Num() >= 1) {
			
			if (   (objectArr[0]->GetName().Contains(TEXT("WaterGun"))&& PickableObject[0]->GetName().Contains(TEXT("WaterGun"))&& gunPicked==false)    ||  (objectArr[0]->GetName().Contains(TEXT("PickableCandle"))&& PickableObject[0]->GetName().Contains(TEXT("PickableCandle")) && candlePicked == false)  ) {
				
			
					UE_LOG(LogTemp, Warning, TEXT("PickableObject num : %d"), PickableObject.Num());
					
					if (PickableObject[0]->GetName().Contains(TEXT("WaterGun"))) {
						if (AnimInstance->PlayAttackMontage()) {
							AnimInstance->JumpToPickMontageSection();
							DirectionToMove.Y = 0.0f;
							DirectionToMove.X = 0.0f;
						}

						WaterGun1->SetActorHiddenInGame(false);
						//총 보이게하기
						gunPicked = true;
					}else if (PickableObject[0]->GetName().Contains(TEXT("PickableCandle"))) {
						if (AnimInstance->PlayAttackMontage()) {
							AnimInstance->JumpToPick2MontageSection();
							DirectionToMove.Y = 0.0f;
							DirectionToMove.X = 0.0f;
						}

						Candle->SetActorHiddenInGame(false);
						//양초 보이게하기
						candlePicked = true;
					}

					PickableObject[0]->SetActorHiddenInGame(true);
					PickableObject.RemoveAt(0);
				

			}
			else if(!objectArr[0]->GetName().Contains(TEXT("WaterGun"))&& !objectArr[0]->GetName().Contains(TEXT("PickableCandle"))&& candlePicked){
				/*FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), objectArr[0]->GetActorLocation());
				FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
				SetActorRotation(LookAtRotationYaw);*/
				if (AnimInstance->PlayAttackMontage()) {
					AnimInstance->JumpToTurnMontageSection();
					DirectionToMove.Y = 0.0f;
					DirectionToMove.X = 0.0f;
					UE_LOG(LogTemp, Warning, TEXT("Interaction num : %d"), objectArr.Num());
					ActivedObjectNameArr.Add(objectArr[0]->GetName());

				}
			}

			
			objectArr.RemoveAt(0);


		}
	}
	//if(candleNum>=1) -> 양초 가져다대기 애니메이션
	//if(gunNum>=1) -> 물총 집는 애니메이션

}
void APlayerCharacter::VisiblePauseMenu()
{
	//Cast<AABPlayerController>(GetController())->VisiblePauseMenu();
}