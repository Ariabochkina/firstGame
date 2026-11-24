// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "Engine/Engine.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	Stamina = MaxStamina;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const bool bIsMoving = GetVelocity().SizeSquared2D() > 1.0f;
	
	if (bIsSprinting && bIsMoving)
	{
		Stamina = FMath::Max(0.0f, Stamina - StaminaDrainRate * DeltaTime);
		
		if (Stamina <= 0.0f)
		{
			StopSprint();
		}
	}
	else
	{
		Stamina = FMath::Min(MaxStamina, Stamina + StaminaRegenRate * DeltaTime);
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow,
			FString::Printf(TEXT("Stamina: %.0f / %.0f"), Stamina, MaxStamina));
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInput)
	{
		if (SprintAction)
		{
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started , this, &AMyCharacter::StartSprint);
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed , this, &AMyCharacter::StopSprint);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started , this, &AMyCharacter::Interact);
		}
	}
}

void AMyCharacter::StartSprint()
{
	if (Stamina < MinStaminaToSprint)
	{
		return;
	}
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMyCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMyCharacter::Interact()
{
	OnInteract.Broadcast();
}