#include "AI/EnemyCharacter.h"

#include "AI/EnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;

	StateText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StateText"));
	StateText->SetupAttachment(GetCapsuleComponent());
	StateText->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	StateText->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	StateText->SetHorizontalAlignment(EHTA_Center);
	StateText->SetVerticalAlignment(EVRTA_TextCenter);
	StateText->SetWorldSize(56.0f);
	StateText->SetTextRenderColor(FColor::Yellow);
	StateText->SetText(FText::GetEmpty());
}

void AEnemyCharacter::SetStateText(const FString& Text)
{
	if (StateText)
	{
		StateText->SetText(FText::FromString(Text));
	}
}
