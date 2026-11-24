#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindSuspicionLocation.generated.h"

UCLASS()
class FIRSTGAME_API UBTTask_FindSuspicionLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindSuspicionLocation();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SuspectActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MoveLocationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
