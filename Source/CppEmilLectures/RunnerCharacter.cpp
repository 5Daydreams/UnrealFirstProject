// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Spikes.h"
#include "WallSpikes.h"
#include "Engine.h"

// Sets default values
ARunnerCharacter::ARunnerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SideViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Side View Camera"));

	SideViewCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.0f);

	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.8f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	tempPos = GetActorLocation();

	zPosition = tempPos.Z + 300.f;

}

// Called when the game starts or when spawned
void ARunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.
		AddDynamic(this, &ARunnerCharacter::OnOverlapBegin);

	CanMove = true;
}

// Called every frame
void ARunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	tempPos = GetActorLocation();
	tempPos.X -= 850.0f;
	tempPos.Z = zPosition;
	SideViewCamera->SetWorldLocation(tempPos);

}

// Called to bind functionality to input
void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveRight", this, &ARunnerCharacter::MoveHorizontal);

}

void ARunnerCharacter::MoveHorizontal(float value)
{
	if (CanMove)
	{
		AddMovementInput(FVector(0.0f, 1.0f, 0.0f), value);
	}
}

void ARunnerCharacter::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void ARunnerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		ASpikes* WallSpikes = Cast<AWallSpikes>(OtherActor);
		ASpikes* Spike = Cast<ASpikes>(OtherActor);

		if (WallSpikes || Spike)
		{
			GetMesh()->Deactivate();
			GetMesh()->SetVisibility(false);

			CanMove = false;

			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &ARunnerCharacter::RestartLevel, 2.f, false);
		}
	}
}
