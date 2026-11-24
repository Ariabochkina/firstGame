#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateSuspicion.generated.h"

UCLASS()
class FIRSTGAME_API UBTService_UpdateSuspicion : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateSuspicion();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SuspectActorKey;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDrawDebug = false;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
