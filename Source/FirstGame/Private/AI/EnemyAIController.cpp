#include "AI/EnemyAIController.h"

#include "AI/AIBlackboardKeys.h"
#include "AI/EnemyCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"

AEnemyAIController::AEnemyAIController()
{
	UAIPerceptionComponent* Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SetPerceptionComponent(*Perception);

	UAISenseConfig_Sight* Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
	Sight->SightRadius = 1200.0f;
	Sight->LoseSightRadius = 1500.0f;
	Sight->PeripheralVisionAngleDegrees = 180.0f;
	Sight->SetMaxAge(2.0f);
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = true;

	Perception->ConfigureSense(*Sight);
	Perception->SetDominantSense(Sight->GetSenseImplementation());
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UAIPerceptionComponent* Perception = GetPerceptionComponent())
	{
		Perception->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
		Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);
	if (!Enemy || !Enemy->BehaviorTree)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Behavior Tree is not assigned"), *GetNameSafe(InPawn));
		return;
	}

	if (UAIPerceptionComponent* Perception = GetPerceptionComponent())
	{
		if (UAISenseConfig_Sight* Sight = Cast<UAISenseConfig_Sight>(
				Perception->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>())))
		{
			Sight->SightRadius = Enemy->SightRadius;
			Sight->LoseSightRadius = FMath::Max(Enemy->LoseSightRadius, Enemy->SightRadius);
			Sight->PeripheralVisionAngleDegrees = Enemy->SightAngleDegrees;
			Perception->ConfigureSense(*Sight);
		}
	}

	if (!RunBehaviorTree(Enemy->BehaviorTree))
	{
		return;
	}

	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsVector(AIBlackboard::HomeLocation, Enemy->GetActorLocation());
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	const APawn* SeenPawn = Cast<APawn>(Actor);
	if (!SeenPawn || !SeenPawn->IsPlayerControlled())
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		BlackboardComp->SetValueAsObject(AIBlackboard::TargetActor, Actor);
	}
	else
	{
		BlackboardComp->ClearValue(AIBlackboard::TargetActor);
	}
}
