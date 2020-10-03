// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Client_01Character.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Server_Client.h"
#include "Client_01AnimInstance.h"

//////////////////////////////////////////////////////////////////////////
// AClient_01Character

AClient_01Character::AClient_01Character()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CHARMESH(TEXT("/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin"));
	if (CHARMESH.Succeeded()) GetMesh()->SetSkeletalMesh(CHARMESH.Object);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeLocationAndRotation(FVector(-100.0f, 0.0f, 200.0f), FRotator(-10.0f, 0.0f, 0.0f));
	
	// Anim Instance Initialize
	static ConstructorHelpers::FClassFinder<UAnimInstance> BP_ANIM_CHAR(TEXT("/Game/BP_Client01_AnimIns.BP_Client01_AnimIns_C"));
	if (BP_ANIM_CHAR.Succeeded()) GetMesh()->SetAnimInstanceClass(BP_ANIM_CHAR.Class);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	SocketClass = NULL;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AClient_01Character::BeginPlay()
{
	Super::BeginPlay();

	// Animation Setting
	AnimIns = Cast<UClient_01AnimInstance>(GetMesh()->GetAnimInstance());

	// Data Setting
	iPlayerNumber = 1;
	myPlayer = false;
	TempPos = GetActorLocation();
	TempRot = GetActorRotation();
}

void AClient_01Character::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);
}

void AClient_01Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	//print(__FUNCTION__);
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AClient_01Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AClient_01Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AClient_01Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AClient_01Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AClient_01Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AClient_01Character::TouchStopped);
}

void AClient_01Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (myPlayer)		
	{
		// Send My Pos and Rot
		if ((TempPos != GetActorLocation()) || (TempRot != GetActorRotation()))
		{
			TempPos = GetActorLocation();
			TempRot = GetActorRotation();

			SocketClass->SendData_PosAndRot(iPlayerNumber, GetActorLocation(), GetActorRotation());	
		}
	}
}

void AClient_01Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (myPlayer) Jump();
}

void AClient_01Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (myPlayer) StopJumping();
}

void AClient_01Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (myPlayer) AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AClient_01Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (myPlayer) AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AClient_01Character::MoveForward(float Value)
{
	if (myPlayer)
	{
		if ((Controller != NULL) && (Value != 0.0f))
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Value);

			// Animation
			AnimIns->Is_Walk = true;
		}
		else
		{
			// Animation
			AnimIns->Is_Walk = false;
		}
	}
}

void AClient_01Character::MoveRight(float Value)
{
	if (myPlayer)
	{
		if ((Controller != NULL) && (Value != 0.0f))
		{
			// find out which way is right
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get right vector 
			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			// add movement in that direction
			AddMovementInput(Direction, Value);

			// Animation
			AnimIns->Is_Walk = true;
		}
		else
		{
			// Animation
			AnimIns->Is_Walk = false;
		}
	}
}
