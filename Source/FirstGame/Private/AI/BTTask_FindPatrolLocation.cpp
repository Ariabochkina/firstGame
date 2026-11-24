#include "AI/BTTask_FindPatrolLocation.h"

#include "AI/AIBlackboardKeys.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation()
{
	NodeName = TEXT("Find Patrol Location");
	HomeLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindPatrolLocation, HomeLocationKey));
	MoveLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FindPatrolLocation, MoveLocationKey));
	HomeLocationKey.SelectedKeyName = AIBlackboard::HomeLocation;
	MoveLocationKey.SelectedKeyName = AIBlackboard::MoveLocation;
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	UNavigationSystemV1* NavSys = Controller ? UNavigationSystemV1::GetCurrent(Controller->GetWorld()) : nullptr;
	if (!Controller || !BlackboardComp || !NavSys || !Pawn)
	{
		return EBTNodeResult::Failed;
	}

	FVector Origin = BlackboardComp->GetValueAsVector(HomeLocationKey.SelectedKeyName);
	FNavLocation Projected;
	if (NavSys->ProjectPointToNavigation(Origin, Projected))
	{
		Origin = Projected.Location;
	}
	else if (NavSys->ProjectPointToNavigation(Pawn->GetActorLocation(), Projected))
	{
		Origin = Projected.Location;
	}

	FNavLocation Result;
	if (!NavSys->GetRandomReachablePointInRadius(Origin, PatrolRadius, Result))
	{
		if (!NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), PatrolRadius, Result))
		{
			return EBTNodeResult::Failed;
		}
	}

	BlackboardComp->SetValueAsVector(MoveLocationKey.SelectedKeyName, Result.Location);
	return EBTNodeResult::Succeeded;
}
