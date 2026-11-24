#include "AI/BTTask_FindSuspicionLocation.h"

#include "AI/AIBlackboardKeys.h"
#include "AI/EnemyCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindSuspicionLocation::UBTTask_FindSuspicionLocation()
{
	NodeName = TEXT("Find Suspicion Location");
	SuspectActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindSuspicionLocation, SuspectActorKey), AActor::StaticClass());
	MoveLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindSuspicionLocation, MoveLocationKey));
	SuspectActorKey.SelectedKeyName = AIBlackboard::SuspectActor;
	MoveLocationKey.SelectedKeyName = AIBlackboard::MoveLocation;
}

EBTNodeResult::Type UBTTask_FindSuspicionLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	APawn* Self = Controller ? Controller->GetPawn() : nullptr;
	UNavigationSystemV1* NavSys = Controller ? UNavigationSystemV1::GetCurrent(Controller->GetWorld()) : nullptr;
	if (!Self || !BlackboardComp || !NavSys)
	{
		return EBTNodeResult::Failed;
	}

	const AActor* Suspect = Cast<AActor>(BlackboardComp->GetValueAsObject(SuspectActorKey.SelectedKeyName));
	if (!Suspect)
	{
		return EBTNodeResult::Failed;
	}

	const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Self);
	const float MinDistance = Enemy ? Enemy->SuspicionMinDistance : 1200.0f;
	const float SuspicionRange = Enemy ? Enemy->SuspicionRange : 2000.0f;
	const float MinRadius = Enemy ? Enemy->SuspicionMinRadius : 300.0f;
	const float MaxRadius = Enemy ? Enemy->SuspicionMaxRadius : 1000.0f;

	const float Distance = static_cast<float>(FVector::Dist(Self->GetActorLocation(), Suspect->GetActorLocation()));
	const float Radius = FMath::GetMappedRangeValueClamped(
		FVector2f(MinDistance, SuspicionRange),
		FVector2f(MinRadius, MaxRadius),
		Distance);

	FNavLocation Result;
	if (!NavSys->GetRandomReachablePointInRadius(Suspect->GetActorLocation(), FMath::Max(Radius, 100.0f), Result))
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComp->SetValueAsVector(MoveLocationKey.SelectedKeyName, Result.Location);
	return EBTNodeResult::Succeeded;
}
